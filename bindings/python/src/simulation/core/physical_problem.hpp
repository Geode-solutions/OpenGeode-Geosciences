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
            module, "PhysicalProblem" )
            .def( pybind11::init<>() )
            .def( "clone", &PhysicalProblem::clone )
            .def_static( "native_extension_static",
                &PhysicalProblem::native_extension_static )
            .def( "native_extension", &PhysicalProblem::native_extension )
            .def( "model_id", &PhysicalProblem::model_id )
            .def( "has_flow", &PhysicalProblem::has_flow )
            .def( "flow", &PhysicalProblem::flow,
                pybind11::return_value_policy::reference_internal )
            .def( "fluid", &PhysicalProblem::fluid,
                pybind11::return_value_policy::reference_internal )
            .def( "thermal", &PhysicalProblem::thermal )
            .def( "materials",
                []( const PhysicalProblem& problem ) {
                    const auto materials = problem.materials();
                    return std::vector< Material >{ materials.begin(),
                        materials.end() };
                } )
            .def( "nb_initial_conditions",
                &PhysicalProblem::nb_initial_conditions )
            .def( "initial_condition", &PhysicalProblem::initial_condition,
                pybind11::return_value_policy::reference_internal )
            .def( "source_terms",
                []( const PhysicalProblem& problem ) {
                    const auto source_terms = problem.source_terms();
                    return std::vector< SourceTerm >{ source_terms.begin(),
                        source_terms.end() };
                } )
            .def( "schedule", &PhysicalProblem::schedule )
            .def( "output", &PhysicalProblem::output )
            .def( "gravity", &PhysicalProblem::gravity )
            .def( "solver_options", &PhysicalProblem::solver_options );
    }
} // namespace geode
