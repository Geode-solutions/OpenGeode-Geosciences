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

#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_input.hpp>

#include <async++.h>

#include <geode/basic/detail/input_files.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/detail/filter.hpp>
#include <geode/model/representation/io/geode/geode_brep_input.hpp>

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>

namespace geode
{
    OpenGeodeStructuralModelInput::OpenGeodeStructuralModelInput(
        std::string_view filename )
        : StructuralModelInput{ filename }
    {
    }

    std::vector< StructuralModel > OpenGeodeStructuralModelInput::read()
    {
        const detail::ArchiveInputFiles files{ filename() };
        StructuralModel structural_model{ BITSERY::constructor };
        detail::load_structural_model_files( structural_model, files );
        std::vector< StructuralModel > results;
        results.push_back( std::move( structural_model ) );
        return results;
    }

    namespace detail
    {
        void load_structural_model_files(
            StructuralModel& structural_model, const InputFiles& files )
        {
            StructuralModelBuilder builder{ structural_model };
            async::parallel_invoke(
                [&builder, &files] {
                    builder.load_faults( files );
                },
                [&builder, &files] {
                    builder.load_horizons( files );
                },
                [&builder, &files] {
                    builder.load_fault_blocks( files );
                },
                [&builder, &files] {
                    builder.load_stratigraphic_units( files );
                } );
            load_brep_files( structural_model, files );
        }
    } // namespace detail
} // namespace geode
