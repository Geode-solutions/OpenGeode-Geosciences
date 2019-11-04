/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/model/representation/io/section_input.h>

#include <geode/geosciences/common.h>

namespace geode
{
    class CrossSection;
    class CrossSectionBuilder;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a CrossSection.
     * The adequate loader is called depending on the filename extension.
     * @param[out] cross_section Loaded CrossSection.
     * @param[in] filename Path to the file to load.
     */
    void opengeode_geosciences_geosciences_api load_cross_section(
        CrossSection& cross_section, const std::string& filename );

    class opengeode_geosciences_geosciences_api CrossSectionInput : public Input
    {
    public:
        virtual ~CrossSectionInput() = default;

    protected:
        CrossSectionInput( CrossSection& cross_section, std::string filename );

        CrossSection& cross_section()
        {
            return cross_section_;
        }

    private:
        CrossSection& cross_section_;
    };

    using CrossSectionInputFactory =
        Factory< std::string, CrossSectionInput, CrossSection&, std::string >;
} // namespace geode
