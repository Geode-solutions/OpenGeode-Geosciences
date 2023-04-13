/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.h>

#include <geode/geometry/point.h>

namespace geode
{
    void define_implicit_structural_model_builder( pybind11::module& module )
    {
        pybind11::class_< ImplicitStructuralModelBuilder,
            StructuralModelBuilder >( module, "ImplicitStructuralModelBuilder" )
            .def( pybind11::init< ImplicitStructuralModel& >() )
            .def( "copy", ( void( ImplicitStructuralModelBuilder::* )(
                              const ImplicitStructuralModel& implicit_model ) )
                              & ImplicitStructuralModelBuilder::copy )
            .def( "instantiate_implicit_attribute_on_blocks",
                &ImplicitStructuralModelBuilder::
                    instantiate_implicit_attribute_on_blocks )
            .def( "set_implicit_value",
                &ImplicitStructuralModelBuilder::set_implicit_value )
            .def( "set_stratigraphic_units_stack",
                []( ImplicitStructuralModelBuilder& builder,
                    StratigraphicUnitsStack3D& su_stack ) {
                    builder.set_stratigraphic_units_stack(
                        std::move( su_stack ) );
                } )
            .def( "set_horizon_isovalue",
                &ImplicitStructuralModelBuilder::set_horizon_isovalue );
    }
} // namespace geode