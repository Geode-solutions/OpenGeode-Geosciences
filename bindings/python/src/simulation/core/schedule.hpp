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

#include <geode/geosciences/simulation/core/schedule.hpp>

namespace geode
{
    void define_schedule( pybind11::module& module )
    {
        pybind11::class_< Schedule >( module, "Schedule" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( double start_time, double end_time,
                                      double initial_time_step,
                                      std::optional< double > max_time_step ) {
                return Schedule{ start_time, end_time, initial_time_step,
                    max_time_step };
            } ),
                pybind11::arg( "start_time" ), pybind11::arg( "end_time" ),
                pybind11::arg( "initial_time_step" ),
                pybind11::arg( "max_time_step" ) = pybind11::none() )
            .def_readwrite( "start_time", &Schedule::start_time )
            .def_readwrite( "end_time", &Schedule::end_time )
            .def_readwrite( "initial_time_step", &Schedule::initial_time_step )
            .def_readwrite( "max_time_step", &Schedule::max_time_step );

        pybind11::class_< OutputSettings >( module, "OutputSettings" )
            .def( pybind11::init<>() )
            .def_readwrite( "frequency", &OutputSettings::frequency )
            .def_readwrite( "times", &OutputSettings::times )
            .def_readwrite( "binary", &OutputSettings::binary )
            .def_readwrite( "restart", &OutputSettings::restart )
            .def_readwrite(
                "restart_frequency", &OutputSettings::restart_frequency );
    }
} // namespace geode
