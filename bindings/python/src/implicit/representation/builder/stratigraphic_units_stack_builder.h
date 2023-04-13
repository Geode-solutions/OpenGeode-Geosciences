/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/stratigraphic_units_stack_builder.h>

#include <geode/geometry/point.h>

#define PYTHON_SU_STACK_BUILDER( dimension )                                   \
    const auto name##dimension =                                               \
        "StratigraphicUnitsStackBuilder" + std::to_string( dimension ) + "D";  \
    pybind11::class_< StratigraphicUnitsStackBuilder##dimension##D,            \
        StratigraphicRelationshipsBuilder, HorizonsBuilder##dimension##D,      \
        StratigraphicUnitsBuilder##dimension##D, IdentifierBuilder >(          \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< StratigraphicUnitsStack##dimension##D& >() )     \
        .def( "copy", &StratigraphicUnitsStackBuilder##dimension##D::copy )    \
        .def( "copy_components",                                               \
            &StratigraphicUnitsStackBuilder##dimension##D::copy_components )   \
        .def( "add_horizon",                                                   \
            &StratigraphicUnitsStackBuilder##dimension##D::add_horizon )       \
        .def( "add_stratigraphic_unit",                                        \
            &StratigraphicUnitsStackBuilder##dimension##D::                    \
                add_stratigraphic_unit )                                       \
        .def( "add_horizon_above",                                             \
            &StratigraphicUnitsStackBuilder##dimension##D::add_horizon_above ) \
        .def( "add_horizon_under",                                             \
            &StratigraphicUnitsStackBuilder##dimension##D::add_horizon_under ) \
        .def( "remove_horizon",                                                \
            &StratigraphicUnitsStackBuilder##dimension##D::remove_horizon )    \
        .def( "remove_stratigraphic_unit",                                     \
            &StratigraphicUnitsStackBuilder##dimension##D::                    \
                remove_stratigraphic_unit )

namespace geode
{
    void define_stratigraphic_units_stack_builder( pybind11::module& module )
    {
        PYTHON_SU_STACK_BUILDER( 2 );
        PYTHON_SU_STACK_BUILDER( 3 );
    }
} // namespace geode