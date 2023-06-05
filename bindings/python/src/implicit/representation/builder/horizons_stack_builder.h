/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.h>

#include <geode/geometry/point.h>

#define PYTHON_HORIZONS_STACK_BUILDER( dimension )                             \
    const auto name##dimension =                                               \
        "HorizonsStackBuilder" + std::to_string( dimension ) + "D";            \
    pybind11::class_< HorizonsStackBuilder##dimension##D,                      \
        StratigraphicRelationshipsBuilder, HorizonsBuilder##dimension##D,      \
        StratigraphicUnitsBuilder##dimension##D, IdentifierBuilder >(          \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< HorizonsStack##dimension##D& >() )               \
        .def( "copy",                                                          \
            []( HorizonsStackBuilder##dimension##D& builder,                   \
                const HorizonsStack##dimension##D& other_stack ) {             \
                builder.copy( other_stack );                                   \
            } )                                                                \
        .def( "copy_components",                                               \
            []( HorizonsStackBuilder##dimension##D& builder,                   \
                const HorizonsStack##dimension##D& other_stack ) {             \
                builder.copy_components( other_stack );                        \
            } )                                                                \
        .def( "add_horizon",                                                   \
            ( const uuid& (HorizonsStackBuilder##dimension##D::*) () )         \
                & HorizonsStackBuilder##dimension##D::add_horizon )            \
        .def( "add_stratigraphic_unit",                                        \
            ( const uuid& (HorizonsStackBuilder##dimension##D::*) () )         \
                & HorizonsStackBuilder##dimension##D::add_stratigraphic_unit ) \
        .def( "add_horizon_above",                                             \
            &HorizonsStackBuilder##dimension##D::add_horizon_above )           \
        .def( "add_horizon_under",                                             \
            &HorizonsStackBuilder##dimension##D::add_horizon_under )           \
        .def( "remove_horizon",                                                \
            &HorizonsStackBuilder##dimension##D::remove_horizon )              \
        .def( "remove_stratigraphic_unit",                                     \
            &HorizonsStackBuilder##dimension##D::remove_stratigraphic_unit )

namespace geode
{
    void define_horizons_stack_builder( pybind11::module& module )
    {
        PYTHON_HORIZONS_STACK_BUILDER( 2 );
        PYTHON_HORIZONS_STACK_BUILDER( 3 );
    }
} // namespace geode