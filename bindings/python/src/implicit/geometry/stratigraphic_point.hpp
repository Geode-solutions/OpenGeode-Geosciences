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

#include <geode/geosciences/implicit/geometry/stratigraphic_point.hpp>

#define PYTHON_STRATIGRAPHIC_POINT( dimension )                                \
    const auto name##dimension =                                               \
        "StratigraphicPoint" + std::to_string( dimension ) + "D";              \
    using StratigraphicPointType##dimension =                                  \
        StratigraphicPoint##dimension##D;                                      \
    define_string_representation(                                              \
        pybind11::class_< StratigraphicPointType##dimension >( module,         \
            name##dimension.c_str(),                                           \
            "Point of the stratigraphic space: an implicit value and the "     \
            "position of the point in the stratigraphic space, a point of "    \
            "dimension - 1 (the stratigraphic location)" ),                    \
        []( const StratigraphicPointType##dimension& point ) {                 \
            return python_call_repr(                                           \
                "StratigraphicPoint" + std::to_string( dimension ) + "D",      \
                { python_repr( point.stratigraphic_location() ),               \
                    python_repr( point.implicit_value() ) } );                 \
        } )                                                                    \
        .def( pybind11::init<>(),                                              \
            "Build a point with a null location and a null implicit value" )   \
        .def( pybind11::init< Point< dimension - 1 >, double >(),              \
            pybind11::arg( "stratigraphic_location" ),                         \
            pybind11::arg( "implicit_value" ),                                 \
            "Build a point from its stratigraphic location and its implicit "  \
            "value" )                                                          \
        .def( pybind11::init< std::array< double, dimension > >(),             \
            pybind11::arg( "values" ),                                         \
            "Build a point from its coordinates: the stratigraphic location "  \
            "then the implicit value as the last one" )                        \
        .def( pybind11::init< Point< dimension > >(),                          \
            pybind11::arg( "stratigraphic_coordinates" ),                      \
            "Build a point from the stratigraphic coordinates: the "           \
            "stratigraphic location then the implicit value as the last "      \
            "coordinate" )                                                     \
        .def(                                                                  \
            "__eq__",                                                          \
            []( const StratigraphicPointType##dimension& point,                \
                const StratigraphicPointType##dimension& other ) {             \
                return point.stratigraphic_location()                          \
                           == other.stratigraphic_location()                   \
                       && point.implicit_value() == other.implicit_value();    \
            },                                                                 \
            pybind11::arg( "other" ),                                          \
            "Return True if the other point has the same stratigraphic "       \
            "location and implicit value" )                                    \
        .def(                                                                  \
            "__hash__",                                                        \
            []( const StratigraphicPointType##dimension& point ) {             \
                return pybind11::hash(                                         \
                    pybind11::make_tuple( point.stratigraphic_location(),      \
                        point.implicit_value() ) );                            \
            },                                                                 \
            "Return a hash of the stratigraphic location and the implicit "    \
            "value" )                                                          \
        .def( "stratigraphic_location",                                        \
            &StratigraphicPointType##dimension::stratigraphic_location,        \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the stratigraphic location of the point (a point of "      \
            "dimension - 1)" )                                                 \
        .def( "implicit_value",                                                \
            &StratigraphicPointType##dimension::implicit_value,                \
            "Return the implicit value of the point" )                         \
        .def( "stratigraphic_coordinates",                                     \
            &StratigraphicPointType##dimension::stratigraphic_coordinates,     \
            "Return the stratigraphic coordinates of the point: the "          \
            "stratigraphic location then the implicit value as the last "      \
            "coordinate" )                                                     \
        .def( "set_stratigraphic_location",                                    \
            &StratigraphicPointType##dimension::set_stratigraphic_location,    \
            pybind11::arg( "stratigraphic_location" ),                         \
            "Set the stratigraphic location of the point" )                    \
        .def( "set_implicit_value",                                            \
            &StratigraphicPointType##dimension::set_implicit_value,            \
            pybind11::arg( "implicit_value" ),                                 \
            "Set the implicit value of the point" )

namespace geode
{
    void define_stratigraphic_point( pybind11::module& module )
    {
        PYTHON_STRATIGRAPHIC_POINT( 2 );
        PYTHON_STRATIGRAPHIC_POINT( 3 );
    }
} // namespace geode
