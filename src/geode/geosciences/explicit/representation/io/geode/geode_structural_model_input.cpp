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

#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_input.h>

#include <async++.h>

#include <fstream>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/filter.h>
#include <geode/model/representation/io/geode/geode_brep_input.h>

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.h>

namespace geode
{
    OpenGeodeStructuralModelInput::OpenGeodeStructuralModelInput(
        absl::string_view filename )
        : StructuralModelInput{ filename }
    {
    }

    void OpenGeodeStructuralModelInput::load_structural_model_files(
        StructuralModel& structural_model, absl::string_view directory )
    {
        StructuralModelBuilder builder{ structural_model };
        async::parallel_invoke(
            [&builder, &directory] {
                builder.load_faults( directory );
            },
            [&builder, &directory] {
                builder.load_horizons( directory );
            },
            [&builder, &directory] {
                builder.load_fault_blocks( directory );
            },
            [&builder, &directory] {
                builder.load_stratigraphic_units( directory );
            } );
    }

    StructuralModel OpenGeodeStructuralModelInput::read()
    {
        const UnzipFile zip_reader{ filename(), uuid{}.string() };
        zip_reader.extract_all();
        StructuralModel structural_model;
        OpenGeodeBRepInput brep_input{ filename() };
        brep_input.load_brep_files( structural_model, zip_reader.directory() );
        load_structural_model_files( structural_model, zip_reader.directory() );
        detail::filter_unsupported_components( structural_model );
        return structural_model;
    }
} // namespace geode
