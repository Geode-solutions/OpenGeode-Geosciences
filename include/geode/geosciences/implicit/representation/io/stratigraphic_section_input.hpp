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

#pragma once

#include <string_view>

#include <geode/basic/factory.hpp>
#include <geode/basic/input.hpp>

#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    class StratigraphicSection;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a StratigraphicSection.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    [[nodiscard]] StratigraphicSection opengeode_geosciences_implicit_api
        load_stratigraphic_section( std::string_view filename );

    class StratigraphicSectionInput : public Input< StratigraphicSection >
    {
    protected:
        explicit StratigraphicSectionInput( std::string_view filename )
            : Input< StratigraphicSection >{ filename }
        {
        }
    };

    [[nodiscard]] StratigraphicSectionInput::AdditionalFiles
        opengeode_geosciences_implicit_api
        stratigraphic_section_additional_files( std::string_view filename );

    [[nodiscard]] Percentage opengeode_geosciences_implicit_api
        is_stratigraphic_section_loadable( std::string_view filename );

    [[nodiscard]] index_t opengeode_geosciences_implicit_api
        stratigraphic_section_object_priority( std::string_view filename );

    using StratigraphicSectionInputFactory =
        Factory< std::string, StratigraphicSectionInput, std::string_view >;
} // namespace geode
