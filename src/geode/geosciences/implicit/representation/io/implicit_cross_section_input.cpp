/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geosciences/implicit/representation/io/implicit_cross_section_input.h>

#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_input_impl.h>

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>

namespace geode
{
    ImplicitCrossSection load_implicit_cross_section(
        absl::string_view filename )
    {
        try
        {
            constexpr auto type = "ImplicitCrossSection";
            auto implicit_model = detail::geode_object_input_impl<
                ImplicitCrossSectionInputFactory >( type, filename );
            auto message = absl::StrCat( type, " has: " );
            detail::add_to_message(
                message, implicit_model.nb_surfaces(), " Surfaces, " );
            detail::add_to_message(
                message, implicit_model.nb_lines(), " Lines, " );
            detail::add_to_message(
                message, implicit_model.nb_corners(), " Corners, " );
            detail::add_to_message( message,
                implicit_model.nb_model_boundaries(), " ModelBoundaries, " );
            detail::add_to_message(
                message, implicit_model.nb_faults(), " Faults, " );
            detail::add_to_message(
                message, implicit_model.nb_horizons(), " Horizons, " );
            detail::add_to_message(
                message, implicit_model.nb_fault_blocks(), " FaultBlocks, " );
            detail::add_to_message( message,
                implicit_model.nb_stratigraphic_units(),
                " StratigraphicUnits" );
            Logger::info( message );
            return implicit_model;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot load ImplicitCrossSection from file: ", filename
            };
        }
    }

    typename ImplicitCrossSectionInput::MissingFiles
        check_implicit_cross_section_missing_files( absl::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            ImplicitCrossSectionInputFactory >( filename );
        return input->check_missing_files();
    }
} // namespace geode
