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

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

namespace geode
{
    template < index_t dimension >
    void save_horizons_stack(
        const HorizonsStack< dimension >& horizons_stack,
        absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION(
                HorizonsStackOutputFactory< dimension >::has_creator(
                    extension ),
                "Unknown extension: ", extension );
            HorizonsStackOutputFactory< dimension >::create(
                extension, filename )
                ->write( horizons_stack );
            Logger::info(
                "HorizonsStack saved in ", filename, " in ", timer.duration() );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot save HorizonsStack in file: ",
                filename };
        }
    }

    template void opengeode_geosciences_implicit_api
        save_horizons_stack(
            const HorizonsStack< 2 >&, absl::string_view );
    template void opengeode_geosciences_implicit_api
        save_horizons_stack(
            const HorizonsStack< 3 >&, absl::string_view );
} // namespace geode
