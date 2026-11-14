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

#include <geode/geosciences/simulation/core/source_term.hpp>

namespace geode
{
    void define_source_term( pybind11::module& module )
    {
        pybind11::class_< SourceTerm > term_class( module, "SourceTerm",
            "Constant rate of one fluid component, spread over the cells "
            "perforated by a well. The wells are located by the simulator "
            "exporter, which knows them by name" );
        term_class
            .def( pybind11::init<>(), "Build a source term with a null rate" )
            .def( pybind11::init( []( std::string well, std::string component,
                                      double rate ) {
                return SourceTerm{ std::move( well ), std::move( component ),
                    rate };
            } ),
                pybind11::arg( "well" ), pybind11::arg( "component" ),
                pybind11::arg( "rate" ),
                "Build a source term from the name of the well, the name of a "
                "component of the fluid and the rate" )
            .def(
                "__repr__",
                []( const SourceTerm& term ) {
                    return python_call_repr( "SourceTerm",
                        { "well=" + python_repr( term.well ),
                            "component=" + python_repr( term.component ),
                            "rate=" + python_repr( term.rate ) } );
                },
                "Return an unambiguous description of the source term, which "
                "can be evaluated" );
        def_readwrite_named( term_class, "well", &SourceTerm::well,
            "Name of the well perforations receiving the source" );
        def_readwrite_named( term_class, "component", &SourceTerm::component,
            "Name of a component of the fluid of the problem" );
        def_readwrite_named( term_class, "rate", &SourceTerm::rate,
            "Rate in kg/s, or mol/s for a mole based multiphase flow; "
            "positive injects, negative produces" );
    }
} // namespace geode
