/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.h>

#include <geode/geometry/point.h>

#include <geode/model/mixin/core/block.h>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>

namespace geode
{
    StratigraphicModelBuilder::StratigraphicModelBuilder(
        StratigraphicModel& stratigraphic_model )
        : ImplicitStructuralModelBuilder( stratigraphic_model ),
          stratigraphic_model_( stratigraphic_model )
    {
    }

    void StratigraphicModelBuilder::copy(
        const StratigraphicModel& implicit_model )
    {
        ImplicitStructuralModelBuilder::copy( implicit_model );
        stratigraphic_model_.instantiate_stratigraphic_location_on_blocks( {} );
    }

    void StratigraphicModelBuilder::
        instantiate_stratigraphic_attribute_on_blocks()
    {
        this->instantiate_implicit_attribute_on_blocks();
        stratigraphic_model_.instantiate_stratigraphic_location_on_blocks( {} );
    }

    void StratigraphicModelBuilder::set_stratigraphic_location(
        const Block3D& block, index_t vertex_id, Point2D value )
    {
        stratigraphic_model_.set_stratigraphic_location(
            block, vertex_id, std::move( value ), {} );
    }

    void StratigraphicModelBuilder::set_stratigraphic_coordinates(
        const Block3D& block,
        index_t vertex_id,
        const StratigraphicPoint3D& value )
    {
        this->set_implicit_value( block, vertex_id, value.implicit_value() );
        stratigraphic_model_.set_stratigraphic_location(
            block, vertex_id, value.stratigraphic_location(), {} );
    }
} // namespace geode
