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

#include <geode/geosciences/simulation/core/schedule.hpp>

namespace geode
{
    void define_schedule( pybind11::module& module )
    {
        pybind11::class_< Schedule > schedule_class(
            module, "Schedule", "Simulated time interval, in seconds" );
        schedule_class
            .def( pybind11::init<>(), "Build a schedule with null times" )
            .def( pybind11::init( []( double start_time, double end_time,
                                      double initial_time_step,
                                      std::optional< double > max_time_step ) {
                return Schedule{ start_time, end_time, initial_time_step,
                    max_time_step };
            } ),
                pybind11::arg( "start_time" ), pybind11::arg( "end_time" ),
                pybind11::arg( "initial_time_step" ),
                pybind11::arg( "max_time_step" ) = pybind11::none(),
                "Build a schedule from the start and end times, the initial "
                "time step and the optional maximum time step, in seconds" )
            .def(
                "__repr__",
                []( const Schedule& schedule ) {
                    return python_call_repr( "Schedule",
                        { "start_time=" + python_repr( schedule.start_time ),
                            "end_time=" + python_repr( schedule.end_time ),
                            "initial_time_step="
                                + python_repr( schedule.initial_time_step ),
                            "max_time_step="
                                + python_repr( schedule.max_time_step ) } );
                },
                "Return an unambiguous description of the schedule, which can "
                "be evaluated" );
        def_readwrite_named( schedule_class, "start_time",
            &Schedule::start_time, "Start time in seconds" );
        def_readwrite_named( schedule_class, "end_time", &Schedule::end_time,
            "End time in seconds" );
        def_readwrite_named( schedule_class, "initial_time_step",
            &Schedule::initial_time_step, "Initial time step in seconds" );
        def_readwrite_named( schedule_class, "max_time_step",
            &Schedule::max_time_step, "Maximum time step in seconds, or None" );

        pybind11::class_< OutputSettings > output_class( module,
            "OutputSettings", "When the results are written, in seconds" );
        output_class
            .def( pybind11::init<>(),
                "Build settings with binary output, without restart" )
            .def(
                "__repr__",
                []( const OutputSettings& output ) {
                    return "OutputSettings(frequency="
                           + python_repr( output.frequency )
                           + ", times=" + python_repr( output.times )
                           + ", binary=" + python_repr( output.binary )
                           + ", restart=" + python_repr( output.restart )
                           + ", restart_frequency="
                           + python_repr( output.restart_frequency ) + ")";
                },
                "Return an unambiguous description of the settings" );
        def_readwrite_named( output_class, "frequency",
            &OutputSettings::frequency,
            "Output frequency in seconds, or None" );
        def_readwrite_named( output_class, "times", &OutputSettings::times,
            "Output times in seconds" );
        def_readwrite_named( output_class, "binary", &OutputSettings::binary,
            "True to write binary files" );
        def_readwrite_named( output_class, "restart", &OutputSettings::restart,
            "True to write a restart file at the end of the run" );
        def_readwrite_named( output_class, "restart_frequency",
            &OutputSettings::restart_frequency,
            "Frequency in seconds of the periodic restart files, or None" );
    }
} // namespace geode
