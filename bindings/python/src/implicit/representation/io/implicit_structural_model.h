/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include "../../factory.h"

#include <geode/geosciences/implicit/representation/io/implicit_structural_model_input.h>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_output.h>

namespace geode
{
    void define_implicit_structural_model_io( pybind11::module& module )
    {
        module.def(
            "save_implicit_structural_model", &save_implicit_structural_model );
        module.def(
            "load_implicit_structural_model", &load_implicit_structural_model );
        PYTHON_FACTORY_CLASS( ImplicitStructuralModelInputFactory );
        PYTHON_FACTORY_CLASS( ImplicitStructuralModelOutputFactory );
    }
} // namespace geode
