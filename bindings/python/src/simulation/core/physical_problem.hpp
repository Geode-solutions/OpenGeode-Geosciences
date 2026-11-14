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

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/solver_options.hpp>
#include <geode/geosciences/simulation/core/source_term.hpp>

namespace geode
{
    void define_physical_problem( pybind11::module& module )
    {
        pybind11::class_< PhysicalProblem, Identifier, pybind11::smart_holder >(
            module, "PhysicalProblem",
            "Simulator independent description of a flow, solid mechanics or "
            "coupled (poromechanics: both are set) simulation on a model: "
            "fluid, flow process, rock materials, initial state, well source "
            "terms, time schedule, outputs and simulator options. The model "
            "regions are referenced by component uuid. The data varying in "
            "space stay in the PhysicalProperties of the model. It is "
            "modified through PhysicalProblemBuilder" )
            .def( pybind11::init<>(), "Build an empty problem" )
            .def(
                "__repr__",
                []( const PhysicalProblem& problem ) {
                    return identifier_repr( problem, "PhysicalProblem",
                        { { "has_flow", python_repr( problem.has_flow() ) },
                            { "nb_materials",
                                python_repr( problem.materials().size() ) },
                            { "nb_initial_conditions",
                                python_repr(
                                    problem.nb_initial_conditions() ) },
                            { "nb_source_terms",
                                python_repr(
                                    problem.source_terms().size() ) } } );
                },
                "Return an unambiguous description of the problem: its id, "
                "its name if it has one, whether it has a flow and its "
                "numbers of materials, initial conditions and source terms" )
            .def( "clone", &PhysicalProblem::clone,
                "Return a copy of the problem; the flow, fluid, laws and "
                "initial conditions are shared with the copy" )
            .def_static( "native_extension_static",
                &PhysicalProblem::native_extension_static,
                "Return the extension of the native file format of the "
                "problem (og_phpb)" )
            .def( "native_extension", &PhysicalProblem::native_extension,
                "Return the extension of the native file format of the "
                "problem (og_phpb)" )
            .def( "model_id", &PhysicalProblem::model_id,
                "Return the uuid of the model the problem was built for, or "
                "None if it was not recorded" )
            .def( "has_flow", &PhysicalProblem::has_flow,
                "Return True if the problem has a flow model" )
            .def( "flow", &PhysicalProblem::flow,
                pybind11::return_value_policy::reference_internal,
                "Return the FlowModel; raise an error if the problem has "
                "none" )
            .def( "fluid", &PhysicalProblem::fluid,
                pybind11::return_value_policy::reference_internal,
                "Return the FluidModel of the flow model; raise an error if "
                "there is no flow model or it has no fluid" )
            .def( "thermal", &PhysicalProblem::thermal,
                "Return the ThermalParameters if the problem solves the "
                "energy equation, else None (isothermal)" )
            .def( "solid_mechanics", &PhysicalProblem::solid_mechanics,
                "Return the SolidMechanics if the problem solves solid "
                "mechanics, else None" )
            .def(
                "materials",
                []( const PhysicalProblem& problem ) {
                    const auto materials = problem.materials();
                    return std::vector< Material >{ materials.begin(),
                        materials.end() };
                },
                "Return a list of copies of the Materials; the laws they "
                "refer to are shared with the problem" )
            .def( "nb_initial_conditions",
                &PhysicalProblem::nb_initial_conditions,
                "Return the number of initial conditions" )
            .def( "initial_condition", &PhysicalProblem::initial_condition,
                pybind11::arg( "initial_condition_id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the initial condition of the given index; raise an "
                "error if there is none" )
            .def(
                "source_terms",
                []( const PhysicalProblem& problem ) {
                    const auto source_terms = problem.source_terms();
                    return std::vector< SourceTerm >{ source_terms.begin(),
                        source_terms.end() };
                },
                "Return a list of copies of the SourceTerms" )
            .def( "schedule", &PhysicalProblem::schedule,
                "Return a copy of the Schedule" )
            .def( "output", &PhysicalProblem::output,
                "Return a copy of the OutputSettings" )
            .def( "gravity", &PhysicalProblem::gravity,
                "Return the gravity vector in m/s2, (0, 0, -9.81) by default" )
            .def( "solver_options", &PhysicalProblem::solver_options,
                pybind11::return_value_policy::reference_internal,
                "Return the SolverOptions of the problem" );
    }
} // namespace geode
