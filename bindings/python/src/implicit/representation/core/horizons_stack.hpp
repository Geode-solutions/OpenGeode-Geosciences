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

#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

#include <geode/geometry/point.hpp>

#define PYTHON_HORIZONS_STACK( dimension )                                     \
    const auto name##dimension =                                               \
        "HorizonsStack" + std::to_string( dimension ) + "D";                   \
    pybind11::class_< HorizonsStack##dimension##D, StratigraphicRelationships, \
        Horizons##dimension##D, StratigraphicUnits##dimension##D,              \
        Identifier >( module, name##dimension.c_str() )                        \
        .def( pybind11::init<>() )                                             \
        .def( "native_extension",                                              \
            &HorizonsStack##dimension##D::native_extension )                   \
        .def( "top_horizon", &HorizonsStack##dimension##D::top_horizon )       \
        .def( "bottom_horizon", &HorizonsStack##dimension##D::bottom_horizon ) \
        .def( "bottom_to_top_horizons",                                        \
            &HorizonsStack##dimension##D::bottom_to_top_horizons )             \
        .def( "bottom_to_top_units",                                           \
            &HorizonsStack##dimension##D::bottom_to_top_units )                \
        .def( "top_to_bottom_horizons",                                        \
            &HorizonsStack##dimension##D::top_to_bottom_horizons )             \
        .def( "top_to_bottom_units",                                           \
            &HorizonsStack##dimension##D::top_to_bottom_units )

namespace geode
{
    void define_horizons_stack( pybind11::module& module )
    {
        PYTHON_HORIZONS_STACK( 2 );
        PYTHON_HORIZONS_STACK( 3 );
    }
} // namespace geode