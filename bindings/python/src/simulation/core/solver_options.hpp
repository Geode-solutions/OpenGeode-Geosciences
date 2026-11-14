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

#include <geode/geosciences/simulation/core/solver_options.hpp>

namespace geode
{
    void define_solver_options( pybind11::module& module )
    {
        pybind11::class_< SolverOptions >( module, "SolverOptions",
            "Simulator specific numerical settings, stored as text for each "
            "simulator (e.g. simulator geos, key nonlinear/newtonTol, value "
            "1e-6). Each simulator writer documents its keys" )
            .def( pybind11::init<>(), "Build empty options" )
            .def( "set", &SolverOptions::set, pybind11::arg( "simulator" ),
                pybind11::arg( "key" ), pybind11::arg( "value" ),
                "Set the value of the key of the simulator" )
            .def( "get", &SolverOptions::get, pybind11::arg( "simulator" ),
                pybind11::arg( "key" ),
                "Return the value of the key of the simulator, or None if it "
                "has none" )
            .def( "options", &SolverOptions::options,
                pybind11::arg( "simulator" ),
                "Return the options of the simulator as a dict sorted by "
                "key, empty if there is none" );
    }
} // namespace geode
