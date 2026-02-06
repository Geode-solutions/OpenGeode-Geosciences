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

#include <string_view>

#include <geode/basic/factory.hpp>
#include <geode/basic/input.hpp>

#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStack );
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStackBuilder );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a HorizonsStack.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     * @return Loaded HorizonsStack.
     */
    template < index_t dimension >
    [[nodiscard]] HorizonsStack< dimension > load_horizons_stack(
        std::string_view filename );

    template < index_t dimension >
    class HorizonsStackInput : public Input< HorizonsStack< dimension > >
    {
    protected:
        explicit HorizonsStackInput( std::string_view filename )
            : Input< HorizonsStack< dimension > >{ filename }
        {
        }
    };

    template < index_t dimension >
    [[nodiscard]] AdditionalFiles horizons_stack_additional_files(
        std::string_view filename );

    template < index_t dimension >
    [[nodiscard]] Percentage is_horizons_stack_loadable(
        std::string_view filename );

    template < index_t dimension >
    [[nodiscard]] index_t horizons_stack_object_priority(
        std::string_view filename );

    template < index_t dimension >
    using HorizonsStackInputFactory = Factory< std::string,
        HorizonsStackInput< dimension >,
        std::string_view >;
    ALIAS_2D_AND_3D( HorizonsStackInputFactory );
} // namespace geode
