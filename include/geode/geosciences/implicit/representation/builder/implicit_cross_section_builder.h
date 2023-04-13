/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.h>
#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitCrossSection;
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStack );
    ALIAS_2D( StratigraphicUnitsStack );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a ImplicitCrossSection
     */
    class opengeode_geosciences_implicit_api ImplicitCrossSectionBuilder
        : public CrossSectionBuilder
    {
    public:
        ImplicitCrossSectionBuilder( ImplicitCrossSection& implicit_section );

        void copy( const ImplicitCrossSection& implicit_section );

        void instantiate_implicit_attribute_on_surfaces();

        void set_implicit_value(
            const Surface2D& surface, index_t vertex_id, double value );

        void set_stratigraphic_units_stack( StratigraphicUnitsStack2D&& stack );

        void set_horizon_isovalue( const Horizon2D& horizon, double isovalue );

    private:
        ImplicitCrossSection& implicit_section_;
    };
} // namespace geode
