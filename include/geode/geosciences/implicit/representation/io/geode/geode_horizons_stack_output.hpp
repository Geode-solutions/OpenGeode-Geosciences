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

#include <filesystem>
#include <string>
#include <vector>

#include <async++.h>

#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/io/horizons_stack_output.hpp>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeHorizonsStackOutput final
        : public HorizonsStackOutput< dimension >
    {
    public:
        OpenGeodeHorizonsStackOutput( std::string_view filename )
            : HorizonsStackOutput< dimension >( filename )
        {
        }

        static std::string_view extension()
        {
            return HorizonsStack< dimension >::native_extension_static();
        }

        void archive_horizons_stack_files( const ZipFile& zip_writer ) const
        {
            for( const auto& file :
                std::filesystem::directory_iterator( zip_writer.directory() ) )
            {
                zip_writer.archive_file( file.path().string() );
            }
        }

        void save_horizons_stack_files(
            const HorizonsStack< dimension >& horizons_stack,
            std::string_view directory ) const
        {
            async::parallel_invoke(
                [&directory, &horizons_stack] {
                    horizons_stack.save_identifier( directory );
                },
                [&directory, &horizons_stack] {
                    horizons_stack.save_stratigraphic_relationships(
                        directory );
                },
                [&directory, &horizons_stack] {
                    horizons_stack.save_horizons( directory );
                    horizons_stack.save_stratigraphic_units( directory );
                } );
        }

        std::vector< std::string > write(
            const HorizonsStack< dimension >& horizons_stack ) const final
        {
            const ZipFile zip_writer{ this->filename(), uuid{}.string() };
            save_horizons_stack_files( horizons_stack, zip_writer.directory() );
            archive_horizons_stack_files( zip_writer );
            return { to_string( this->filename() ) };
        }
    };
    ALIAS_2D_AND_3D( OpenGeodeHorizonsStackOutput );
} // namespace geode
