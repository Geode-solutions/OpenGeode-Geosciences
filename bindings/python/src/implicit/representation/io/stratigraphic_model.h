/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include "../../factory.h"

#include <geode/geosciences/implicit/representation/io/stratigraphic_model_input.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_model_output.h>

namespace geode
{
    void define_stratigraphic_model_io( pybind11::module& module )
    {
        module.def( "save_stratigraphic_model", &save_stratigraphic_model );
        module.def( "load_stratigraphic_model", &load_stratigraphic_model );
        PYTHON_FACTORY_CLASS( StratigraphicModelInputFactory );
        PYTHON_FACTORY_CLASS( StratigraphicModelOutputFactory );
    }
} // namespace geode
