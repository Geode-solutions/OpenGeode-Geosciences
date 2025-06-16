/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
#include <geode/basic/pimpl_impl.hpp>
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
        using UnconformityType = bool;
        static constexpr bool CONFORMAL{ true };
        static constexpr bool UNCONFORMAL{ false };
        // static constexpr index_t NO_UNCONFORMITY{ NO_ID };
        // static constexpr index_t EROSION_RELATION{ 0 };
        // static constexpr index_t BASELAP_RELATION{ 1 };
        static constexpr local_index_t ABOVE_EDGE_VERTEX{ 0 };
        static constexpr local_index_t UNDER_EDGE_VERTEX{ 1 };
        // static constexpr local_index_t ERODES_EDGE_VERTEX{ 0 };
        // static constexpr local_index_t IS_ERODED_EDGE_VERTEX{ 1 };
        // static constexpr local_index_t BASELAPS_ON_EDGE_VERTEX{ 0 };
        // static constexpr local_index_t IS_BASELAP_OF_EDGE_VERTEX{ 1 };

        Impl() : RelationshipsImpl()
        {
            initialize_relation_attributes();
        }

        bool is_above( const uuid& above, const uuid& under ) const
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
                       .id()
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
                        .id();
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
                        .id();
                }
            }
            return std::nullopt;
        }

        bool is_conformal_above( const uuid& element ) const
        {
            const auto index_from = vertex_id( element );
            OPENGEODE_EXCEPTION( index_from,
                "[StratigraphicRelationships::Impl::is_conformal_"
                "above] Did not find element id in relations graph. Element "
                "with id ",
                element.string(), "probably doesn't exist." );
            for( const auto& edge_vertex :
                this->graph().edges_around_vertex( index_from.value() ) )
            {
                if( !above_relations_->value( edge_vertex.edge_id ) )
                {
                    continue;
                }
                if( edge_vertex.vertex_id == UNDER_EDGE_VERTEX )
                {
                    return unconformity_relations_->value( edge_vertex.edge_id )
                           == CONFORMAL;
                }
            }
            geode::Logger::debug( "Didn't find any relation leading to above" );
            return true;
        }

        bool is_conformal_under( const uuid& element ) const
        {
            const auto index_from = vertex_id( element );
            OPENGEODE_EXCEPTION( index_from,
                "[StratigraphicRelationships::Impl::is_conformal_"
                "under] Did not find element id in relations graph. Element "
                "with id ",
                element.string(), "probably doesn't exist." );
            for( const auto& edge_vertex :
                this->graph().edges_around_vertex( index_from.value() ) )
            {
                if( !above_relations_->value( edge_vertex.edge_id ) )
                {
                    continue;
                }
                if( edge_vertex.vertex_id == ABOVE_EDGE_VERTEX )
                {
                    return unconformity_relations_->value( edge_vertex.edge_id )
                           == CONFORMAL;
                }
            }
            geode::Logger::debug( "Didn't find any relation leading to under" );
            return true;
        }

        index_t add_above_relation(
            const ComponentID& above, const ComponentID& under )
        {
            if( const auto id = relation_edge( above.id(), under.id() ) )
            {
                above_relations_->set_value( id.value(), true );
                return id.value();
            }
            const auto index = add_relation_edge( above, under );
            above_relations_->set_value( index, true );
            return index;
        }

        index_t add_unconformity_relation(
            const ComponentID& component_1, const ComponentID& component_2 )
        {
            if( const auto id =
                    relation_edge( component_1.id(), component_2.id() ) )
            {
                unconformity_relations_->set_value( id.value(), UNCONFORMAL );
                return id.value();
            }
            geode::Logger::trace( "Found no edge between component ",
                component_1.string(), " and component ", component_2.string(),
                ", creating a new one" );
            const auto index = add_relation_edge( component_1, component_2 );
            unconformity_relations_->set_value( index, UNCONFORMAL );
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

        void remove_unconformity_relation( const uuid& id1, const uuid& id2 )
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
            if( above_relations_->value( id.value() ) == false )
            {
                this->remove_relation_edge( id.value() );
            }
            unconformity_relations_->set_value( id.value(), CONFORMAL );
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
            OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
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
            OPENGEODE_EXCEPTION(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Relationships::load] Error while reading file: ", filename );
        }

    private:
        void initialize_relation_attributes()
        {
            above_relations_ =
                relation_attribute_manager()
                    .find_or_create_attribute< SparseAttribute, bool >(
                        "geode_above_relations", false );
            unconformity_relations_ =
                relation_attribute_manager()
                    .find_or_create_attribute< SparseAttribute,
                        UnconformityType >( "geode_conformity", CONFORMAL );
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
                if( to == other.id() )
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
                         impl.initialize_relation_attributes();
                         auto previous_unconformities =
                             impl.relation_attribute_manager()
                                 .find_attribute< SparseAttribute, index_t >(
                                     "geode_unconformities" );
                         std::vector< std::pair< uuid > > edges_to_remove;
                         for( const auto edge_id :
                             Range{ impl.graph().nb_edges() } )
                         {
                             if( previous_unconformities->value( edge_id )
                                 == 0 )
                             {
                                 if( !above_relations_->value( edge_id ) )
                                 {
                                     edges_to_remove.emplace_back(
                                         impl.graph_component_id(
                                                 { edge_id, 0 } )
                                             .id(),
                                         impl.graph_component_id(
                                                 { edge_id, 1 } )
                                             .id() );
                                     continue;
                                 }
                                 impl.unconformity_relations_->set_value(
                                     edge_id, UNCONFORMAL );
                             }
                             else if( previous_unconformities->value( edge_id )
                                      == 1 )
                             {
                                 const auto opposite_edge =
                                     impl.graph().edge_from_vertices(
                                         impl.graph().edge_vertex( edge_id, 1 ),
                                         impl.graph().edge_vertex(
                                             edge_id, 0 ) );
                                 edges_to_remove.emplace_back(
                                     impl.graph_component_id( { edge_id, 0 } )
                                         .id(),
                                     impl.graph_component_id( { edge_id, 1 } )
                                         .id() );
                                 if( opposite_edge
                                     && above_relations_->value(
                                         opposite_edge.value() ) )
                                 {
                                     impl.unconformity_relations_->set_value(
                                         opposite_edge.value(), UNCONFORMAL );
                                     continue;
                                 }
                             }
                         }
                         for( const auto& [id_1, id_2] : edges_to_remove )
                         {
                             impl.remove_relation( id_1, id_2 );
                         }
                     },
                        []( Archive& archive2, Impl& impl ) {
                            archive2.ext(
                                impl, bitsery::ext::BaseClass<
                                          detail::RelationshipsImpl >{} );
                            archive2.ext( impl.above_relations_,
                                bitsery::ext::StdSmartPtr{} );
                            archive2.ext( impl.unconformity_relations_,
                                bitsery::ext::StdSmartPtr{} );
                        } } } );
        }

    private:
        std::shared_ptr< SparseAttribute< bool > > above_relations_;
        std::shared_ptr< SparseAttribute< UnconformityType > >
            unconformity_relations_;
    };

    StratigraphicRelationships::StratigraphicRelationships() = default;
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

    bool StratigraphicRelationships::is_conformal_above(
        const uuid& element ) const
    {
        return impl_->is_conformal_above( element );
    }

    bool StratigraphicRelationships::is_conformal_under(
        const uuid& element ) const
    {
        return impl_->is_conformal_under( element );
    }

    index_t StratigraphicRelationships::add_above_relation(
        const ComponentID& above,
        const ComponentID& under,
        StratigraphicRelationshipsBuilderKey )
    {
        return impl_->add_above_relation( above, under );
    }

    index_t StratigraphicRelationships::add_unconformity_relation(
        const ComponentID& component_1,
        const ComponentID& component_2,
        StratigraphicRelationshipsBuilderKey )
    {
        return impl_->add_unconformity_relation( component_1, component_2 );
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

    void StratigraphicRelationships::remove_unconformity_relation(
        const uuid& id1, const uuid& id2, StratigraphicRelationshipsBuilderKey )
    {
        impl_->remove_unconformity_relation( id1, id2 );
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
