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
#include <vector>

#include <absl/strings/string_view.h>

#include <geode/basic/factory.h>

#include <geode/model/representation/io/brep_output.h>

#include <geode/geosciences/explicit/common.h>

namespace geode
{
    class StructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a StructuralModel.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] structural_model StructuralModel to save.
     * @param[in] filename Path to the file where save the StructuralModel.
     */
    std::vector< std::string > opengeode_geosciences_explicit_api
        save_structural_model( const StructuralModel& structural_model,
            absl::string_view filename );

    class StructuralModelOutput : public Output< StructuralModel >
    {
    public:
        virtual ~StructuralModelOutput() = default;

    protected:
        StructuralModelOutput( absl::string_view filename )
            : Output< StructuralModel >{ filename }
        {
        }
    };

    bool opengeode_geosciences_explicit_api is_structural_model_saveable(
        const StructuralModel& structural_model, absl::string_view filename );

    using StructuralModelOutputFactory =
        Factory< std::string, StructuralModelOutput, absl::string_view >;
} // namespace geode
