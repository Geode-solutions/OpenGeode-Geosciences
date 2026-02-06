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
#include <string_view>
#include <vector>

#include <geode/basic/factory.hpp>
#include <geode/basic/output.hpp>

#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    class StratigraphicModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a StratigraphicModel.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] stratigraphic_model StratigraphicModel to save.
     * @param[in] filename Path to the file where save the
     * StratigraphicModel.
     */
    std::vector< std::string > opengeode_geosciences_implicit_api
        save_stratigraphic_model( const StratigraphicModel& stratigraphic_model,
            std::string_view filename );

    class opengeode_geosciences_implicit_api StratigraphicModelOutput
        : public Output< StratigraphicModel >
    {
    public:
        virtual ~StratigraphicModelOutput() = default;

    protected:
        explicit StratigraphicModelOutput( std::string_view filename )
            : Output< StratigraphicModel >{ filename }
        {
        }
    };

    [[nodiscard]] bool opengeode_geosciences_implicit_api
        is_stratigraphic_model_saveable(
            const StratigraphicModel& stratigraphic_model,
            std::string_view filename );

    using StratigraphicModelOutputFactory =
        Factory< std::string, StratigraphicModelOutput, std::string_view >;
} // namespace geode
