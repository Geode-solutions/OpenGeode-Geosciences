/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.h>

#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>

namespace geode
{
    ImplicitCrossSectionBuilder::ImplicitCrossSectionBuilder(
        ImplicitCrossSection& implicit_section )
        : CrossSectionBuilder( implicit_section ),
          implicit_section_( implicit_section )
    {
    }

    void ImplicitCrossSectionBuilder::copy(
        const ImplicitCrossSection& implicit_section )
    {
        CrossSectionBuilder::copy( implicit_section );
        instantiate_implicit_attribute_on_surfaces();
    }

    void ImplicitCrossSectionBuilder::
        instantiate_implicit_attribute_on_surfaces()
    {
        implicit_section_.instantiate_implicit_attribute_on_surfaces( {} );
    }

    void ImplicitCrossSectionBuilder::set_implicit_value(
        const Surface2D& surface, index_t vertex_id, double value )
    {
        implicit_section_.set_implicit_value( surface, vertex_id, value, {} );
    }

    void ImplicitCrossSectionBuilder::set_stratigraphic_units_stack(
        StratigraphicUnitsStack2D&& stack )
    {
        implicit_section_.set_stratigraphic_units_stack(
            std::move( stack ), {} );
    }

    void ImplicitCrossSectionBuilder::set_horizon_isovalue(
        const Horizon2D& horizon, double isovalue )
    {
        implicit_section_.set_horizon_isovalue( horizon, isovalue, {} );
    }
} // namespace geode
