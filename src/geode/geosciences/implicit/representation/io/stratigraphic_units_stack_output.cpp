/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_output.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>

namespace geode
{
    template < index_t dimension >
    void save_stratigraphic_units_stack(
        const StratigraphicUnitsStack< dimension >& stratigraphic_units_stack,
        absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION(
                StratigraphicUnitsStackOutputFactory< dimension >::has_creator(
                    extension ),
                "Unknown extension: ", extension );
            StratigraphicUnitsStackOutputFactory< dimension >::create(
                extension, filename )
                ->write( stratigraphic_units_stack );
            Logger::info( "StratigraphicUnitsStack saved in ", filename, " in ",
                timer.duration() );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot save StratigraphicUnitsStack in file: ", filename
            };
        }
    }

    template void opengeode_geosciences_implicit_api
        save_stratigraphic_units_stack(
            const StratigraphicUnitsStack< 2 >&, absl::string_view );
    template void opengeode_geosciences_implicit_api
        save_stratigraphic_units_stack(
            const StratigraphicUnitsStack< 3 >&, absl::string_view );
} // namespace geode
