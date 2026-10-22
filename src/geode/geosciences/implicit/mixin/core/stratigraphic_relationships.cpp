/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.hpp>

#include <fstream>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/sparse_attribute.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/bitsery_archive.hpp>

#include <geode/mesh/builder/geode/geode_graph_builder.hpp>
#include <geode/mesh/core/bitsery_archive.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/graph_output.hpp>

#include <geode/model/mixin/core/bitsery_archive.hpp>
#include <geode/model/mixin/core/detail/relationships_impl.hpp>

namespace geode
{
    class StratigraphicRelationships::Impl : public detail::RelationshipsImpl
    {
    public:
        static constexpr local_index_t ABOVE_EDGE_VERTEX{ 0 };
        static constexpr local_index_t UNDER_EDGE_VERTEX{ 1 };

        Impl() : RelationshipsImpl()
        {
            initialize_relation_attributes();
        }

        Impl( BITSERY bitsery ) : RelationshipsImpl( bitsery ) {}

        bool is_above( const uuid& above_id, const uuid& under_id ) const
        {
            auto current = under_id;
            for( [[maybe_unused]] const auto step :
                Range{ this->graph().nb_vertices() } )
            {
                const auto directly_above = this->above( current );
                if( !directly_above )
                {
                    return false;
                }
                if( directly_above.value() == above_id )
                {
                    return true;
                }
                current = directly_above.value();
            }
            return false;
        }

        bool is_directly_above( const uuid& above, const uuid& under ) const
        {
            const auto edge_id = relation_edge( above, under );
            if( !edge_id )
            {
                return false;
            }
            if( !above_relations_->value( edge_id.value() ) )
            {
                return false;
            }
            return graph_component_id( { edge_id.value(), ABOVE_EDGE_VERTEX } )
                       .id
                   == above;
        }

        std::optional< uuid > above( const uuid& element ) const
        {
            const auto index_from = vertex_id( element );
            if( !index_from )
            {
                return std::nullopt;
            }
            for( const auto& edge_vertex :
                this->graph().edges_around_vertex( index_from.value() ) )
            {
                if( !above_relations_->value( edge_vertex.edge_id ) )
                {
                    continue;
                }
                if( edge_vertex.vertex_id == UNDER_EDGE_VERTEX )
                {
                    return this->graph_component_id( edge_vertex.opposite() )
                        .id;
                }
            }
            return std::nullopt;
        }

        std::optional< uuid > under( const uuid& element ) const
        {
            const auto index_from = vertex_id( element );
            if( !index_from )
            {
                return std::nullopt;
            }
            for( const auto& edge_vertex :
                this->graph().edges_around_vertex( index_from.value() ) )
            {
                if( !above_relations_->value( edge_vertex.edge_id ) )
                {
                    continue;
                }
                if( edge_vertex.vertex_id == ABOVE_EDGE_VERTEX )
                {
                    return this->graph_component_id( edge_vertex.opposite() )
                        .id;
                }
            }
            return std::nullopt;
        }

        index_t add_above_relation(
            const ComponentID& above, const ComponentID& under )
        {
            if( const auto id = relation_edge( above.id, under.id ) )
            {
                above_relations_->set_value( id.value(), true );
                return id.value();
            }
            OpenGeodeGeosciencesImplicitException::check_exception(
                !would_create_cycle( above.id, under.id ), nullptr,
                OpenGeodeException::TYPE::data,
                "[StratigraphicRelationships::add_above_relation] Cannot set ",
                above.string(), " above ", under.string(),
                ": it would create a cycle in the above/under relations" );
            const auto index = add_relation_edge( above, under );
            above_relations_->set_value( index, true );
            return index;
        }

        void remove_above_relation( const uuid& id1, const uuid& id2 )
        {
            auto id = relation_edge( id1, id2 );
            if( !id )
            {
                id = relation_edge( id2, id1 );
                if( !id )
                {
                    return;
                }
            }
            this->remove_relation_edge( id.value() );
        }

        void copy( const Impl& impl, const ModelCopyMapping& mapping )
        {
            detail::RelationshipsImpl::copy( impl, mapping );
            initialize_relation_attributes();
        }

        void save( std::string_view directory ) const
        {
            const auto filename =
                absl::StrCat( directory, "/stratigraphic_relationships" );
            std::ofstream file{ filename, std::ofstream::binary };
            TContext context{};
            BitseryExtensions::register_serialize_pcontext(
                std::get< 0 >( context ) );
            Serializer archive{ context, file };
            archive.object( *this );
            archive.adapter().flush();
            OpenGeodeGeosciencesImplicitException::check_exception(
                std::get< 1 >( context ).isValid(), nullptr,
                OpenGeodeException::TYPE::internal,
                "[Relationships::save] Error while writing file: ", filename );
        }

        void load( std::string_view directory )
        {
            const auto filename =
                absl::StrCat( directory, "/stratigraphic_relationships" );
            std::ifstream file{ filename, std::ifstream::binary };
            TContext context{};
            BitseryExtensions::register_deserialize_pcontext(
                std::get< 0 >( context ) );
            Deserializer archive{ context, file };
            archive.object( *this );
            const auto& adapter = archive.adapter();
            OpenGeodeGeosciencesImplicitException::check_exception(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                nullptr, OpenGeodeException::TYPE::internal,
                "[Relationships::load] Error while reading file: ", filename );
            remove_cycles();
        }

    private:
        void initialize_relation_attributes()
        {
            const auto ids =
                relation_attribute_manager().attribute_ids_matching_name(
                    "geode_above_relations" );
            if( ids.has_value() )
            {
                above_relations_ = relation_attribute_manager()
                                       .find_attribute< SparseAttribute, bool >(
                                           ids.value()[0] );
                return;
            }
            AttributeValues< bool > above_relations_default_values;
            above_relations_default_values.default_value = false;
            AttributeProperties above_relations_properties;
            above_relations_properties.assignable = false;
            above_relations_properties.interpolable = false;
            above_relations_properties.transferable = true;
            const auto above_relations_id =
                relation_attribute_manager()
                    .create_attribute< SparseAttribute, bool >(
                        "geode_above_relations", above_relations_default_values,
                        above_relations_properties );
            above_relations_ = relation_attribute_manager()
                                   .find_attribute< SparseAttribute, bool >(
                                       above_relations_id );
        }

        /*!
         * Removes the above relations closing a cycle (corrupted data).
         * Iterative depth-first search following the above relations upward:
         * any relation leading to a vertex in the current path closes a cycle.
         */
        void remove_cycles()
        {
            const auto& graph = this->graph();
            const auto nb_vertices = graph.nb_vertices();
            constexpr local_index_t NOT_VISITED{ 0 };
            constexpr local_index_t IN_PATH{ 1 };
            constexpr local_index_t DONE{ 2 };
            std::vector< local_index_t > status( nb_vertices, NOT_VISITED );
            std::vector< bool > to_delete( graph.nb_edges(), false );
            bool has_cycle{ false };
            // Stack of (vertex, next edge around vertex to process)
            std::vector< std::pair< index_t, index_t > > path;
            for( const auto start : Range{ nb_vertices } )
            {
                if( status[start] != NOT_VISITED )
                {
                    continue;
                }
                status[start] = IN_PATH;
                path.emplace_back( start, 0 );
                while( !path.empty() )
                {
                    auto& [vertex, next_edge] = path.back();
                    const auto& edges = graph.edges_around_vertex( vertex );
                    if( next_edge >= edges.size() )
                    {
                        status[vertex] = DONE;
                        path.pop_back();
                        continue;
                    }
                    const auto& edge_vertex = edges[next_edge++];
                    if( edge_vertex.vertex_id != UNDER_EDGE_VERTEX
                        || !above_relations_->value( edge_vertex.edge_id ) )
                    {
                        continue;
                    }
                    const auto vertex_above =
                        graph.edge_vertex( edge_vertex.opposite() );
                    if( status[vertex_above] == IN_PATH )
                    {
                        to_delete[edge_vertex.edge_id] = true;
                        has_cycle = true;
                        Logger::warning( "[StratigraphicRelationships::load] "
                                         "Removing relation ",
                            graph_component_id( vertex_above ).string(),
                            " above ", graph_component_id( vertex ).string(),
                            " which closes a cycle in above relations" );
                    }
                    else if( status[vertex_above] == NOT_VISITED )
                    {
                        status[vertex_above] = IN_PATH;
                        path.emplace_back( vertex_above, 0 );
                    }
                }
            }
            if( has_cycle )
            {
                GraphBuilder::create( *graph_ )->delete_edges( to_delete );
            }
        }

        /*!
         * Returns true if adding a relation "above is above under" would
         * create a cycle, i.e. if above is already (indirectly) under under.
         */
        bool would_create_cycle( const uuid& above, const uuid& under ) const
        {
            if( above == under )
            {
                return true;
            }
            const auto above_vertex = vertex_id( above );
            const auto under_vertex = vertex_id( under );
            if( !above_vertex || !under_vertex )
            {
                return false;
            }
            std::vector< bool > visited( this->graph().nb_vertices(), false );
            std::vector< index_t > to_visit{ above_vertex.value() };
            visited[above_vertex.value()] = true;
            while( !to_visit.empty() )
            {
                const auto vertex = to_visit.back();
                to_visit.pop_back();
                for( const auto& edge_vertex :
                    this->graph().edges_around_vertex( vertex ) )
                {
                    if( edge_vertex.vertex_id != UNDER_EDGE_VERTEX
                        || !above_relations_->value( edge_vertex.edge_id ) )
                    {
                        continue;
                    }
                    const auto vertex_above =
                        this->graph().edge_vertex( edge_vertex.opposite() );
                    if( vertex_above == under_vertex.value() )
                    {
                        return true;
                    }
                    if( !visited[vertex_above] )
                    {
                        visited[vertex_above] = true;
                        to_visit.push_back( vertex_above );
                    }
                }
            }
            return false;
        }

        std::optional< index_t > relation_edge(
            const uuid& from, const uuid& to ) const
        {
            const auto index_from = vertex_id( from );
            if( !index_from )
            {
                return std::nullopt;
            }
            const auto index_to = vertex_id( to );
            if( !index_to )
            {
                return std::nullopt;
            }
            for( const auto& edge_vertex :
                this->graph().edges_around_vertex( index_from.value() ) )
            {
                const auto& other =
                    this->graph_component_id( edge_vertex.opposite() );
                if( to == other.id )
                {
                    return edge_vertex.edge_id;
                }
            }
            return std::nullopt;
        }

        void remove_relation_edge( index_t relation_edge_id )
        {
            std::vector< bool > to_delete( graph_->nb_edges(), false );
            to_delete[relation_edge_id] = true;
            GraphBuilder::create( *graph_ )->delete_edges( to_delete );
        }

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive2, Impl& impl ) {
                         archive2.ext(
                             impl, bitsery::ext::BaseClass<
                                       detail::RelationshipsImpl >{} );
                         archive2.ext( impl.above_relations_,
                             bitsery::ext::StdSmartPtr{} );
                         std::shared_ptr< SparseAttribute< index_t > >
                             v0_unconformity_relations;
                         archive2.ext( v0_unconformity_relations,
                             bitsery::ext::StdSmartPtr{} );
                         std::vector< bool > to_delete(
                             impl.graph_->nb_edges(), false );
                         for( const auto edge_id :
                             Range{ impl.graph_->nb_edges() } )
                         {
                             if( !impl.above_relations_->value( edge_id ) )
                             {
                                 to_delete[edge_id] = true;
                             }
                         }
                         GraphBuilder::create( *impl.graph_ )
                             ->delete_edges( to_delete );
                     },
                        []( Archive& archive2, Impl& impl ) {
                            archive2.ext(
                                impl, bitsery::ext::BaseClass<
                                          detail::RelationshipsImpl >{} );
                            archive2.ext( impl.above_relations_,
                                bitsery::ext::StdSmartPtr{} );
                        } } } );
        }

    private:
        std::shared_ptr< SparseAttribute< bool > > above_relations_;
    };

    StratigraphicRelationships::StratigraphicRelationships() = default;
    StratigraphicRelationships::StratigraphicRelationships( BITSERY bitsery )
        : impl_( bitsery )
    {
    }
    StratigraphicRelationships::StratigraphicRelationships(
        StratigraphicRelationships&& ) noexcept = default;
    StratigraphicRelationships& StratigraphicRelationships::operator=(
        StratigraphicRelationships&& ) noexcept = default;
    StratigraphicRelationships::~StratigraphicRelationships() = default;

    void StratigraphicRelationships::remove_component(
        const uuid& id, StratigraphicRelationshipsBuilderKey )
    {
        impl_->remove_component( id );
    }

    bool StratigraphicRelationships::is_above(
        const uuid& above, const uuid& under ) const
    {
        return impl_->is_above( above, under );
    }

    bool StratigraphicRelationships::is_directly_above(
        const uuid& above, const uuid& under ) const
    {
        return impl_->is_directly_above( above, under );
    }

    std::optional< uuid > StratigraphicRelationships::above(
        const uuid& element ) const
    {
        return impl_->above( element );
    }

    std::optional< uuid > StratigraphicRelationships::under(
        const uuid& element ) const
    {
        return impl_->under( element );
    }

    index_t StratigraphicRelationships::add_above_relation(
        const ComponentID& above,
        const ComponentID& under,
        StratigraphicRelationshipsBuilderKey )
    {
        return impl_->add_above_relation( above, under );
    }

    void StratigraphicRelationships::remove_relation(
        const uuid& id1, const uuid& id2, StratigraphicRelationshipsBuilderKey )
    {
        impl_->remove_relation( id1, id2 );
    }

    void StratigraphicRelationships::remove_above_relation(
        const uuid& id1, const uuid& id2, StratigraphicRelationshipsBuilderKey )
    {
        impl_->remove_above_relation( id1, id2 );
    }

    void StratigraphicRelationships::save_stratigraphic_relationships(
        std::string_view directory ) const
    {
        impl_->save( directory );
    }

    void StratigraphicRelationships::copy_stratigraphic_relationships(
        const ModelCopyMapping& mapping,
        const StratigraphicRelationships& relationships,
        StratigraphicRelationshipsBuilderKey )
    {
        impl_->copy( *relationships.impl_, mapping );
    }

    void StratigraphicRelationships::load_stratigraphic_relationships(
        std::string_view directory, StratigraphicRelationshipsBuilderKey )
    {
        return impl_->load( directory );
    }
} // namespace geode
