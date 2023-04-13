/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/io/implicit_cross_section_output.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/model/representation/io/section_output.h>

#include <geode/geosciences/explicit/representation/io/cross_section_output.h>

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>

namespace geode
{
    void save_implicit_cross_section(
        const ImplicitCrossSection& section, absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            if( ImplicitCrossSectionOutputFactory::has_creator( extension ) )
            {
                ImplicitCrossSectionOutputFactory::create( extension, filename )
                    ->write( section );
            }
            else if( CrossSectionOutputFactory::has_creator( extension ) )
            {
                CrossSectionOutputFactory::create( extension, filename )
                    ->write( section );
            }
            else if( SectionOutputFactory::has_creator( extension ) )
            {
                SectionOutputFactory::create( extension, filename )
                    ->write( section );
            }
            else
            {
                throw OpenGeodeException{ "Unknown extension: ", extension };
            }
            Logger::info( "ImplicitCrossSection saved in ", filename, " in ",
                timer.duration() );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot save ImplicitCrossSection in file: ", filename
            };
        }
    }
} // namespace geode
