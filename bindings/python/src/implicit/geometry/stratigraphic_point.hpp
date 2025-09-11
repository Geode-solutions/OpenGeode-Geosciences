/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geosciences/implicit/geometry/stratigraphic_point.hpp>

#define PYTHON_STRATIGRAPHIC_POINT( dimension )                                \
    const auto name##dimension =                                               \
        "StratigraphicPoint" + std::to_string( dimension ) + "D";              \
    pybind11::class_< StratigraphicPoint##dimension##D >(                      \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init<>() )                                             \
        .def( pybind11::init< Point< dimension - 1 >, double >() )             \
        .def( pybind11::init< std::array< double, dimension > >() )            \
        .def( pybind11::init< Point< dimension > >() )                         \
        .def( "stratigraphic_location",                                        \
            &StratigraphicPoint##dimension##D::stratigraphic_location )        \
        .def( "implicit_value",                                                \
            &StratigraphicPoint##dimension##D::implicit_value )                \
        .def( "stratigraphic_coordinates",                                     \
            &StratigraphicPoint##dimension##D::stratigraphic_coordinates )     \
        .def( "set_stratigraphic_location",                                    \
            &StratigraphicPoint##dimension##D::set_stratigraphic_location )    \
        .def( "set_implicit_value",                                            \
            &StratigraphicPoint##dimension##D::set_implicit_value )            \
        .def( "string", &StratigraphicPoint##dimension##D::string )

namespace geode
{
    void define_stratigraphic_point( pybind11::module& module )
    {
        PYTHON_STRATIGRAPHIC_POINT( 3 );
        PYTHON_STRATIGRAPHIC_POINT( 2 );
    }
} // namespace geode
