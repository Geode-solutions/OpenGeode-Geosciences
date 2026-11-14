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

#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/helpers/simulation_checker.hpp>

namespace geode
{
    void define_simulation_checker( pybind11::module& module )
    {
        pybind11::class_< SimulationChecker >( module, "SimulationChecker",
            "Checks that a simulator has everything it needs to simulate a "
            "PhysicalProblem on a BRep. The checkers are registered by the "
            "simulator libraries in SimulationCheckerFactory, under the "
            "simulator name in lower case" )
            .def( "check", &SimulationChecker::check,
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Check the problem and the model and return the "
                "SimulationReport with every issue found: the simulator "
                "independent checks, then the simulator ones. Each call "
                "starts a new report" );
        pybind11::class_< SimulationCheckerFactory >( module,
            "SimulationCheckerFactory",
            "Registry of the simulation checkers, identified by the "
            "simulator name in lower case" )
            .def_static( "list_creators",
                &SimulationCheckerFactory::list_creators,
                "Return the names of the registered simulators" )
            .def_static( "has_creator", &SimulationCheckerFactory::has_creator,
                pybind11::arg( "key" ),
                "Return True if a checker is registered for this simulator "
                "name" )
            .def_static(
                "create",
                []( const std::string& key, const PhysicalProblem& problem,
                    const BRep& model ) {
                    return SimulationCheckerFactory::create(
                        key, problem, model );
                },
                pybind11::arg( "key" ), pybind11::arg( "problem" ),
                pybind11::arg( "model" ), pybind11::keep_alive< 0, 2 >(),
                pybind11::keep_alive< 0, 3 >(),
                "Return the checker of the simulator for the problem and the "
                "model, which it keeps alive; raise an error if no checker is "
                "registered for the simulator name" );
    }
} // namespace geode
