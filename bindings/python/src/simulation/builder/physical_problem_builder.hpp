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

#pragma once

#include <geode/basic/uuid.hpp>

#include <geode/geometry/vector.hpp>

#include <geode/geosciences/simulation/builder/physical_problem_builder.hpp>
#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/solid_mechanics.hpp>
#include <geode/geosciences/simulation/core/source_term.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

namespace geode
{
    void define_physical_problem_builder( pybind11::module& module )
    {
        pybind11::class_< PhysicalProblemBuilder, IdentifierBuilder >( module,
            "PhysicalProblemBuilder",
            "Builder of a PhysicalProblem: the only way to modify it. The "
            "flow, fluid, laws and initial conditions given to it are "
            "shared, not copied: do not modify them afterwards. Removing a "
            "material, an initial condition or a source term shifts down the "
            "indices of the following ones" )
            .def( pybind11::init< PhysicalProblem& >(),
                pybind11::arg( "problem" ), pybind11::keep_alive< 1, 2 >(),
                "Build a builder of the problem, which is kept alive by the "
                "builder" )
            .def( "copy", &PhysicalProblemBuilder::copy,
                pybind11::arg( "other" ),
                "Copy the content of the other problem into the problem; the "
                "flow, fluid, laws and initial conditions are shared" )
            .def( "set_model_id", &PhysicalProblemBuilder::set_model_id,
                pybind11::arg( "model_id" ),
                "Record the uuid of the model the problem is built for" )
            .def( "set_flow", &PhysicalProblemBuilder::set_flow,
                pybind11::arg( "flow" ),
                "Set the FlowModel of the problem, which is shared" )
            .def( "set_thermal", &PhysicalProblemBuilder::set_thermal,
                pybind11::arg( "thermal" ),
                "Set the ThermalParameters: the problem solves the energy "
                "equation (problems are isothermal by default)" )
            .def( "set_solid_mechanics",
                &PhysicalProblemBuilder::set_solid_mechanics,
                pybind11::arg( "mechanics" ),
                "Set the SolidMechanics: the problem solves quasi-static "
                "solid mechanics" )
            .def( "remove_solid_mechanics",
                &PhysicalProblemBuilder::remove_solid_mechanics,
                "Remove the SolidMechanics" )
            .def( "add_material", &PhysicalProblemBuilder::add_material,
                pybind11::arg( "material" ),
                "Add the Material and return its index" )
            .def( "set_material", &PhysicalProblemBuilder::set_material,
                pybind11::arg( "material_id" ), pybind11::arg( "material" ),
                "Replace the Material of the given index; raise an error if "
                "there is none" )
            .def( "remove_material", &PhysicalProblemBuilder::remove_material,
                pybind11::arg( "material_id" ),
                "Remove the Material of the given index; raise an error if "
                "there is none" )
            .def( "add_initial_condition",
                &PhysicalProblemBuilder::add_initial_condition,
                pybind11::arg( "initial_condition" ),
                "Add the initial condition, which is shared, and return its "
                "index" )
            .def( "remove_initial_condition",
                &PhysicalProblemBuilder::remove_initial_condition,
                pybind11::arg( "initial_condition_id" ),
                "Remove the initial condition of the given index; raise an "
                "error if there is none" )
            .def( "add_source_term", &PhysicalProblemBuilder::add_source_term,
                pybind11::arg( "source_term" ),
                "Add the SourceTerm and return its index" )
            .def( "remove_source_term",
                &PhysicalProblemBuilder::remove_source_term,
                pybind11::arg( "source_term_id" ),
                "Remove the SourceTerm of the given index; raise an error if "
                "there is none" )
            .def( "set_schedule", &PhysicalProblemBuilder::set_schedule,
                pybind11::arg( "schedule" ), "Set the Schedule" )
            .def( "set_output", &PhysicalProblemBuilder::set_output,
                pybind11::arg( "output" ), "Set the OutputSettings" )
            .def( "set_gravity", &PhysicalProblemBuilder::set_gravity,
                pybind11::arg( "gravity" ), "Set the gravity vector in m/s2" )
            .def( "set_solver_option",
                &PhysicalProblemBuilder::set_solver_option,
                pybind11::arg( "simulator" ), pybind11::arg( "key" ),
                pybind11::arg( "value" ),
                "Set the value of the key of the SolverOptions of the "
                "simulator" );
    }
} // namespace geode
