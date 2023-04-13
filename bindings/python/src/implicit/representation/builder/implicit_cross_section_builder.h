/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.h>

#include <geode/geometry/point.h>

namespace geode
{
    void define_implicit_cross_section_builder( pybind11::module& module )
    {
        pybind11::class_< ImplicitCrossSectionBuilder, CrossSectionBuilder >(
            module, "ImplicitCrossSectionBuilder" )
            .def( pybind11::init< ImplicitCrossSection& >() )
            .def( "copy", ( void( ImplicitCrossSectionBuilder::* )(
                              const ImplicitCrossSection& implicit_section ) )
                              & ImplicitCrossSectionBuilder::copy )
            .def( "instantiate_implicit_attribute_on_surfaces",
                &ImplicitCrossSectionBuilder::
                    instantiate_implicit_attribute_on_surfaces )
            .def( "set_implicit_value",
                &ImplicitCrossSectionBuilder::set_implicit_value )
            .def( "set_stratigraphic_units_stack",
                []( ImplicitCrossSectionBuilder& builder,
                    StratigraphicUnitsStack2D& su_stack ) {
                    builder.set_stratigraphic_units_stack(
                        std::move( su_stack ) );
                } )
            .def( "set_horizon_isovalue",
                &ImplicitCrossSectionBuilder::set_horizon_isovalue );
    }
} // namespace geode