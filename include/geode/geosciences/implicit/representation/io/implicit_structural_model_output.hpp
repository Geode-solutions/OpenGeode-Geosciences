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

#include <string>
#include <string_view>
#include <vector>

#include <geode/basic/factory.hpp>
#include <geode/basic/output.hpp>

#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    class ImplicitStructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving an ImplicitStructuralModel.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] implicit_model ImplicitStructuralModel to save.
     * @param[in] filename Path to the file where save the
     * ImplicitStructuralModel.
     */
    std::vector< std::string >
        opengeode_geosciences_implicit_api save_implicit_structural_model(
            const ImplicitStructuralModel& implicit_model,
            std::string_view filename );

    class opengeode_geosciences_implicit_api ImplicitStructuralModelOutput
        : public Output< ImplicitStructuralModel >
    {
    public:
        virtual ~ImplicitStructuralModelOutput() = default;

    protected:
        explicit ImplicitStructuralModelOutput( std::string_view filename )
            : Output< ImplicitStructuralModel >{ filename }
        {
        }
    };

    [[nodiscard]] bool opengeode_geosciences_implicit_api
        is_implicit_structural_model_saveable(
            const ImplicitStructuralModel& implicit_model,
            std::string_view filename );

    using ImplicitStructuralModelOutputFactory =
        Factory< std::string, ImplicitStructuralModelOutput, std::string_view >;
} // namespace geode
