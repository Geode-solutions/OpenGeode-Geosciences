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
#include <vector>

#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_output.hpp>

namespace geode
{
    class ZipFile;
}

namespace geode
{
    class opengeode_geosciences_implicit_api
        OpenGeodeImplicitStructuralModelOutput final
        : public ImplicitStructuralModelOutput
    {
    public:
        explicit OpenGeodeImplicitStructuralModelOutput(
            std::string_view filename )
            : ImplicitStructuralModelOutput( filename )
        {
        }

        [[nodiscard]] static std::string_view extension()
        {
            return ImplicitStructuralModel::native_extension_static();
        }

        void archive_implicit_model_files( const ZipFile& zip_writer ) const;

        void save_implicit_model_files(
            const ImplicitStructuralModel& implicit_model,
            std::string_view directory ) const;

        std::vector< std::string > write(
            const ImplicitStructuralModel& implicit_model ) const final;
    };
} // namespace geode
