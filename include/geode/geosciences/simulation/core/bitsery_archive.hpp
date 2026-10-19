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

#include <string>

#include <geode/basic/bitsery_archive.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace geode
{
    /*!
     * Register a leaf class so that it can be saved through a pointer to its
     * base. The leaf type name is written in files: it must never change.
     * Libraries defining their own leaf classes call this from a function
     * given to BitseryExtensions::register_functions.
     */
    template < typename Serializer, typename Base, typename Leaf >
    void register_simulation_leaf( PContext& context )
    {
        context.registerSingleBaseBranch< Serializer, Base, Leaf >(
            std::string{ Leaf::type_name_static() }.c_str() );
    }

    void opengeode_geosciences_simulation_api
        register_simulation_serialize_pcontext( PContext& context );

    void opengeode_geosciences_simulation_api
        register_simulation_deserialize_pcontext( PContext& context );
} // namespace geode
