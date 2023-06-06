/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.h>

#include <fstream>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/bitsery_archive.h>

#include <geode/mesh/builder/geode/geode_graph_builder.h>
#include <geode/mesh/core/bitsery_archive.h>
#include <geode/mesh/io/graph_input.h>
#include <geode/mesh/io/graph_output.h>

#include <geode/model/mixin/core/bitsery_archive.h>
#include <geode/model/mixin/core/detail/relationships_impl.h>

namespace geode
{
    class StratigraphicRelationships::Impl : public detail::RelationshipsImpl
    {
    public:
        using UnconformityType = index_t;
        static constexpr index_t NO_UNCONFORMITY{ NO_ID };
        static constexpr index_t EROSION_RELATION{ 0 };
        static constexpr index_t BASELAP_RELATION{ 1 };
        static constexpr local_index_t ABOVE_EDGE_VERTEX{ 0 };
        static constexpr local_index_t UNDER_EDGE_VERTEX{ 1 };
        static constexpr local_index_t ERODES_EDGE_VERTEX{ 0 };
        static constexpr local_index_t IS_ERODED_EDGE_VERTEX{ 1 };
        static constexpr local_index_t BASELAPS_ON_EDGE_VERTEX{ 0 };
        static constexpr local_index_t IS_BASELAP_OF_EDGE_VERTEX{ 1 };

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
            return vertex_component_id( { edge_id.value(), ABOVE_EDGE_VERTEX } )
                       .id()
                   == above;
        }

        bool is_eroded_by( const uuid& eroded, const uuid& erosion ) const
        {
            const auto edge_id = relation_edge( eroded, erosion );
            if( !edge_id )
            {
                return false;
            }
            if( unconformity_relations_->value( edge_id.value() )
                != EROSION_RELATION )
            {
                return false;
            }
            return vertex_component_id(
                       { edge_id.value(), IS_ERODED_EDGE_VERTEX } )
                       .id()
                   == eroded;
        }

        bool is_baselap_of( const uuid& baselap, const uuid& baselap_top ) const
        {
            const auto edge_id = relation_edge( baselap, baselap_top );
            if( !edge_id )
            {
                return false;
            }
            if( unconformity_relations_->value( edge_id.value() )
                != BASELAP_RELATION )
            {
                return false;
            }
            return vertex_component_id(
                       { edge_id.value(), IS_BASELAP_OF_EDGE_VERTEX } )
                       .id()
                   == baselap;
        }

        absl::optional< uuid > above( const uuid& element ) const
        {
            const auto index_from = vertex_id( element );
            if( !index_from )
            {
                return absl::nullopt;
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
                    return this->vertex_component_id( edge_vertex.opposite() )
                        .id();
                }
            }
            return absl::nullopt;
        }

        absl::optional< uuid > under( const uuid& element ) const
        {
            const auto index_from = vertex_id( element );
            if( !index_from )
            {
                return absl::nullopt;
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
                    return this->vertex_component_id( edge_vertex.opposite() )
                        .id();
                }
            }
            return absl::nullopt;
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

        index_t add_erosion_relation(
            const ComponentID& erosion, const ComponentID& eroded )
        {
            if( const auto id = relation_edge( erosion.id(), eroded.id() ) )
            {
                unconformity_relations_->set_value(
                    id.value(), EROSION_RELATION );
                return id.value();
            }
            const auto index = add_relation_edge( erosion, eroded );
            unconformity_relations_->set_value( index, EROSION_RELATION );
            return index;
        }

        index_t add_baselap_relation(
            const ComponentID& baselap_top, const ComponentID& baselap )
        {
            if( const auto id =
                    relation_edge( baselap_top.id(), baselap.id() ) )
            {
                unconformity_relations_->set_value(
                    id.value(), BASELAP_RELATION );
                return id.value();
            }
            const auto index = add_relation_edge( baselap_top, baselap );
            unconformity_relations_->set_value( index, BASELAP_RELATION );
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
            if( unconformity_relations_->value( id.value() )
                == NO_UNCONFORMITY )
            {
                this->remove_relation_edge( id.value() );
            }
            above_relations_->set_value( id.value(), false );
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
            unconformity_relations_->set_value( id.value(), NO_UNCONFORMITY );
        }

        void copy( const Impl& impl, const ModelCopyMapping& mapping )
        {
            detail::RelationshipsImpl::copy( impl, mapping );
        }

        void save( absl::string_view directory ) const
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

        void load( absl::string_view directory )
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
                        UnconformityType >(
                        "geode_unconformities", NO_UNCONFORMITY );
        }

        absl::optional< index_t > relation_edge(
            const uuid& from, const uuid& to ) const
        {
            const auto index_from = vertex_id( from );
            if( !index_from )
            {
                return absl::nullopt;
            }
            const auto index_to = vertex_id( to );
            if( !index_to )
            {
                return absl::nullopt;
            }
            for( const auto& edge_vertex :
                this->graph().edges_around_vertex( index_from.value() ) )
            {
                const auto& other =
                    this->vertex_component_id( edge_vertex.opposite() );
                if( to == other.id() )
                {
                    return edge_vertex.edge_id;
                }
            }
            return absl::nullopt;
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
            archive.ext( *this, Growable< Archive, Impl >{ { []( Archive& a,
                                                                 Impl& impl ) {
                a.ext( impl,
                    bitsery::ext::BaseClass< detail::RelationshipsImpl >{} );
                a.ext( impl.above_relations_, bitsery::ext::StdSmartPtr{} );
                a.ext(
                    impl.unconformity_relations_, bitsery::ext::StdSmartPtr{} );
            } } } );
        }

    private:
        std::shared_ptr< SparseAttribute< bool > > above_relations_;
        std::shared_ptr< SparseAttribute< UnconformityType > >
            unconformity_relations_;
    };

    StratigraphicRelationships::StratigraphicRelationships() {} // NOLINT
    StratigraphicRelationships::StratigraphicRelationships(
        StratigraphicRelationships&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }
    StratigraphicRelationships::~StratigraphicRelationships() {} // NOLINT

    StratigraphicRelationships& StratigraphicRelationships::operator=(
        StratigraphicRelationships&& other )
    {
        impl_ = std::move( other.impl_ );
        return *this;
    }

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

    bool StratigraphicRelationships::is_eroded_by(
        const uuid& eroded, const uuid& erosion ) const
    {
        return impl_->is_eroded_by( eroded, erosion );
    }

    bool StratigraphicRelationships::is_baselap_of(
        const uuid& baselap, const uuid& baselap_top ) const
    {
        return impl_->is_baselap_of( baselap, baselap_top );
    }

    absl::optional< uuid > StratigraphicRelationships::above(
        const uuid& element ) const
    {
        return impl_->above( element );
    }

    absl::optional< uuid > StratigraphicRelationships::under(
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

    index_t StratigraphicRelationships::add_erosion_relation(
        const ComponentID& erosion,
        const ComponentID& eroded,
        StratigraphicRelationshipsBuilderKey )
    {
        return impl_->add_erosion_relation( erosion, eroded );
    }

    index_t StratigraphicRelationships::add_baselap_relation(
        const ComponentID& baselap_top,
        const ComponentID& baselap,
        StratigraphicRelationshipsBuilderKey )
    {
        return impl_->add_baselap_relation( baselap_top, baselap );
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
        absl::string_view directory ) const
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
        absl::string_view directory, StratigraphicRelationshipsBuilderKey )
    {
        return impl_->load( directory );
    }
} // namespace geode
