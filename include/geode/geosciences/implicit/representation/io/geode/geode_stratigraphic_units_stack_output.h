/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <ghc/filesystem.hpp>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_output.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeStratigraphicUnitsStackOutput final
        : public StratigraphicUnitsStackOutput< dimension >
    {
    public:
        OpenGeodeStratigraphicUnitsStackOutput( absl::string_view filename )
            : StratigraphicUnitsStackOutput< dimension >( filename )
        {
        }

        static absl::string_view extension()
        {
            return StratigraphicUnitsStack<
                dimension >::native_extension_static();
        }

        void archive_stratigraphic_units_stack_files(
            const ZipFile& zip_writer ) const
        {
            for( const auto& file :
                ghc::filesystem::directory_iterator( zip_writer.directory() ) )
            {
                zip_writer.archive_file( file.path().string() );
            }
        }

        void save_stratigraphic_units_stack_files(
            const StratigraphicUnitsStack< dimension >&
                stratigraphic_units_stack,
            absl::string_view directory ) const
        {
            async::parallel_invoke(
                [&directory, &stratigraphic_units_stack] {
                    stratigraphic_units_stack.save_identifier( directory );
                },
                [&directory, &stratigraphic_units_stack] {
                    stratigraphic_units_stack.save_stratigraphic_relationships(
                        directory );
                },
                [&directory, &stratigraphic_units_stack] {
                    stratigraphic_units_stack.save_horizons( directory );
                    stratigraphic_units_stack.save_stratigraphic_units(
                        directory );
                } );
        }

        void write( const StratigraphicUnitsStack< dimension >&
                stratigraphic_units_stack ) const final
        {
            const ZipFile zip_writer{ this->filename(), uuid{}.string() };
            save_stratigraphic_units_stack_files(
                stratigraphic_units_stack, zip_writer.directory() );
            archive_stratigraphic_units_stack_files( zip_writer );
        }
    };
    ALIAS_2D_AND_3D( OpenGeodeStratigraphicUnitsStackOutput );
} // namespace geode
