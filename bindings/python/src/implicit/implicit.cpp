/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include "../common.h"

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "geometry/stratigraphic_point.h"
#include "mixin/builder/stratigraphic_relationships_builder.h"
#include "mixin/core/stratigraphic_relationships.h"
#include "representation/builder/horizons_stack_builder.h"
#include "representation/builder/implicit_cross_section_builder.h"
#include "representation/builder/implicit_structural_model_builder.h"
#include "representation/builder/stratigraphic_model_builder.h"
#include "representation/builder/stratigraphic_section_builder.h"
#include "representation/core/helpers.h"
#include "representation/core/horizons_stack.h"
#include "representation/core/implicit_cross_section.h"
#include "representation/core/implicit_structural_model.h"
#include "representation/core/stratigraphic_model.h"
#include "representation/core/stratigraphic_section.h"
#include "representation/io/horizons_stack.h"
#include "representation/io/implicit_cross_section.h"
#include "representation/io/implicit_structural_model.h"
#include "representation/io/stratigraphic_model.h"
#include "representation/io/stratigraphic_section.h"

PYBIND11_MODULE( opengeode_geosciences_py_implicit, module )
{
    module.doc() = "OpenGeode-Geosciences Python binding for implicit";
    pybind11::module::import( "opengeode" );
    pybind11::class_< geode::GeosciencesImplicitLibrary >(
        module, "GeosciencesImplicitLibrary" )
        .def( "initialize", &geode::GeosciencesImplicitLibrary::initialize );
    geode::define_stratigraphic_point( module );

    geode::define_stratigraphic_relationships( module );
    geode::define_stratigraphic_relationships_builder( module );

    geode::detail::define_implicit_model_helpers( module );
    geode::define_implicit_cross_section( module );
    geode::define_implicit_structural_model( module );
    geode::define_stratigraphic_model( module );
    geode::define_stratigraphic_section( module );
    geode::define_horizons_stack( module );
    geode::define_implicit_cross_section_builder( module );
    geode::define_implicit_structural_model_builder( module );
    geode::define_stratigraphic_model_builder( module );
    geode::define_stratigraphic_section_builder( module );
    geode::define_horizons_stack_builder( module );

    geode::define_implicit_cross_section_io( module );
    geode::define_implicit_structural_model_io( module );
    geode::define_stratigraphic_model_io( module );
    geode::define_stratigraphic_section_io( module );
    geode::define_horizons_stack_io( module );
}