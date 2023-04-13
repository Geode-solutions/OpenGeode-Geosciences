/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/io/stratigraphic_section_output.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/model/representation/io/section_output.h>

#include <geode/geosciences/explicit/representation/io/cross_section_output.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>
#include <geode/geosciences/implicit/representation/io/implicit_cross_section_output.h>

namespace geode
{
    void save_stratigraphic_section(
        const StratigraphicSection& stratigraphic_section,
        absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            if( StratigraphicSectionOutputFactory::has_creator( extension ) )
            {
                StratigraphicSectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else if( ImplicitCrossSectionOutputFactory::has_creator(
                         extension ) )
            {
                ImplicitCrossSectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else if( CrossSectionOutputFactory::has_creator( extension ) )
            {
                CrossSectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else if( SectionOutputFactory::has_creator( extension ) )
            {
                SectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else
            {
                throw OpenGeodeException{ "Unknown extension: ", extension };
            }
            Logger::info( "StratigraphicSection saved in ", filename, " in ",
                timer.duration() );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot save StratigraphicSection in file: ", filename
            };
        }
    }
} // namespace geode
