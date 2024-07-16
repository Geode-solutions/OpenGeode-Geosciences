/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/explicit/representation/io/cross_section_input.hpp>

#include <absl/strings/str_cat.h>
#include <string_view>

#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/representation/io/section_input.hpp>

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.hpp>
#include <geode/geosciences/explicit/representation/core/cross_section.hpp>

namespace geode
{
    CrossSection load_cross_section( std::string_view filename )
    {
        constexpr auto TYPE = "CrossSection";
        try
        {
            auto cross_section =
                detail::geode_object_input_impl< CrossSectionInputFactory >(
                    TYPE, filename );
            auto message = absl::StrCat( TYPE, " has: " );
            detail::add_to_message(
                message, cross_section.nb_surfaces(), " Surfaces, " );
            detail::add_to_message(
                message, cross_section.nb_lines(), " Lines, " );
            detail::add_to_message(
                message, cross_section.nb_corners(), " Corners, " );
            detail::add_to_message( message,
                cross_section.nb_model_boundaries(), " ModelBoundaries, " );
            detail::add_to_message(
                message, cross_section.nb_faults(), " Faults, " );
            detail::add_to_message(
                message, cross_section.nb_horizons(), " Horizons, " );
            detail::add_to_message(
                message, cross_section.nb_fault_blocks(), " FaultBlocks, " );
            detail::add_to_message( message,
                cross_section.nb_stratigraphic_units(), " StratigraphicUnits" );
            Logger::info( message );
            return cross_section;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< CrossSectionInputFactory >( TYPE );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< SectionInputFactory >( "Section" );
            throw OpenGeodeException{ "Cannot load CrossSection from file: ",
                filename };
        }
    }

    typename CrossSectionInput::MissingFiles check_cross_section_missing_files(
        std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< CrossSectionInputFactory >(
                filename );
        return input->check_missing_files();
    }

    bool is_cross_section_loadable( std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< CrossSectionInputFactory >(
                filename );
        return input->is_loadable();
    }
} // namespace geode
