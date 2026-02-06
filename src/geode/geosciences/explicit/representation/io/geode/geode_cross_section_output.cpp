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

#include <geode/geosciences/explicit/representation/io/geode/geode_cross_section_output.hpp>

#include <string>
#include <vector>

#include <async++.h>

#include <fstream>

#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/model/representation/io/geode/geode_section_output.hpp>

namespace geode
{
    OpenGeodeCrossSectionOutput::OpenGeodeCrossSectionOutput(
        std::string_view filename )
        : CrossSectionOutput( filename )
    {
    }

    void OpenGeodeCrossSectionOutput::save_cross_section_files(
        const CrossSection& cross_section, std::string_view directory ) const
    {
        async::parallel_invoke(
            [&directory, &cross_section] {
                OpenGeodeSectionOutput section_output{ "" };
                section_output.save_section_files( cross_section, directory );
            },
            [&directory, &cross_section] {
                cross_section.save_faults( directory );
            },
            [&directory, &cross_section] {
                cross_section.save_horizons( directory );
            },
            [&directory, &cross_section] {
                cross_section.save_fault_blocks( directory );
            },
            [&directory, &cross_section] {
                cross_section.save_stratigraphic_units( directory );
            } );
    }

    void OpenGeodeCrossSectionOutput::archive_cross_section_files(
        const ZipFile& zip_writer ) const
    {
        OpenGeodeSectionOutput section_output{ "" };
        section_output.archive_section_files( zip_writer );
    }

    std::vector< std::string > OpenGeodeCrossSectionOutput::write(
        const CrossSection& cross_section ) const
    {
        const ZipFile zip_writer{ filename(), uuid{}.string() };
        save_cross_section_files(
            cross_section, to_string( zip_writer.directory() ) );
        archive_cross_section_files( zip_writer );
        return { to_string( filename() ) };
    }
} // namespace geode
