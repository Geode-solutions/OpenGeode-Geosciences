/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
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
        static constexpr local_index_t ABOVE_EDGE_VERTEX{ 0 };
        static constexpr local_index_t UNDER_EDGE_VERTEX{ 1 };

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

        bool is_above( const uuid& above, const uuid& under ) const
        {
            const auto edge_id = relation_edge( above, under );
            if( !edge_id )
            {
                return false;
            }
            return vertex_component_id( { edge_id.value(), ABOVE_EDGE_VERTEX } )
                       .id()
                   == above;
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
                Logger::warn( "[StratigraphicRelationships::add_above_relation]"
                              " There is already a relation between ",
                    above.string(), " and ", under.string() );
                return id.value();
            }
            const auto index = add_relation_edge( above, under );
            return index;
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
            register_basic_serialize_pcontext( std::get< 0 >( context ) );
            register_geometry_serialize_pcontext( std::get< 0 >( context ) );
            register_mesh_serialize_pcontext( std::get< 0 >( context ) );
            register_model_serialize_pcontext( std::get< 0 >( context ) );
            /// Add register for structural model ?
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
            register_basic_deserialize_pcontext( std::get< 0 >( context ) );
            register_geometry_deserialize_pcontext( std::get< 0 >( context ) );
            register_mesh_deserialize_pcontext( std::get< 0 >( context ) );
            register_model_deserialize_pcontext( std::get< 0 >( context ) );
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
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{ { []( Archive& a,
                                                                 Impl& impl ) {
                a.ext( impl,
                    bitsery::ext::BaseClass< detail::RelationshipsImpl >{} );
            } } } );
        }
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

    void StratigraphicRelationships::remove_relation(
        const uuid& id1, const uuid& id2, StratigraphicRelationshipsBuilderKey )
    {
        impl_->remove_relation( id1, id2 );
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
