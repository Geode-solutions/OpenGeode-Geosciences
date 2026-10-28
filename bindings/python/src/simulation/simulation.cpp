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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <geode/geosciences/simulation/common.hpp>

#include "core/flow_model.hpp"
#include "core/fluid.hpp"
#include "core/initial_condition.hpp"
#include "core/material.hpp"
#include "core/physical_problem.hpp"
#include "core/saturation_functions.hpp"
#include "core/schedule.hpp"
#include "core/solver_options.hpp"
#include "core/source_term.hpp"
#include "core/thermal_parameters.hpp"

#include "builder/physical_problem_builder.hpp"

#include "helpers/physical_problem_validation.hpp"

#include "io/physical_problem.hpp"

PYBIND11_MODULE( opengeode_geosciences_py_simulation, module )
{
    module.doc() = "OpenGeode-Geosciences Python binding for simulation";
    pybind11::module::import( "opengeode" );
    pybind11::class_< geode::OpenGeodeGeosciencesSimulationLibrary >(
        module, "OpenGeodeGeosciencesSimulationLibrary" )
        .def( "initialize",
            &geode::OpenGeodeGeosciencesSimulationLibrary::initialize );

    geode::define_fluid( module );
    geode::define_flow_model( module );
    geode::define_saturation_functions( module );
    geode::define_material( module );
    geode::define_initial_condition( module );
    geode::define_schedule( module );
    geode::define_source_term( module );
    geode::define_thermal_parameters( module );
    geode::define_solver_options( module );

    geode::define_physical_problem( module );
    geode::define_physical_problem_builder( module );

    geode::define_physical_problem_validation( module );

    geode::define_physical_problem_io( module );
}
