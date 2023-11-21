/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <absl/strings/string_view.h>

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitStructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an ImplicitStructuralModel.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    ImplicitStructuralModel opengeode_geosciences_implicit_api
        load_implicit_structural_model( absl::string_view filename );

    class ImplicitStructuralModelInput : public Input< ImplicitStructuralModel >
    {
    public:
        using Base = Input< ImplicitStructuralModel >;
        using typename Base::InputData;
        using typename Base::MissingFiles;

    protected:
        explicit ImplicitStructuralModelInput( absl::string_view filename )
            : Base{ filename }
        {
        }
    };

    ImplicitStructuralModelInput::MissingFiles
        opengeode_geosciences_implicit_api
        check_implicit_structural_model_missing_files(
            absl::string_view filename );

    bool opengeode_geosciences_implicit_api
        is_implicit_structural_model_loadable( absl::string_view filename );

    using ImplicitStructuralModelInputFactory =
        Factory< std::string, ImplicitStructuralModelInput, absl::string_view >;
} // namespace geode
