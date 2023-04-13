/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.h>

namespace geode
{
    class StratigraphicModel;
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicPoint );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_3D( StratigraphicPoint );
    ALIAS_2D( Point );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StratigraphicModel
     */
    class opengeode_geosciences_implicit_api StratigraphicModelBuilder
        : public ImplicitStructuralModelBuilder
    {
    public:
        StratigraphicModelBuilder( StratigraphicModel& stratigraphic_model_ );

        void copy( const StratigraphicModel& stratigraphic_model_ );

        void instantiate_stratigraphic_attribute_on_blocks();

        void set_stratigraphic_location(
            const Block3D& block, index_t vertex_id, Point2D value );

        void set_stratigraphic_coordinates( const Block3D& block,
            index_t vertex_id,
            const StratigraphicPoint3D& value );

    private:
        StratigraphicModel& stratigraphic_model_;
    };
} // namespace geode
