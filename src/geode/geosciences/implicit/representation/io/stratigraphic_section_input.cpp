/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/io/stratigraphic_section_input.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

namespace
{
    void add_to_message( std::string& message,
        geode::index_t nb_components,
        absl::string_view component_text )
    {
        if( nb_components > 0 )
        {
            absl::StrAppend( &message, nb_components, component_text );
        }
    }
} // namespace

namespace geode
{
    StratigraphicSection load_stratigraphic_section(
        absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION(
                StratigraphicSectionInputFactory::has_creator( extension ),
                "Unknown extension: ", extension );
            auto stratigraphic_section =
                StratigraphicSectionInputFactory::create( extension, filename )
                    ->read();
            Logger::info( "StratigraphicSection loaded from ", filename, " in ",
                timer.duration() );
            std::string message{ "StratigraphicSection has: " };
            add_to_message(
                message, stratigraphic_section.nb_surfaces(), " Surfaces, " );
            add_to_message(
                message, stratigraphic_section.nb_lines(), " Lines, " );
            add_to_message(
                message, stratigraphic_section.nb_corners(), " Corners, " );
            add_to_message( message,
                stratigraphic_section.nb_model_boundaries(),
                " ModelBoundaries, " );
            add_to_message(
                message, stratigraphic_section.nb_faults(), " Faults, " );
            add_to_message(
                message, stratigraphic_section.nb_horizons(), " Horizons, " );
            add_to_message( message, stratigraphic_section.nb_fault_blocks(),
                " FaultBlocks, " );
            add_to_message( message,
                stratigraphic_section.nb_stratigraphic_units(),
                " StratigraphicUnits" );
            Logger::info( message );
            return stratigraphic_section;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot load StratigraphicSection from file: ", filename
            };
        }
    }
} // namespace geode
