/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include "../../factory.h"

#include <geode/geosciences/implicit/representation/io/implicit_cross_section_input.h>
#include <geode/geosciences/implicit/representation/io/implicit_cross_section_output.h>

namespace geode
{
    void define_implicit_cross_section_io( pybind11::module& module )
    {
        module.def(
            "save_implicit_cross_section", &save_implicit_cross_section );
        module.def(
            "load_implicit_cross_section", &load_implicit_cross_section );
        PYTHON_FACTORY_CLASS( ImplicitCrossSectionInputFactory );
        PYTHON_FACTORY_CLASS( ImplicitCrossSectionOutputFactory );
    }
} // namespace geode
