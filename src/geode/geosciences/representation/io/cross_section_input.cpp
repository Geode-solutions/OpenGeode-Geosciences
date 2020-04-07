/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/geosciences/representation/core/cross_section.h>

namespace
{
    void add_to_message( std::string& message,
        geode::index_t nb_components,
        absl::string_view component_text )
    {
        if( nb_components > 0 )
        {
            message = absl::StrCat( message, nb_components, component_text );
        }
    }
} // namespace

namespace geode
{
    void load_cross_section(
        CrossSection& cross_section, absl::string_view filename )
    {
        try
        {
            auto input = CrossSectionInputFactory::create(
                extension_from_filename( filename ).data(), cross_section,
                filename );
            input->read();
            Logger::info( "CrossSection loaded from ", filename );
            std::string message{ "CrossSection has: " };
            add_to_message(
                message, cross_section.nb_surfaces(), " Surfaces, " );
            add_to_message( message, cross_section.nb_lines(), " Lines, " );
            add_to_message( message, cross_section.nb_faults(), " Faults, " );
            add_to_message(
                message, cross_section.nb_horizons(), " Horizons, " );
            add_to_message(
                message, cross_section.nb_fault_blocks(), " FaultBlocks, " );
            add_to_message( message, cross_section.nb_stratigraphic_units(),
                " StratigraphicUnits" );
            Logger::info( message );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "[load_cross_section] Cannot load CrossSection from file: ",
                filename
            };
        }
    }

    CrossSectionInput::CrossSectionInput(
        CrossSection& cross_section, absl::string_view filename )
        : Input{ filename }, cross_section_( cross_section )
    {
    }
} // namespace geode
