/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geosciences/representation/io/structural_model_input.h>

#include <geode/geosciences/representation/core/structural_model.h>

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
    StructuralModel load_structural_model( absl::string_view filename )
    {
        try
        {
            StructuralModel structural_model;
            auto input = StructuralModelInputFactory::create(
                to_string( extension_from_filename( filename ) ),
                structural_model, filename );
            input->read();
            Logger::info( "StructuralModel loaded from ", filename );
            std::string message{ "StructuralModel has: " };
            add_to_message(
                message, structural_model.nb_blocks(), " Blocks, " );
            add_to_message(
                message, structural_model.nb_surfaces(), " Surfaces, " );
            add_to_message( message, structural_model.nb_lines(), " Lines, " );
            add_to_message(
                message, structural_model.nb_corners(), " Corners, " );
            add_to_message( message, structural_model.nb_model_boundaries(),
                " ModelBoundaries, " );
            add_to_message(
                message, structural_model.nb_faults(), " Faults, " );
            add_to_message(
                message, structural_model.nb_horizons(), " Horizons, " );
            add_to_message(
                message, structural_model.nb_fault_blocks(), " FaultBlocks, " );
            add_to_message( message, structural_model.nb_stratigraphic_units(),
                " StratigraphicUnits" );
            Logger::info( message );
            return structural_model;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load StructuralModel from file: ",
                filename };
        }
    }

    StructuralModelInput::StructuralModelInput(
        StructuralModel& structural_model, absl::string_view filename )
        : Input{ filename }, structural_model_( structural_model )
    {
    }
} // namespace geode
