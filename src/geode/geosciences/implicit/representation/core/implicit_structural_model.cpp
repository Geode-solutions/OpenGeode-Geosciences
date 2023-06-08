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

#include <geode/geosciences/implicit/representation/core/implicit_structural_model.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/cached_value.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/helpers/aabb_solid_helpers.h>
#include <geode/mesh/helpers/tetrahedral_solid_scalar_function.h>

#include <geode/model/mixin/core/block.h>

#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

namespace geode
{
    class ImplicitStructuralModel::Impl
    {
    public:
        void initialize_implicit_query_trees(
            const ImplicitStructuralModel& model )
        {
            implicit_attributes_.reserve( model.nb_blocks() );
            instantiate_implicit_attribute_on_blocks( model );
            block_mesh_aabb_trees_.reserve( model.nb_blocks() );
            for( const auto& block : model.blocks() )
            {
                block_mesh_aabb_trees_.try_emplace( block.id() );
            }
            block_distance_to_tetras_.reserve( model.nb_blocks() );
            build_model_distance_to_mesh_elements( model );
        }

        double implicit_value( const Block3D& block, index_t vertex_id ) const
        {
            return implicit_attributes_.at( block.id() ).value( vertex_id );
        }

        absl::optional< double > implicit_value(
            const Block3D& block, const Point3D& point ) const
        {
            if( const auto containing_tetra =
                    containing_polyhedron( block, point ) )
            {
                return implicit_value( block, point, containing_tetra.value() );
            }
            return absl::nullopt;
        }

        double implicit_value( const Block3D& block,
            const Point3D& point,
            index_t tetrahedron_id ) const
        {
            return implicit_attributes_.at( block.id() )
                .value( point, tetrahedron_id );
        }

        absl::optional< index_t > containing_polyhedron(
            const Block3D& block, const Point3D& point ) const
        {
            auto closest_tetrahedron = std::get< 0 >(
                block_mesh_aabb_trees_
                    .at( block.id() )( create_aabb_tree, block.mesh() )
                    .closest_element_box(
                        point, block_distance_to_tetras_.at( block.id() ) ) );
            if( std::get< 0 >( block_distance_to_tetras_.at( block.id() )(
                    point, closest_tetrahedron ) )
                < global_epsilon )
            {
                return closest_tetrahedron;
            }
            return absl::nullopt;
        }

        const HorizonsStack3D& horizons_stack() const
        {
            return horizons_stack_;
        }

        HorizonsStack3D& modifiable_horizons_stack()
        {
            return horizons_stack_;
        }

        absl::optional< double > horizon_implicit_value(
            const Horizon3D& horizon ) const
        {
            OPENGEODE_EXCEPTION( horizons_stack_.has_horizon( horizon.id() ),
                "[horizon_implicit_value] You cannot access the isovalue of "
                "Horizon ",
                horizon.id().string(),
                " because the horizon is not defined in the "
                "HorizonsStack." );
            const auto value = horizon_isovalues_.find( horizon.id() );
            if( value == horizon_isovalues_.end() )
            {
                return absl::nullopt;
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

        absl::optional< uuid > containing_stratigraphic_unit(
            double implicit_function_value ) const
        {
            if( horizon_isovalues_.empty() )
            {
                return absl::nullopt;
            }
            const auto increasing = increasing_stack_isovalues();
            if( !increasing.has_value() )
            {
                return absl::nullopt;
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
                        return absl::nullopt;
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
                    return absl::nullopt;
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
            for( const auto& block : model.blocks() )
            {
                OPENGEODE_EXCEPTION(
                    ( block.mesh().type_name()
                        == TetrahedralSolid3D::type_name_static() ),
                    "[ImplicitStructuralModel::instantiate_implicit_attribute_"
                    "on_blocks] Blocks must be meshed as TetrahedralSolids, "
                    "which is not the case for block with uuid '",
                    block.id().string(), "'." );
                if( !block.mesh().vertex_attribute_manager().attribute_exists(
                        implicit_attribute_name ) )
                {
                    implicit_attributes_.try_emplace(
                        block.id(), TetrahedralSolidScalarFunction3D::create(
                                        block.mesh< TetrahedralSolid3D >(),
                                        implicit_attribute_name, 0 ) );
                }
                else
                {
                    implicit_attributes_.try_emplace(
                        block.id(), TetrahedralSolidScalarFunction3D::find(
                                        block.mesh< TetrahedralSolid3D >(),
                                        implicit_attribute_name ) );
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
                " because the horizon is not defined in the "
                "HorizonsStack." );
            horizon_isovalues_[horizon.id()] = isovalue;
        }

    private:
        void build_model_distance_to_mesh_elements(
            const ImplicitStructuralModel& model )
        {
            for( const auto& block : model.blocks() )
            {
                block_distance_to_tetras_.try_emplace(
                    block.id(), DistanceToTetrahedron3D{
                                    block.mesh< TetrahedralSolid3D >() } );
            }
        }

        absl::optional< bool > increasing_stack_isovalues() const
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
                        return absl::nullopt;
                    }
                    return it0->second > it1->second;
                }
            }
            return absl::nullopt;
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
        ImplicitStructuralModel&& implicit_model )
        : StructuralModel{ std::move( implicit_model ) }
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    ImplicitStructuralModel::ImplicitStructuralModel(
        StructuralModel&& structural_model )
        : StructuralModel{ std::move( structural_model ) }
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    ImplicitStructuralModel::~ImplicitStructuralModel() {}

    double ImplicitStructuralModel::implicit_value(
        const Block3D& block, index_t vertex_id ) const
    {
        return impl_->implicit_value( block, vertex_id );
    }

    absl::optional< double > ImplicitStructuralModel::implicit_value(
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
    absl::optional< index_t > ImplicitStructuralModel::containing_polyhedron(
        const Block3D& block, const Point3D& point ) const
    {
        return impl_->containing_polyhedron( block, point );
    }

    const HorizonsStack3D& ImplicitStructuralModel::horizons_stack() const
    {
        return impl_->horizons_stack();
    }

    absl::optional< double > ImplicitStructuralModel::horizon_implicit_value(
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

    absl::optional< uuid >
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
} // namespace geode