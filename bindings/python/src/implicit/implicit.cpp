/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "../common.hpp"

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "geometry/stratigraphic_point.hpp"
#include "mixin/builder/stratigraphic_relationships_builder.hpp"
#include "mixin/core/stratigraphic_relationships.hpp"
#include "representation/builder/horizons_stack_builder.hpp"
#include "representation/builder/implicit_cross_section_builder.hpp"
#include "representation/builder/implicit_structural_model_builder.hpp"
#include "representation/builder/stratigraphic_model_builder.hpp"
#include "representation/builder/stratigraphic_section_builder.hpp"
#include "representation/core/helpers.hpp"
#include "representation/core/horizons_stack.hpp"
#include "representation/core/implicit_cross_section.hpp"
#include "representation/core/implicit_structural_model.hpp"
#include "representation/core/stratigraphic_model.hpp"
#include "representation/core/stratigraphic_section.hpp"
#include "representation/io/horizons_stack.hpp"
#include "representation/io/implicit_cross_section.hpp"
#include "representation/io/implicit_structural_model.hpp"
#include "representation/io/stratigraphic_model.hpp"
#include "representation/io/stratigraphic_section.hpp"

PYBIND11_MODULE( opengeode_geosciences_py_implicit, module )
{
    pybind11::module::import( "opengeode" );
    geode::import_binding_dependency(
        module, "opengeode_geosciences_py_explicit" );
    module.doc() =
        "Implicit geosciences models of OpenGeode-Geosciences: HorizonsStack "
        "(the stacking of Horizons and StratigraphicUnits), "
        "ImplicitStructuralModel and ImplicitCrossSection (a StructuralModel "
        "and a CrossSection with an implicit value on the vertices and a "
        "HorizonsStack), StratigraphicModel and StratigraphicSection (which "
        "also store a stratigraphic location), their builders, the "
        "functions loading and saving them (load_implicit_structural_model, "
        "save_horizons_stack3D...) and helpers (rescaling the implicit "
        "values, building a HorizonsStack from names...). Call "
        "OpenGeodeGeosciencesImplicitLibrary.initialize() before using the "
        "module.";
    pybind11::class_< geode::OpenGeodeGeosciencesImplicitLibrary >( module,
        "OpenGeodeGeosciencesImplicitLibrary",
        "Initialization of the implicit geosciences module" )
        .def_static( "initialize",
            &geode::OpenGeodeGeosciencesImplicitLibrary::initialize,
            "Register the native formats of the HorizonsStack and of the "
            "implicit models in the factories; also initializes "
            "OpenGeodeGeosciencesExplicitLibrary. Calling it again has no "
            "effect" );
    geode::define_stratigraphic_point( module );

    geode::define_stratigraphic_relationships( module );
    geode::define_stratigraphic_relationships_builder( module );

    geode::define_horizons_stack( module );
    geode::define_horizons_stack_builder( module );
    geode::define_implicit_cross_section( module );
    geode::define_implicit_structural_model( module );
    geode::define_stratigraphic_model( module );
    geode::define_stratigraphic_section( module );
    geode::define_implicit_cross_section_builder( module );
    geode::define_implicit_structural_model_builder( module );
    geode::define_stratigraphic_model_builder( module );
    geode::define_stratigraphic_section_builder( module );

    geode::define_implicit_cross_section_io( module );
    geode::define_implicit_structural_model_io( module );
    geode::define_stratigraphic_model_io( module );
    geode::define_stratigraphic_section_io( module );
    geode::define_horizons_stack_io( module );

    geode::detail::define_implicit_model_helpers( module );
}