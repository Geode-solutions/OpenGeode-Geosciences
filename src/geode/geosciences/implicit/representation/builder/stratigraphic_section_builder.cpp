/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/stratigraphic_section_builder.h>

#include <geode/geometry/point.h>

#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

namespace geode
{
    StratigraphicSectionBuilder::StratigraphicSectionBuilder(
        StratigraphicSection& stratigraphic_section )
        : ImplicitCrossSectionBuilder( stratigraphic_section ),
          stratigraphic_section_( stratigraphic_section )
    {
    }

    void StratigraphicSectionBuilder::copy(
        const StratigraphicSection& stratigraphic_section )
    {
        ImplicitCrossSectionBuilder::copy( stratigraphic_section );
        stratigraphic_section_.instantiate_stratigraphic_location_on_surfaces(
            {} );
    }

    void StratigraphicSectionBuilder::
        instantiate_stratigraphic_attribute_on_surfaces()
    {
        this->instantiate_implicit_attribute_on_surfaces();
        stratigraphic_section_.instantiate_stratigraphic_location_on_surfaces(
            {} );
    }

    void StratigraphicSectionBuilder::set_stratigraphic_location(
        const Surface2D& surface, index_t vertex_id, Point1D value )
    {
        stratigraphic_section_.set_stratigraphic_location(
            surface, vertex_id, std::move( value ), {} );
    }

    void StratigraphicSectionBuilder::set_stratigraphic_coordinates(
        const Surface2D& surface,
        index_t vertex_id,
        const StratigraphicPoint2D& value )
    {
        this->set_implicit_value( surface, vertex_id, value.implicit_value() );
        stratigraphic_section_.set_stratigraphic_location(
            surface, vertex_id, value.stratigraphic_location(), {} );
    }
} // namespace geode
