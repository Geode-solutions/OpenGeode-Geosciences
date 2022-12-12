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

#include <geode/geosciences/representation/io/cross_section_input.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/identifier_builder.h>
#include <geode/basic/timer.h>

#include <geode/geosciences/representation/builder/cross_section_builder.h>
#include <geode/geosciences/representation/core/cross_section.h>

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
    CrossSection load_cross_section( absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION(
                CrossSectionInputFactory::has_creator( extension ),
                "Unknown extension: ", extension );
            auto input =
                CrossSectionInputFactory::create( extension, filename );
            auto cross_section = input->read();
            if( cross_section.name() == Identifier::DEFAULT_NAME )
            {
                IdentifierBuilder{ cross_section }.set_name(
                    filename_without_extension( filename ) );
            }
            Logger::info( "CrossSection loaded from ", filename, " in ",
                timer.duration() );
            std::string message{ "CrossSection has: " };
            add_to_message(
                message, cross_section.nb_surfaces(), " Surfaces, " );
            add_to_message( message, cross_section.nb_lines(), " Lines, " );
            add_to_message( message, cross_section.nb_corners(), " Corners, " );
            add_to_message( message, cross_section.nb_model_boundaries(),
                " ModelBoundaries, " );
            add_to_message( message, cross_section.nb_faults(), " Faults, " );
            add_to_message(
                message, cross_section.nb_horizons(), " Horizons, " );
            add_to_message(
                message, cross_section.nb_fault_blocks(), " FaultBlocks, " );
            add_to_message( message, cross_section.nb_stratigraphic_units(),
                " StratigraphicUnits" );
            Logger::info( message );
            return cross_section;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load CrossSection from file: ",
                filename };
        }
    }
} // namespace geode
