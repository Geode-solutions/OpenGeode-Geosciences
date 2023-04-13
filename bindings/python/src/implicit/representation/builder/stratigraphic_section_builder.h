/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/stratigraphic_section_builder.h>

#include <geode/geometry/point.h>

namespace geode
{
    void define_stratigraphic_section_builder( pybind11::module& module )
    {
        pybind11::class_< StratigraphicSectionBuilder, CrossSectionBuilder >(
            module, "StratigraphicSectionBuilder" )
            .def( pybind11::init< StratigraphicSection& >() )
            .def( "copy", ( void( StratigraphicSectionBuilder::* )(
                              const StratigraphicSection& implicit_section ) )
                              & StratigraphicSectionBuilder::copy )
            .def( "instantiate_stratigraphic_attribute_on_surfaces",
                &StratigraphicSectionBuilder::
                    instantiate_stratigraphic_attribute_on_surfaces )
            .def( "set_stratigraphic_location",
                &StratigraphicSectionBuilder::set_stratigraphic_location )
            .def( "set_stratigraphic_coordinates",
                &StratigraphicSectionBuilder::set_stratigraphic_coordinates );
    }
} // namespace geode