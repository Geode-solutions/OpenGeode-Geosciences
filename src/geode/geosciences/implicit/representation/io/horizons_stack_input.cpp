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

#include <geode/geosciences/implicit/representation/io/horizons_stack_input.h>

#include <absl/strings/str_cat.h>
#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_input_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.h>
#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

namespace geode
{
    template < index_t dimension >
    HorizonsStack< dimension > load_horizons_stack( absl::string_view filename )
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
    typename HorizonsStackInput< dimension >::MissingFiles
        check_horizons_stack_missing_files( absl::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            HorizonsStackInputFactory< dimension > >( filename );
        return input->check_missing_files();
    }

    template < index_t dimension >
    bool is_horizons_stack_loadable( absl::string_view filename )
    {
        const auto input = detail::geode_object_input_reader<
            HorizonsStackInputFactory< dimension > >( filename );
        return input->is_loadable();
    }

    template HorizonsStack< 2 > opengeode_geosciences_implicit_api
        load_horizons_stack( absl::string_view );
    template HorizonsStack< 3 > opengeode_geosciences_implicit_api
        load_horizons_stack( absl::string_view );

    template HorizonsStackInput< 2 >::MissingFiles
        opengeode_geosciences_implicit_api
            check_horizons_stack_missing_files< 2 >( absl::string_view );
    template HorizonsStackInput< 3 >::MissingFiles
        opengeode_geosciences_implicit_api
            check_horizons_stack_missing_files< 3 >( absl::string_view );

    template bool opengeode_geosciences_implicit_api
        is_horizons_stack_loadable< 2 >( absl::string_view );
    template bool opengeode_geosciences_implicit_api
        is_horizons_stack_loadable< 3 >( absl::string_view );
} // namespace geode
