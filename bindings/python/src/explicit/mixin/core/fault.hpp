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

#include <pybind11/native_enum.h>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>

#define PYTHON_FAULT( dimension )                                              \
    const auto name##dimension = "Fault" + std::to_string( dimension ) + "D";  \
    pybind11::class_< Fault##dimension##D, Component##dimension##D,            \
        pybind11::smart_holder >                                               \
        fault##dimension##D( module, name##dimension.c_str(),                  \
            "Geological component describing a Fault, described by a "         \
            "FaultType (no_type until it is set)" );                           \
                                                                               \
    pybind11::native_enum< Fault##dimension##D::FAULT_TYPE >(                  \
        fault##dimension##D, "FaultType", "enum.Enum",                         \
        "Kind of a Fault; no_type is the default, when none is defined" )      \
        .value( "no_type", Fault##dimension##D::FAULT_TYPE::no_type )          \
        .value( "normal", Fault##dimension##D::FAULT_TYPE::normal )            \
        .value( "reverse", Fault##dimension##D::FAULT_TYPE::reverse )          \
        .value( "strike_slip", Fault##dimension##D::FAULT_TYPE::strike_slip )  \
        .value( "listric", Fault##dimension##D::FAULT_TYPE::listric )          \
        .value( "decollement", Fault##dimension##D::FAULT_TYPE::decollement )  \
        .finalize();                                                           \
                                                                               \
    fault##dimension##D                                                        \
        .def( "has_type", &Fault##dimension##D::has_type,                      \
            "Return True if the type of the Fault is defined (not no_type)" )  \
        .def( "type", &Fault##dimension##D::type,                              \
            "Return the FaultType of the Fault, no_type if it is not "         \
            "defined" )                                                        \
        .def( "component_id", &Fault##dimension##D::component_id,              \
            "Return the ComponentID of the Fault (its type and its uuid)" )

namespace geode
{
    void define_fault( pybind11::module& module )
    {
        PYTHON_FAULT( 2 );
        PYTHON_FAULT( 3 );
    }
} // namespace geode
