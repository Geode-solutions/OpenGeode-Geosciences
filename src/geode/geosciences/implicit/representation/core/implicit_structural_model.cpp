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

#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/cached_value.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/helpers/aabb_solid_helpers.hpp>
#include <geode/mesh/helpers/tetrahedral_solid_scalar_function.hpp>

#include <geode/model/mixin/core/block.hpp>

#include <geode/geosciences/explicit/representation/core/detail/clone.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

namespace geode
{
    class ImplicitStructuralModel::Impl
    {
    public:
        Impl() = default;

        void initialize_implicit_query_trees(
            const ImplicitStructuralModel& model )
        {
            instantiate_implicit_attribute_on_blocks( model );
            block_mesh_aabb_trees_.reserve( model.nb_blocks() );
            block_distance_to_tetras_.reserve( model.nb_blocks() );
            for( const auto& block : model.blocks() )
            {
                if( !block_is_meshed( block ) )
                {
                    continue;
                }
                block_mesh_aabb_trees_.try_emplace( block.id() );
                block_distance_to_tetras_.try_emplace(
                    block.id(), DistanceToTetrahedron3D{
                                    block.mesh< TetrahedralSolid3D >() } );
            }
        }

        double implicit_value( const Block3D& block, index_t vertex_id ) const
        {
            return implicit_attributes_.at( block.id() ).value( vertex_id );
        }

        std::optional< double > implicit_value(
            const Block3D& block, const Point3D& point ) const
        {
            if( const auto containing_tetra =
                    containing_polyhedron( block, point ) )
            {
                return implicit_value( block, point, containing_tetra.value() );
            }
            return std::nullopt;
        }

        double implicit_value( const Block3D& block,
            const Point3D& point,
            index_t tetrahedron_id ) const
        {
            return implicit_attributes_.at( block.id() )
                .value( point, tetrahedron_id );
        }

        std::optional< index_t > containing_polyhedron(
            const Block3D& block, const Point3D& point ) const
        {
            auto closest_tetrahedron = std::get< 0 >(
                block_mesh_aabb_trees_
                    .at( block.id() )( create_aabb_tree, block.mesh() )
                    .closest_element_box(
                        point, block_distance_to_tetras_.at( block.id() ) ) );
            if( block_distance_to_tetras_.at( block.id() )(
                    point, closest_tetrahedron )
                < GLOBAL_EPSILON )
            {
                return closest_tetrahedron;
            }
            return std::nullopt;
        }

        const HorizonsStack3D& horizons_stack() const
        {
            return horizons_stack_;
        }

        HorizonsStack3D& modifiable_horizons_stack()
        {
            return horizons_stack_;
        }

        std::optional< double > horizon_implicit_value(
            const Horizon3D& horizon ) const
        {
            OPENGEODE_EXCEPTION( horizons_stack_.has_horizon( horizon.id() ),
                "[horizon_implicit_value] You cannot access the isovalue of "
                "Horizon ",
                horizon.id().string(),
                " because the horizon is not defined in the HorizonsStack." );
            const auto value = horizon_isovalues_.find( horizon.id() );
            if( value == horizon_isovalues_.end() )
            {
                return std::nullopt;
            }
            return value->second;
        }

        bool implicit_value_is_above_horizon(
            double implicit_function_value, const Horizon3D& horizon ) const
        {
            const auto increasing = increasing_stack_isovalues();
            OPENGEODE_EXCEPTION( increasing.has_value(),
                "[implicit_value_is_above_horizon] Could not define if "
                "implicit values were "
                "increasing or decreasing in the horizon stack." );
            const auto it = horizon_isovalues_.find( horizon.id() );
            OPENGEODE_EXCEPTION( it != horizon_isovalues_.end(),
                "[implicit_value_is_above_horizon] Cannot find horizon "
                "implicit value in the horizon stack." );
            return increasing.value()
                   == ( implicit_function_value >= it->second );
        }

        std::optional< uuid > containing_stratigraphic_unit(
            double implicit_function_value ) const
        {
            if( horizon_isovalues_.empty() )
            {
                return std::nullopt;
            }
            const auto increasing = increasing_stack_isovalues();
            if( !increasing.has_value() )
            {
                return std::nullopt;
            }
            auto horizon_id = horizon_isovalues_.begin()->first;
            while( true )
            {
                if( increasing.value()
                    == ( implicit_function_value
                         >= horizon_isovalues_.at( horizon_id ) ) )
                {
                    const auto unit_above = horizons_stack_.above( horizon_id );
                    if( !unit_above )
                    {
                        return std::nullopt;
                    }
                    const auto horizon_above =
                        horizons_stack_.above( unit_above.value() );
                    if( !horizon_above
                        || increasing.value()
                               == ( implicit_function_value
                                    < horizon_isovalues_.at( horizon_id ) ) )
                    {
                        return unit_above.value();
                    }
                    horizon_id = horizon_above.value();
                    continue;
                }
                const auto unit_under = horizons_stack_.under( horizon_id );
                if( !unit_under )
                {
                    return std::nullopt;
                }
                const auto horizon_under =
                    horizons_stack_.under( unit_under.value() );
                if( !horizon_under
                    || increasing.value()
                           == ( implicit_function_value
                                > horizon_isovalues_.at( horizon_id ) ) )
                {
                    return unit_under.value();
                }
                horizon_id = horizon_under.value();
            }
        }

        void instantiate_implicit_attribute_on_blocks(
            const ImplicitStructuralModel& model )
        {
            implicit_attributes_.reserve( model.nb_blocks() );
            for( const auto& block : model.blocks() )
            {
                if( !block_is_meshed( block ) )
                {
                    continue;
                }
                OPENGEODE_EXCEPTION(
                    ( block.mesh().type_name()
                        == TetrahedralSolid3D::type_name_static() ),
                    "[ImplicitStructuralModel::instantiate_implicit_attribute_"
                    "on_blocks] Blocks must be meshed as TetrahedralSolids, "
                    "which is not the case for block with uuid '",
                    block.id().string(), "'." );
                if( !block.mesh().vertex_attribute_manager().attribute_exists(
                        IMPLICIT_ATTRIBUTE_NAME ) )
                {
                    implicit_attributes_.try_emplace(
                        block.id(), TetrahedralSolidScalarFunction3D::create(
                                        block.mesh< TetrahedralSolid3D >(),
                                        IMPLICIT_ATTRIBUTE_NAME, 0 ) );
                }
                else
                {
                    implicit_attributes_.try_emplace(
                        block.id(), TetrahedralSolidScalarFunction3D::find(
                                        block.mesh< TetrahedralSolid3D >(),
                                        IMPLICIT_ATTRIBUTE_NAME ) );
                }
            }
        }

        void set_implicit_value(
            const Block3D& block, index_t vertex_id, double value )
        {
            OPENGEODE_EXCEPTION( implicit_attributes_.find( block.id() )
                                     != implicit_attributes_.end(),
                "[ImplicitStructuralModel::set_implicit_value] Couldn't find "
                "block uuid in the attributes registered - Try instantiating "
                "your attribute first." );
            implicit_attributes_.at( block.id() ).set_value( vertex_id, value );
        }

        void set_horizons_stack( HorizonsStack3D&& stack )
        {
            horizons_stack_ = std::move( stack );
        }

        void set_horizon_implicit_value(
            const Horizon3D& horizon, double isovalue )
        {
            OPENGEODE_EXCEPTION( horizons_stack_.has_horizon( horizon.id() ),
                "[horizon_implicit_value] You cannot access the isovalue of "
                "Horizon ",
                horizon.id().string(),
                " because the horizon is not defined in the HorizonsStack." );
            horizon_isovalues_[horizon.id()] = isovalue;
        }

    private:
        bool block_is_meshed( const Block3D& block )
        {
            return block.mesh().nb_polyhedra() != 0;
        }

        std::optional< bool > increasing_stack_isovalues() const
        {
            for( const auto& unit : horizons_stack_.stratigraphic_units() )
            {
                const auto above = horizons_stack_.above( unit.id() );
                const auto under = horizons_stack_.under( unit.id() );
                if( above && under )
                {
                    const auto it0 = horizon_isovalues_.find( above.value() );
                    const auto it1 = horizon_isovalues_.find( under.value() );
                    if( it0 == horizon_isovalues_.end()
                        || it1 == horizon_isovalues_.end() )
                    {
                        return std::nullopt;
                    }
                    return it0->second > it1->second;
                }
            }
            return std::nullopt;
        }

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{ { []( Archive& a,
                                                                 Impl& impl ) {
                a.ext( impl.horizon_isovalues_,
                    bitsery::ext::StdMap{ impl.horizon_isovalues_.max_size() },
                    []( Archive& a2, uuid& id, double item ) {
                        a2.object( id );
                        a2.value8b( item );
                    } );
            } } } );
        }

    private:
        absl::flat_hash_map< uuid, TetrahedralSolidScalarFunction3D >
            implicit_attributes_;
        HorizonsStack3D horizons_stack_;
        absl::flat_hash_map< uuid, double > horizon_isovalues_;
        absl::flat_hash_map< uuid, CachedValue< AABBTree3D > >
            block_mesh_aabb_trees_;
        absl::flat_hash_map< uuid, DistanceToTetrahedron3D >
            block_distance_to_tetras_;
    };

    ImplicitStructuralModel::ImplicitStructuralModel()
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    ImplicitStructuralModel::ImplicitStructuralModel(
        ImplicitStructuralModel&& other ) noexcept
        : StructuralModel{ std::move( other ) },
          impl_{ std::move( other.impl_ ) }
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    ImplicitStructuralModel::ImplicitStructuralModel(
        StructuralModel&& structural_model ) noexcept
        : StructuralModel{ std::move( structural_model ) }
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    ImplicitStructuralModel::ImplicitStructuralModel(
        const ImplicitStructuralModel& initial_model,
        BRep&& brep,
        const ModelGenericMapping& initial_to_brep_mappings ) noexcept
        : StructuralModel{ initial_model, std::move( brep ),
              initial_to_brep_mappings }
    {
        impl_->initialize_implicit_query_trees( *this );
        ModelCopyMapping mappings;
        detail::add_geology_clone_mapping< StructuralModel >(
            mappings, initial_model );
        ImplicitStructuralModelBuilder builder{ *this };
        builder.copy_implicit_information( mappings, initial_model );
    }

    ImplicitStructuralModel::~ImplicitStructuralModel() = default;

    ImplicitStructuralModel ImplicitStructuralModel::clone() const
    {
        ImplicitStructuralModel model_clone{ StructuralModel::clone() };
        ImplicitStructuralModelBuilder clone_builder{ model_clone };
        ModelCopyMapping clone_mappings;
        detail::add_geology_clone_mapping< StructuralModel >(
            clone_mappings, *this );
        clone_builder.copy_implicit_information( clone_mappings, *this );
        return model_clone;
    }

    double ImplicitStructuralModel::implicit_value(
        const Block3D& block, index_t vertex_id ) const
    {
        return impl_->implicit_value( block, vertex_id );
    }

    std::optional< double > ImplicitStructuralModel::implicit_value(
        const Block3D& block, const Point3D& point ) const
    {
        return impl_->implicit_value( block, point );
    }

    double ImplicitStructuralModel::implicit_value( const Block3D& block,
        const Point3D& point,
        index_t polyhedron_id ) const
    {
        return impl_->implicit_value( block, point, polyhedron_id );
    }
    std::optional< index_t > ImplicitStructuralModel::containing_polyhedron(
        const Block3D& block, const Point3D& point ) const
    {
        return impl_->containing_polyhedron( block, point );
    }

    const HorizonsStack3D& ImplicitStructuralModel::horizons_stack() const
    {
        return impl_->horizons_stack();
    }

    std::optional< double > ImplicitStructuralModel::horizon_implicit_value(
        const Horizon3D& horizon ) const
    {
        return impl_->horizon_implicit_value( horizon );
    }

    bool ImplicitStructuralModel::implicit_value_is_above_horizon(
        double implicit_function_value, const Horizon3D& horizon ) const
    {
        return impl_->implicit_value_is_above_horizon(
            implicit_function_value, horizon );
    }

    std::optional< uuid >
        ImplicitStructuralModel::containing_stratigraphic_unit(
            double implicit_function_value ) const
    {
        return impl_->containing_stratigraphic_unit( implicit_function_value );
    }

    void ImplicitStructuralModel::initialize_implicit_query_trees(
        ImplicitStructuralModelBuilderKey )
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    void ImplicitStructuralModel::instantiate_implicit_attribute_on_blocks(
        ImplicitStructuralModelBuilderKey )
    {
        impl_->instantiate_implicit_attribute_on_blocks( *this );
    }

    void ImplicitStructuralModel::set_implicit_value( const Block3D& block,
        index_t vertex_id,
        double value,
        ImplicitStructuralModelBuilderKey )
    {
        do_set_implicit_value( block, vertex_id, value );
    }

    void ImplicitStructuralModel::set_horizons_stack(
        HorizonsStack3D&& stack, ImplicitStructuralModelBuilderKey )
    {
        impl_->set_horizons_stack( std::move( stack ) );
    }

    void ImplicitStructuralModel::set_horizon_implicit_value(
        const Horizon3D& horizon,
        double isovalue,
        ImplicitStructuralModelBuilderKey )
    {
        impl_->set_horizon_implicit_value( horizon, isovalue );
    }

    HorizonsStack3D& ImplicitStructuralModel::modifiable_horizons_stack(
        ImplicitStructuralModelBuilderKey )
    {
        return impl_->modifiable_horizons_stack();
    }

    void ImplicitStructuralModel::do_set_implicit_value(
        const Block3D& block, index_t vertex_id, double value )
    {
        impl_->set_implicit_value( block, vertex_id, value );
    }

    template < typename Archive >
    void ImplicitStructuralModel::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, ImplicitStructuralModel >{
                       { []( Archive& a, ImplicitStructuralModel& model ) {
                           a.object( model.impl_ );
                       } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_implicit_api, ImplicitStructuralModel );
} // namespace geode