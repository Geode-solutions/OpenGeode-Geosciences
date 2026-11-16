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

#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_blocks.hpp>

#define PYTHON_FAULT_BLOCKS( dimension )                                       \
    const auto name##dimension =                                               \
        "FaultBlocks" + std::to_string( dimension ) + "D";                     \
    pybind11::class_< FaultBlocks##dimension##D, pybind11::smart_holder >(     \
        module, name##dimension.c_str(),                                       \
        "Storage of the FaultBlock components of a model, read through the "   \
        "model class" )                                                        \
        .def( "nb_fault_blocks", &FaultBlocks##dimension##D::nb_fault_blocks,  \
            "Return the number of FaultBlock components of the model" )        \
        .def( "nb_active_fault_blocks",                                        \
            &FaultBlocks##dimension##D::nb_active_fault_blocks,                \
            "Return the number of active FaultBlock components of the model" ) \
        .def( "has_fault_block", &FaultBlocks##dimension##D::has_fault_block,  \
            pybind11::arg( "id" ),                                             \
            "Return True if the model has a FaultBlock with the given uuid" )  \
        .def( "fault_block", &FaultBlocks##dimension##D::fault_block,          \
            pybind11::arg( "id" ),                                             \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the FaultBlock with the given uuid; raise an error if "    \
            "the "                                                             \
            "model has none" )                                                 \
        .def(                                                                  \
            "fault_blocks",                                                    \
            []( const FaultBlocks##dimension##D& self ) {                      \
                std::vector< const FaultBlock##dimension##D* > components;     \
                for( const auto& component : self.fault_blocks() )             \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the list of the FaultBlock components of the model" )      \
        .def(                                                                  \
            "active_fault_blocks",                                             \
            []( const FaultBlocks##dimension##D& self ) {                      \
                std::vector< const FaultBlock##dimension##D* > components;     \
                for( const auto& component : self.active_fault_blocks() )      \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the list of the active FaultBlock components of the "      \
            "model" )

namespace geode
{
    void define_fault_blocks( pybind11::module& module )
    {
        PYTHON_FAULT_BLOCKS( 2 );
        PYTHON_FAULT_BLOCKS( 3 );
    }
} // namespace geode
