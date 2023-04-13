/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.h>
#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitStructuralModel;
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStack );
    ALIAS_3D( StratigraphicUnitsStack );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a ImplicitStructuralModel
     */
    class opengeode_geosciences_implicit_api ImplicitStructuralModelBuilder
        : public StructuralModelBuilder
    {
    public:
        ImplicitStructuralModelBuilder(
            ImplicitStructuralModel& implicit_model );

        void copy( const ImplicitStructuralModel& implicit_model );

        void instantiate_implicit_attribute_on_blocks();

        void set_implicit_value(
            const Block3D& block, index_t vertex_id, double value );

        void set_stratigraphic_units_stack( StratigraphicUnitsStack3D&& stack );

        void set_horizon_isovalue( const Horizon3D& horizon, double isovalue );

    private:
        ImplicitStructuralModel& implicit_model_;
    };
} // namespace geode
