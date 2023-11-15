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

#include <geode/geosciences/implicit/representation/io/horizons_stack_output.h>

#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_output_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

namespace geode
{
    template < index_t dimension >
    void save_horizons_stack( const HorizonsStack< dimension >& horizons_stack,
        absl::string_view filename )
    {
        constexpr auto TYPE = "HorizonsStack";
        try
        {
            detail::geode_object_output_impl<
                HorizonsStackOutputFactory< dimension > >(
                TYPE, horizons_stack, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions<
                HorizonsStackOutputFactory< dimension > >( TYPE );
            throw OpenGeodeException{ "Cannot save HorizonsStack in file: ",
                filename };
        }
    }

    template < index_t dimension >
    bool is_horizons_stack_saveable(
        const HorizonsStack< dimension >& horizons_stack,
        absl::string_view filename )
    {
        const auto output = detail::geode_object_output_writer<
            HorizonsStackOutputFactory< dimension > >( filename );
        return output->is_saveable( horizons_stack );
    }

    template void opengeode_geosciences_implicit_api save_horizons_stack(
        const HorizonsStack< 2 >&, absl::string_view );
    template void opengeode_geosciences_implicit_api save_horizons_stack(
        const HorizonsStack< 3 >&, absl::string_view );

    template bool opengeode_geosciences_implicit_api is_horizons_stack_saveable(
        const HorizonsStack< 2 >&, absl::string_view );
    template bool opengeode_geosciences_implicit_api is_horizons_stack_saveable(
        const HorizonsStack< 3 >&, absl::string_view );
} // namespace geode
