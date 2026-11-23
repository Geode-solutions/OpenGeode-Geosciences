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

#include <geode/geosciences/explicit/representation/io/geode/geode_cross_section_input.hpp>

#include <async++.h>

#include <geode/basic/detail/input_files.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/builder/detail/filter.hpp>
#include <geode/model/representation/io/geode/geode_section_input.hpp>

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.hpp>

namespace geode
{
    OpenGeodeCrossSectionInput::OpenGeodeCrossSectionInput(
        std::string_view filename )
        : CrossSectionInput{ filename }
    {
    }

    std::vector< CrossSection > OpenGeodeCrossSectionInput::read()
    {
        const detail::ArchiveInputFiles files{ filename() };
        CrossSection cross_section{ BITSERY::constructor };
        detail::load_cross_section_files( cross_section, files );
        std::vector< CrossSection > results;
        results.push_back( std::move( cross_section ) );
        return results;
    }

    namespace detail
    {
        void load_cross_section_files(
            CrossSection& cross_section, const InputFiles& files )
        {
            CrossSectionBuilder builder{ cross_section };
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
            load_section_files( cross_section, files );
        }
    } // namespace detail
} // namespace geode
