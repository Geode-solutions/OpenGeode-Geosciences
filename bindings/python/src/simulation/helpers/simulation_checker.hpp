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

#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/helpers/simulation_checker.hpp>

namespace geode
{
    void define_simulation_checker( pybind11::module& module )
    {
        pybind11::class_< SimulationChecker >( module, "SimulationChecker" )
            .def( "check", &SimulationChecker::check );

        pybind11::class_< SimulationCheckerFactory >(
            module, "SimulationCheckerFactory" )
            .def_static(
                "list_creators", &SimulationCheckerFactory::list_creators )
            .def_static( "has_creator", &SimulationCheckerFactory::has_creator )
            .def_static(
                "create",
                []( const std::string& key, const PhysicalProblem& problem,
                    const BRep& model ) {
                    return SimulationCheckerFactory::create(
                        key, problem, model );
                },
                pybind11::keep_alive< 0, 2 >(),
                pybind11::keep_alive< 0, 3 >() );
    }
} // namespace geode
