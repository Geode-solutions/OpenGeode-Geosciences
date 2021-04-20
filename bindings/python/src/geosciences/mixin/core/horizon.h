/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/geosciences/mixin/core/horizon.h>

#define PYTHON_HORIZON( dimension )                                            \
    const auto name##dimension =                                               \
        "Horizon" + std::to_string( dimension ) + "D";                         \
    pybind11::class_< Horizon##dimension##D, Component##dimension##D >         \
        horizon##dimension##D( module, name##dimension.c_str() );              \
                                                                               \
    horizon##dimension##D.def( "has_type", &Horizon##dimension##D::has_type )  \
        .def( "type", &Horizon##dimension##D::type )                           \
        .def( "component_id", &Horizon##dimension##D::component_id );          \
                                                                               \
    pybind11::enum_< Horizon##dimension##D::HORIZON_TYPE >(                    \
        horizon##dimension##D, "HORIZON_TYPE" )                                \
        .value( "NO_TYPE", Horizon##dimension##D::HORIZON_TYPE::NO_TYPE )      \
        .value( "CONFORMAL", Horizon##dimension##D::HORIZON_TYPE::CONFORMAL )  \
        .value( "NON_CONFORMAL",                                               \
            Horizon##dimension##D::HORIZON_TYPE::NON_CONFORMAL )               \
        .value(                                                                \
            "TOPOGRAPHY", Horizon##dimension##D::HORIZON_TYPE::TOPOGRAPHY )    \
        .value( "INTRUSION", Horizon##dimension##D::HORIZON_TYPE::INTRUSION )

namespace geode
{
    void define_horizon( pybind11::module& module )
    {
        PYTHON_HORIZON( 2 );
        PYTHON_HORIZON( 3 );
    }
} // namespace geode
