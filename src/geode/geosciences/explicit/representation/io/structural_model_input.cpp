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

#include <geode/geosciences/explicit/representation/io/structural_model_input.h>

#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/model/representation/io/brep_input.h>

#include <geode/geosciences/explicit/representation/core/structural_model.h>

namespace geode
{
    StructuralModel load_structural_model( absl::string_view filename )
    {
        constexpr auto TYPE = "StructuralModel";
        try
        {
            auto structural_model =
                detail::geode_object_input_impl< StructuralModelInputFactory >(
                    TYPE, filename );
            auto message = absl::StrCat( TYPE, " has: " );
            detail::add_to_message(
                message, structural_model.nb_blocks(), " Blocks, " );
            detail::add_to_message(
                message, structural_model.nb_surfaces(), " Surfaces, " );
            detail::add_to_message(
                message, structural_model.nb_lines(), " Lines, " );
            detail::add_to_message(
                message, structural_model.nb_corners(), " Corners, " );
            detail::add_to_message( message,
                structural_model.nb_model_boundaries(), " ModelBoundaries, " );
            detail::add_to_message(
                message, structural_model.nb_faults(), " Faults, " );
            detail::add_to_message(
                message, structural_model.nb_horizons(), " Horizons, " );
            detail::add_to_message(
                message, structural_model.nb_fault_blocks(), " FaultBlocks, " );
            detail::add_to_message( message,
                structural_model.nb_stratigraphic_units(),
                " StratigraphicUnits" );
            Logger::info( message );
            return structural_model;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< StructuralModelInputFactory >( TYPE );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< BRepInputFactory >( "BRep" );
            throw OpenGeodeException{ "Cannot load StructuralModel from file: ",
                filename };
        }
    }

    typename StructuralModelInput::MissingFiles
        check_structural_model_missing_files( absl::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< StructuralModelInputFactory >(
                filename );
        return input->check_missing_files();
    }

    bool is_structural_model_loadable( absl::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< StructuralModelInputFactory >(
                filename );
        return input->is_loadable();
    }
} // namespace geode
