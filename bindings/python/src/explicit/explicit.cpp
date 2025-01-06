/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include "geometry/crs.hpp"
#include "geometry/crs_helper.hpp"

#include "mixin/builder/fault_blocks_builder.hpp"
#include "mixin/builder/faults_builder.hpp"
#include "mixin/builder/horizons_builder.hpp"
#include "mixin/builder/stratigraphic_units_builder.hpp"

#include "mixin/core/fault.hpp"
#include "mixin/core/fault_block.hpp"
#include "mixin/core/fault_blocks.hpp"
#include "mixin/core/faults.hpp"
#include "mixin/core/horizon.hpp"
#include "mixin/core/horizons.hpp"
#include "mixin/core/stratigraphic_unit.hpp"
#include "mixin/core/stratigraphic_units.hpp"

#include "representation/builder/cross_section_builder.hpp"
#include "representation/builder/structural_model_builder.hpp"

#include "representation/core/cross_section.hpp"
#include "representation/core/structural_model.hpp"

#include "representation/io/cross_section.hpp"
#include "representation/io/structural_model.hpp"

PYBIND11_MODULE( opengeode_geosciences_py_explicit, module )
{
    module.doc() = "OpenGeode-Geosciences Python binding for explicit library";
    pybind11::module::import( "opengeode" );
    pybind11::class_< geode::GeosciencesExplicitLibrary >(
        module, "GeosciencesExplicitLibrary" )
        .def( "initialize", &geode::GeosciencesExplicitLibrary::initialize );
    geode::define_fault_block( module );
    geode::define_fault( module );
    geode::define_horizon( module );
    geode::define_stratigraphic_unit( module );
    geode::define_fault_blocks( module );
    geode::define_faults( module );
    geode::define_horizons( module );
    geode::define_stratigraphic_units( module );

    geode::define_fault_blocks_builder( module );
    geode::define_faults_builder( module );
    geode::define_horizons_builder( module );
    geode::define_stratigraphic_units_builder( module );

    geode::define_structural_model( module );
    geode::define_cross_section( module );

    geode::define_structural_model_builder( module );
    geode::define_cross_section_builder( module );

    geode::define_structural_model_io( module );
    geode::define_cross_section_io( module );

    geode::define_crs( module );
    geode::define_crs_helper( module );
}