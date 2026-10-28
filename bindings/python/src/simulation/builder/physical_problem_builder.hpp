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

#include <geode/basic/uuid.hpp>

#include <geode/geometry/vector.hpp>

#include <geode/geosciences/simulation/builder/physical_problem_builder.hpp>
#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/source_term.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

namespace geode
{
    void define_physical_problem_builder( pybind11::module& module )
    {
        pybind11::class_< PhysicalProblemBuilder, IdentifierBuilder >(
            module, "PhysicalProblemBuilder" )
            .def( pybind11::init< PhysicalProblem& >(),
                pybind11::kep_alive< 1, 2 >() )
            .def( "copy", &PheysicalProblemBuilder::copy )
            .def( "set_model_id", &PhysicalProblemBuilder::set_model_id )
            .def( "set_flow", &PhysicalProblemBuilder::set_flow )
            .def( "set_thermal", &PhysicalProblemBuilder::set_thermal )
            .def( "add_material", &PhysicalProblemBuilder::add_material )
            .def( "set_material", &PhysicalProblemBuilder::set_material )
            .def( "remove_material", &PhysicalProblemBuilder::remove_material )
            .def( "add_initial_condition",
                &PhysicalProblemBuilder::add_initial_condition )
            .def( "remove_initial_condition",
                &PhysicalProblemBuilder::remove_initial_condition )
            .def( "add_source_term", &PhysicalProblemBuilder::add_source_term )
            .def( "remove_source_term",
                &PhysicalProblemBuilder::remove_source_term )
            .def( "set_schedule", &PhysicalProblemBuilder::set_schedule )
            .def( "set_output", &PhysicalProblemBuilder::set_output )
            .def( "set_gravity", &PhysicalProblemBuilder::set_gravity )
            .def( "set_solver_option",
                &PhysicalProblemBuilder::set_solver_option );
    }
} // namespace geode
