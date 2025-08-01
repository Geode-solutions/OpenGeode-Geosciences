/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geosciences/implicit/representation/io/stratigraphic_model_input.hpp>

#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/representation/io/brep_input.hpp>

#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_input.hpp>

namespace geode
{
    StratigraphicModel load_stratigraphic_model( std::string_view filename )
    {
        constexpr auto TYPE = "StratigraphicModel";
        try
        {
            auto stratigraphic_model = detail::geode_object_input_impl<
                StratigraphicModelInputFactory >( TYPE, filename );
            auto message = absl::StrCat( TYPE, " has: " );
            detail::add_to_message(
                message, stratigraphic_model.nb_blocks(), " Blocks, " );
            detail::add_to_message(
                message, stratigraphic_model.nb_surfaces(), " Surfaces, " );
            detail::add_to_message(
                message, stratigraphic_model.nb_lines(), " Lines, " );
            detail::add_to_message(
                message, stratigraphic_model.nb_corners(), " Corners, " );
            detail::add_to_message( message,
                stratigraphic_model.nb_model_boundaries(),
                " ModelBoundaries, " );
            detail::add_to_message(
                message, stratigraphic_model.nb_faults(), " Faults, " );
            detail::add_to_message(
                message, stratigraphic_model.nb_horizons(), " Horizons, " );
            detail::add_to_message( message,
                stratigraphic_model.nb_fault_blocks(), " FaultBlocks, " );
            detail::add_to_message( message,
                stratigraphic_model.nb_stratigraphic_units(),
                " StratigraphicUnits" );
            Logger::info( message );
            return stratigraphic_model;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< StratigraphicModelInputFactory >(
                TYPE );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< ImplicitStructuralModelInputFactory >(
                "ImplicitStructuralModel" );
            print_available_extensions< StructuralModelInputFactory >(
                "StructuralModel" );
            print_available_extensions< BRepInputFactory >( "BRep" );
            throw OpenGeodeException{
                "Cannot load StratigraphicModel from file: ", filename
            };
        }
    }

    typename StratigraphicModelInput::AdditionalFiles
        stratigraphic_model_additional_files( std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< StratigraphicModelInputFactory >(
                filename );
        return input->additional_files();
    }

    Percentage is_stratigraphic_model_loadable( std::string_view filename )
    {
        try
        {
            const auto input = detail::geode_object_input_reader<
                StratigraphicModelInputFactory >( filename );
            return input->is_loadable();
        }
        catch( ... )
        {
            return Percentage{ 0 };
        }
    }

    index_t stratigraphic_model_object_priority( std::string_view filename )
    {
        const auto input =
            detail::geode_object_input_reader< StratigraphicModelInputFactory >(
                filename );
        return input->object_priority();
    }
} // namespace geode
