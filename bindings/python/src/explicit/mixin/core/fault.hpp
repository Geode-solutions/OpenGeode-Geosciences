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

#include <geode/geosciences/explicit/mixin/core/fault.hpp>

#define PYTHON_FAULT( dimension )                                              \
    const auto name##dimension = "Fault" + std::to_string( dimension ) + "D";  \
    pybind11::class_< Fault##dimension##D, Component##dimension##D >           \
        fault##dimension##D( module, name##dimension.c_str() );                \
                                                                               \
    fault##dimension##D.def( "has_type", &Fault##dimension##D::has_type )      \
        .def( "type", &Fault##dimension##D::type )                             \
        .def( "component_id", &Fault##dimension##D::component_id );            \
                                                                               \
    pybind11::enum_< Fault##dimension##D::FAULT_TYPE >(                        \
        fault##dimension##D, "FAULT_TYPE" )                                    \
        .value( "NO_TYPE", Fault##dimension##D::FAULT_TYPE::no_type )          \
        .value( "NORMAL", Fault##dimension##D::FAULT_TYPE::normal )            \
        .value( "REVERSE", Fault##dimension##D::FAULT_TYPE::reverse )          \
        .value( "STRIKE_SLIP", Fault##dimension##D::FAULT_TYPE::strike_slip )  \
        .value( "LISTRIC", Fault##dimension##D::FAULT_TYPE::listric )          \
        .value( "DECOLLEMENT", Fault##dimension##D::FAULT_TYPE::decollement )

namespace geode
{
    void define_fault( pybind11::module& module )
    {
        PYTHON_FAULT( 2 );
        PYTHON_FAULT( 3 );
    }
} // namespace geode
