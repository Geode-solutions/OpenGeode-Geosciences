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

#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.hpp>

namespace geode
{
    void define_stratigraphic_relationships( pybind11::module& module )
    {
        pybind11::class_< StratigraphicRelationships, pybind11::smart_holder >(
            module, "StratigraphicRelationships",
            "Above-under relations between geological components, identified "
            "by their uuids, in the stratigraphy" )
            .def( pybind11::init<>(), "Build an empty set of relations" )
            .def( "is_above", &StratigraphicRelationships::is_above,
                pybind11::arg( "above" ), pybind11::arg( "under" ),
                "Return True if the component above is above the component "
                "under, directly or through other components" )
            .def( "is_directly_above",
                &StratigraphicRelationships::is_directly_above,
                pybind11::arg( "above" ), pybind11::arg( "under" ),
                "Return True if the component above is directly above the "
                "component under" )
            .def( "above", &StratigraphicRelationships::above,
                pybind11::arg( "element" ),
                "Return the uuid of the component directly above the "
                "element, or None if there is none" )
            .def( "under", &StratigraphicRelationships::under,
                pybind11::arg( "element" ),
                "Return the uuid of the component directly under the "
                "element, or None if there is none" );
    }
} // namespace geode
