/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.h>

namespace geode
{
    class StratigraphicSection;
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicPoint );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_2D( StratigraphicPoint );
    ALIAS_1D( Point );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StratigraphicSection
     */
    class opengeode_geosciences_implicit_api StratigraphicSectionBuilder
        : public ImplicitCrossSectionBuilder
    {
    public:
        StratigraphicSectionBuilder(
            StratigraphicSection& stratigraphic_section );

        void copy( const StratigraphicSection& stratigraphic_section );

        void instantiate_stratigraphic_attribute_on_surfaces();

        void set_stratigraphic_location(
            const Surface2D& surface, index_t vertex_id, Point1D value );

        void set_stratigraphic_coordinates( const Surface2D& surface,
            index_t vertex_id,
            const StratigraphicPoint2D& value );

    private:
        StratigraphicSection& stratigraphic_section_;
    };
} // namespace geode
