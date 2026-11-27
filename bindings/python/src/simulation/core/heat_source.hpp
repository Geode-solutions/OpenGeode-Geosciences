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

#include <geode/geosciences/simulation/core/heat_source.hpp>

namespace geode
{
    void define_heat_source( pybind11::module& module )
    {
        pybind11::class_< HeatSource > source_class( module, "HeatSource",
            "Heat released in the rock, in W (positive heats, negative cools), "
            "for thermal problems. It is located either in a Block, where it "
            "is spread uniformly over the Block volume, or at a named set of "
            "points given to the simulator exporter, like wells" );
        source_class
            .def( pybind11::init<>(),
                "Build a point heat source with no name and no power" )
            .def( pybind11::init( []( std::string name,
                                      std::optional< uuid > block,
                                      std::vector< double > times,
                                      std::vector< double > powers ) {
                return HeatSource{ std::move( name ), std::move( block ),
                    std::move( times ), std::move( powers ) };
            } ),
                pybind11::arg( "name" ), pybind11::arg( "block" ),
                pybind11::arg( "times" ), pybind11::arg( "powers" ),
                "Build a heat source from its name, its Block (None for a "
                "point heat source), its times in s (empty for a constant "
                "power) and its powers in W (one value if times is empty, "
                "otherwise one per time)" )
            .def(
                "__repr__",
                []( const HeatSource& source ) {
                    return python_call_repr( "HeatSource",
                        { "name=" + python_repr( source.name ),
                            "block=" + python_repr( source.block ),
                            "times=" + python_repr( source.times ),
                            "powers=" + python_repr( source.powers ) } );
                },
                "Return an unambiguous description of the heat source, which "
                "can be evaluated" );
        def_readwrite_named( source_class, "name", &HeatSource::name,
            "Unique name; the exporters locate a point heat source by this "
            "name" );
        def_readwrite_named( source_class, "block", &HeatSource::block,
            "Block releasing the heat, or None for a point heat source" );
        def_readwrite_named( source_class, "times", &HeatSource::times,
            "Times in s, strictly increasing; empty for a constant power" );
        def_readwrite_named( source_class, "powers", &HeatSource::powers,
            "Powers in W: one value if times is empty, otherwise one per "
            "time, linear between times and held before the first and after "
            "the last" );
    }
} // namespace geode
