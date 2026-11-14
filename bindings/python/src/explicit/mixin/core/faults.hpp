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

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/faults.hpp>

#define PYTHON_FAULTS( dimension )                                             \
    const auto name##dimension = "Faults" + std::to_string( dimension ) + "D"; \
    pybind11::class_< Faults##dimension##D, pybind11::smart_holder >( module,  \
        name##dimension.c_str(),                                               \
        "Storage of the Fault components of a model, read through the "        \
        "model class" )                                                        \
        .def( "nb_faults", &Faults##dimension##D::nb_faults,                   \
            "Return the number of Fault components of the model" )             \
        .def( "nb_active_faults", &Faults##dimension##D::nb_active_faults,     \
            "Return the number of active Fault components of the model" )      \
        .def( "has_fault", &Faults##dimension##D::has_fault,                   \
            pybind11::arg( "id" ),                                             \
            "Return True if the model has a Fault with the given uuid" )       \
        .def( "fault", &Faults##dimension##D::fault, pybind11::arg( "id" ),    \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the Fault with the given uuid; raise an error if the "     \
            "model has none" )                                                 \
        .def(                                                                  \
            "faults",                                                          \
            []( const Faults##dimension##D& self ) {                           \
                std::vector< const Fault##dimension##D* > components;          \
                for( const auto& component : self.faults() )                   \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the list of the Fault components of the model" )           \
        .def(                                                                  \
            "active_faults",                                                   \
            []( const Faults##dimension##D& self ) {                           \
                std::vector< const Fault##dimension##D* > components;          \
                for( const auto& component : self.active_faults() )            \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the list of the active Fault components of the "           \
            "model" )

namespace geode
{
    void define_faults( pybind11::module& module )
    {
        PYTHON_FAULTS( 2 );
        PYTHON_FAULTS( 3 );
    }
} // namespace geode
