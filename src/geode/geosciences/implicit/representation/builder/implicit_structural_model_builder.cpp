/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.h>

#include <geode/geometry/point.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/builder/detail/copy.h>

#include <geode/geosciences/implicit/representation/core/implicit_structural_model.h>

namespace geode
{
    ImplicitStructuralModelBuilder::ImplicitStructuralModelBuilder(
        ImplicitStructuralModel& implicit_model )
        : StructuralModelBuilder( implicit_model ),
          implicit_model_( implicit_model )
    {
    }

    void ImplicitStructuralModelBuilder::copy(
        const ImplicitStructuralModel& implicit_model )
    {
        StructuralModelBuilder::copy( implicit_model );
        instantiate_implicit_attribute_on_blocks();
    }

    void ImplicitStructuralModelBuilder::
        instantiate_implicit_attribute_on_blocks()
    {
        implicit_model_.instantiate_implicit_attribute_on_blocks( {} );
    }

    void ImplicitStructuralModelBuilder::set_implicit_value(
        const Block3D& block, index_t vertex_id, double value )
    {
        implicit_model_.set_implicit_value( block, vertex_id, value, {} );
    }

    void ImplicitStructuralModelBuilder::set_stratigraphic_units_stack(
        StratigraphicUnitsStack3D&& stack )
    {
        implicit_model_.set_stratigraphic_units_stack( std::move( stack ), {} );
    }

    void ImplicitStructuralModelBuilder::set_horizon_isovalue(
        const Horizon3D& horizon, double isovalue )
    {
        implicit_model_.set_horizon_isovalue( horizon, isovalue, {} );
    }
} // namespace geode
