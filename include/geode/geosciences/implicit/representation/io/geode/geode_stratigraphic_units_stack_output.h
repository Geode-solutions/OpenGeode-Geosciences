/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
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
