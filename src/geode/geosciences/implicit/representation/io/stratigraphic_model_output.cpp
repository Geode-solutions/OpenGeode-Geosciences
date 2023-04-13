/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/io/stratigraphic_model_output.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/model/representation/io/brep_output.h>

#include <geode/geosciences/explicit/representation/io/structural_model_output.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_output.h>

namespace geode
{
    void save_stratigraphic_model(
        const StratigraphicModel& stratigraphic_model,
        absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            if( StratigraphicModelOutputFactory::has_creator( extension ) )
            {
                StratigraphicModelOutputFactory::create( extension, filename )
                    ->write( stratigraphic_model );
            }
            else if( ImplicitStructuralModelOutputFactory::has_creator(
                         extension ) )
            {
                ImplicitStructuralModelOutputFactory::create(
                    extension, filename )
                    ->write( stratigraphic_model );
            }
            else if( StructuralModelOutputFactory::has_creator( extension ) )
            {
                StructuralModelOutputFactory::create( extension, filename )
                    ->write( stratigraphic_model );
            }
            else if( BRepOutputFactory::has_creator( extension ) )
            {
                BRepOutputFactory::create( extension, filename )
                    ->write( stratigraphic_model );
            }
            else
            {
                throw OpenGeodeException{ "Unknown extension: ", extension };
            }
            Logger::info( "StratigraphicModel saved in ", filename, " in ",
                timer.duration() );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot save StratigraphicModel in file: ", filename
            };
        }
    }
} // namespace geode
