/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include "../../factory.h"

#include <geode/geosciences/implicit/representation/io/stratigraphic_section_input.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_section_output.h>

namespace geode
{
    void define_stratigraphic_section_io( pybind11::module& module )
    {
        module.def( "save_stratigraphic_section", &save_stratigraphic_section );
        module.def( "load_stratigraphic_section", &load_stratigraphic_section );
        PYTHON_FACTORY_CLASS( StratigraphicSectionInputFactory );
        PYTHON_FACTORY_CLASS( StratigraphicSectionOutputFactory );
    }
} // namespace geode
