/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <fstream>

#include <geode/basic/uuid.h>

#include <geode/model/representation/io/detail/geode_section_output.h>

#include <geode/geosciences/representation/core/cross_section.h>
#include <geode/geosciences/representation/io/cross_section_output.h>

namespace geode
{
    class opengeode_geosciences_geosciences_api OpenGeodeCrossSectionOutput
        final : public CrossSectionOutput
    {
    public:
        OpenGeodeCrossSectionOutput(
            const CrossSection& cross_section, absl::string_view filename )
            : CrossSectionOutput( cross_section, filename )
        {
        }

        static absl::string_view extension()
        {
            return CrossSection::native_extension_static();
        }

        void save_cross_section_files( absl::string_view directory ) const
        {
            OpenGeodeSectionOutput section_output{ cross_section(),
                filename() };
            section_output.save_section_files( directory );
            cross_section().save_faults( directory );
            cross_section().save_horizons( directory );
            cross_section().save_fault_blocks( directory );
            cross_section().save_stratigraphic_units( directory );
        }

        void archive_cross_section_files( const ZipFile& zip_writer ) const
        {
            OpenGeodeSectionOutput section_output{ cross_section(),
                filename() };
            section_output.archive_section_files( zip_writer );
        }

        void write() const final
        {
            const ZipFile zip_writer{ filename(), uuid{}.string() };
            save_cross_section_files( zip_writer.directory().data() );
            archive_cross_section_files( zip_writer );
        }
    };
} // namespace geode
