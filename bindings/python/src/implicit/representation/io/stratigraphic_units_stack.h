/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include "../../factory.h"

#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_input.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_output.h>

#define PYTHON_STRATIGRAPHIC_UNITS_STACK_IO( dimension )                       \
    const auto save##dimension =                                               \
        "save_stratigraphic_units_stack" + std::to_string( dimension ) + "D";  \
    module.def( save##dimension.c_str(),                                       \
        &save_stratigraphic_units_stack< dimension > );                        \
    const auto load##dimension =                                               \
        "load_stratigraphic_units_stack" + std::to_string( dimension ) + "D";  \
    module.def( load##dimension.c_str(),                                       \
        &load_stratigraphic_units_stack< dimension > );                        \
    PYTHON_FACTORY_CLASS( StratigraphicUnitsStackInputFactory##dimension##D ); \
    PYTHON_FACTORY_CLASS( StratigraphicUnitsStackOutputFactory##dimension##D )

namespace geode
{
    void define_stratigraphic_units_stack_io( pybind11::module& module )
    {
        PYTHON_STRATIGRAPHIC_UNITS_STACK_IO( 2 );
        PYTHON_STRATIGRAPHIC_UNITS_STACK_IO( 3 );
    }
} // namespace geode
