/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.h>

#include <geode/geometry/point.h>

namespace geode
{
    void define_stratigraphic_model_builder( pybind11::module& module )
    {
        pybind11::class_< StratigraphicModelBuilder, StructuralModelBuilder >(
            module, "StratigraphicModelBuilder" )
            .def( pybind11::init< StratigraphicModel& >() )
            .def( "copy", ( void( StratigraphicModelBuilder::* )(
                              const StratigraphicModel& implicit_model ) )
                              & StratigraphicModelBuilder::copy )
            .def( "instantiate_stratigraphic_attribute_on_blocks",
                &StratigraphicModelBuilder::
                    instantiate_stratigraphic_attribute_on_blocks )
            .def( "set_stratigraphic_location",
                &StratigraphicModelBuilder::set_stratigraphic_location )
            .def( "set_stratigraphic_coordinates",
                &StratigraphicModelBuilder::set_stratigraphic_coordinates );
    }
} // namespace geode