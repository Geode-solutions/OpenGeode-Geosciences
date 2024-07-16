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

#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_cross_section_output.hpp>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <async++.h>

#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/model/representation/io/geode/geode_section_output.hpp>

#include <geode/geosciences/explicit/representation/io/geode/geode_cross_section_output.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/io/horizons_stack_output.hpp>

namespace geode
{
    void OpenGeodeImplicitCrossSectionOutput::archive_implicit_section_files(
        const ZipFile& zip_writer ) const
    {
        for( const auto& file :
            std::filesystem::directory_iterator( zip_writer.directory() ) )
        {
            zip_writer.archive_file( file.path().string() );
        }
    }

    void OpenGeodeImplicitCrossSectionOutput::save_implicit_section_files(
        const ImplicitCrossSection& implicit_section,
        std::string_view directory ) const
    {
        async::parallel_invoke(
            [&directory, &implicit_section] {
                OpenGeodeCrossSectionOutput cross_section_output{ "" };
                cross_section_output.save_cross_section_files(
                    implicit_section, directory );
            },
            [&directory, &implicit_section] {
                save_horizons_stack( implicit_section.horizons_stack(),
                    absl::StrCat( directory, "/horizons_stack.",
                        HorizonsStack2D::native_extension_static() ) );
            },
            [&directory, &implicit_section] {
                const auto filename = absl::StrCat(
                    directory, "/implicit_section_impl.og_ixsctn" );
                std::ofstream file{ filename, std::ofstream::binary };
                TContext context{};
                BitseryExtensions::register_serialize_pcontext(
                    std::get< 0 >( context ) );
                Serializer archive{ context, file };
                archive.object( implicit_section );
                archive.adapter().flush();
                OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                    "[OpenGeodeImplicitCrossSectionOutput::save_section_impl] "
                    "Error while writing file: ",
                    filename );
            } );
    }

    std::vector< std::string > OpenGeodeImplicitCrossSectionOutput::write(
        const ImplicitCrossSection& implicit_section ) const
    {
        const ZipFile zip_writer{ this->filename(), uuid{}.string() };
        save_implicit_section_files( implicit_section, zip_writer.directory() );
        archive_implicit_section_files( zip_writer );
        return { to_string( this->filename() ) };
    }
} // namespace geode
