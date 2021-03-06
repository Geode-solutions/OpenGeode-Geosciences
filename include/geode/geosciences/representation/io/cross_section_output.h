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

#pragma once

#include <geode/basic/factory.h>

#include <geode/model/representation/io/section_output.h>

#include <geode/geosciences/common.h>

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
    void opengeode_geosciences_geosciences_api save_cross_section(
        const CrossSection& cross_section, absl::string_view filename );

    class opengeode_geosciences_geosciences_api CrossSectionOutput
        : public Output
    {
    public:
        virtual ~CrossSectionOutput() = default;

    protected:
        CrossSectionOutput(
            const CrossSection& cross_section, absl::string_view filename );

        const CrossSection& cross_section() const
        {
            return cross_section_;
        }

    private:
        const CrossSection& cross_section_;
    };

    using CrossSectionOutputFactory = Factory< std::string,
        CrossSectionOutput,
        const CrossSection&,
        absl::string_view >;
} // namespace geode
