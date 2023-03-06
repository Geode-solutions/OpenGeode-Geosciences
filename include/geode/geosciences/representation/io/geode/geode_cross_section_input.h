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

#include <async++.h>

#include <fstream>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/representation/io/geode/geode_section_input.h>

#include <geode/geosciences/representation/builder/cross_section_builder.h>
#include <geode/geosciences/representation/core/cross_section.h>
#include <geode/geosciences/representation/io/cross_section_input.h>

namespace geode
{
    class opengeode_geosciences_geosciences_api OpenGeodeCrossSectionInput final
        : public CrossSectionInput
    {
    public:
        OpenGeodeCrossSectionInput( absl::string_view filename )
            : CrossSectionInput{ filename }
        {
        }

        static absl::string_view extension()
        {
            return CrossSection::native_extension_static();
        }

        void load_cross_section_files(
            CrossSection& cross_section, absl::string_view directory )
        {
            CrossSectionBuilder builder{ cross_section };
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

        CrossSection read() final
        {
            const UnzipFile zip_reader{ filename(), uuid{}.string() };
            zip_reader.extract_all();
            CrossSection cross_section;
            OpenGeodeSectionInput section_input{ filename() };
            section_input.load_section_files(
                cross_section, zip_reader.directory() );
            load_cross_section_files( cross_section, zip_reader.directory() );
            return cross_section;
        }
    };
} // namespace geode
