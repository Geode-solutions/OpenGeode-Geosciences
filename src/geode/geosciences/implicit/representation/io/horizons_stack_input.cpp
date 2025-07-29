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

#include <geode/geosciences/implicit/representation/io/horizons_stack_input.hpp>

#include <string_view>

#include <absl/strings/str_cat.h>

#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

namespace geode
{
    template < index_t dimension >
    HorizonsStack< dimension > load_horizons_stack( std::string_view filename )
    {
        constexpr auto TYPE = "HorizonsStack";
        try
        {
            auto horizons_stack = detail::geode_object_input_impl<
                HorizonsStackInputFactory< dimension > >( TYPE, filename );
            auto message = absl::StrCat( TYPE, " has: " );
            detail::add_to_message(
                message, horizons_stack.nb_horizons(), " Horizons, " );
            detail::add_to_message( message,
                horizons_stack.nb_stratigraphic_units(),
                " Stratigraphic Units" );
            Logger::info( message );
            return horizons_stack;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions<
                HorizonsStackInputFactory< dimension > >( TYPE );
            throw OpenGeodeException{ "Cannot load HorizonsStack from file: ",
                filename };
        }
    }

    template < index_t dimension >
    typename HorizonsStackInput< dimension >::AdditionalFiles
        horizons_stack_additional_files( std::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            HorizonsStackInputFactory< dimension > >( filename );
        return input->additional_files();
    }

    template < index_t dimension >
    bool is_horizons_stack_loadable( std::string_view filename )
    {
        try
        {
            const auto input = detail::geode_object_input_reader<
                HorizonsStackInputFactory< dimension > >( filename );
            return input->is_loadable();
        }
        catch( ... )
        {
            return false;
        }
    }

    template HorizonsStack< 2 > opengeode_geosciences_implicit_api
        load_horizons_stack( std::string_view );
    template HorizonsStack< 3 > opengeode_geosciences_implicit_api
        load_horizons_stack( std::string_view );

    template HorizonsStackInput< 2 >::AdditionalFiles
        opengeode_geosciences_implicit_api horizons_stack_additional_files< 2 >(
            std::string_view );
    template HorizonsStackInput< 3 >::AdditionalFiles
        opengeode_geosciences_implicit_api horizons_stack_additional_files< 3 >(
            std::string_view );

    template bool opengeode_geosciences_implicit_api
        is_horizons_stack_loadable< 2 >( std::string_view );
    template bool opengeode_geosciences_implicit_api
        is_horizons_stack_loadable< 3 >( std::string_view );
} // namespace geode
