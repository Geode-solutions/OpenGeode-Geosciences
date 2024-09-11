/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
#include <string_view>
#include <vector>

#include <geode/basic/factory.hpp>

#include <geode/model/representation/io/section_output.hpp>

#include <geode/geosciences/explicit/common.hpp>

namespace geode
{
    class CrossSection;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a CrossSection.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] cross_section CrossSection to save.
     * @param[in] filename Path to the file where save the CrossSection.
     */
    std::vector< std::string >
        opengeode_geosciences_explicit_api save_cross_section(
            const CrossSection& cross_section, std::string_view filename );

    class CrossSectionOutput : public Output< CrossSection >
    {
    public:
        virtual ~CrossSectionOutput() = default;

    protected:
        explicit CrossSectionOutput( std::string_view filename )
            : Output< CrossSection >{ filename }
        {
        }
    };

    [[nodiscard]] bool opengeode_geosciences_explicit_api
        is_cross_section_saveable(
            const CrossSection& cross_section, std::string_view filename );

    using CrossSectionOutputFactory =
        Factory< std::string, CrossSectionOutput, std::string_view >;
} // namespace geode
