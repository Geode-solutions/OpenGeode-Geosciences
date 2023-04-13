/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>

#include <geode/geometry/point.h>

#define PYTHON_STRATIGRAPHIC_UNITS_STACK( dimension )                          \
    const auto name##dimension =                                               \
        "StratigraphicUnitsStack" + std::to_string( dimension ) + "D";         \
    pybind11::class_< StratigraphicUnitsStack##dimension##D,                   \
        StratigraphicRelationships, Horizons##dimension##D,                    \
        StratigraphicUnits##dimension##D, Identifier >(                        \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init<>() )                                             \
        .def( "native_extension",                                              \
            &StratigraphicUnitsStack##dimension##D::native_extension )

namespace geode
{
    void define_stratigraphic_units_stack( pybind11::module& module )
    {
        PYTHON_STRATIGRAPHIC_UNITS_STACK( 2 );
        PYTHON_STRATIGRAPHIC_UNITS_STACK( 3 );
    }
} // namespace geode