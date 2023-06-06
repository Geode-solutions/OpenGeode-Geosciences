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

#include <geode/geosciences/implicit/representation/io/horizons_stack_input.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/identifier_builder.h>
#include <geode/basic/timer.h>

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.h>
#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

namespace geode
{
    template < index_t dimension >
    HorizonsStack< dimension > load_horizons_stack( absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            OPENGEODE_EXCEPTION(
                HorizonsStackInputFactory< dimension >::has_creator(
                    extension ),
                "Unknown extension: ", extension );
            auto input = HorizonsStackInputFactory< dimension >::create(
                extension, filename );
            auto horizons_stack = input->read();
            if( horizons_stack.name() == Identifier::DEFAULT_NAME )
            {
                IdentifierBuilder{ horizons_stack }.set_name(
                    filename_without_extension( filename ) );
            }
            Logger::info( "HorizonsStack loaded from ", filename, " in ",
                timer.duration() );
            Logger::info( "HorizonsStack has: ", horizons_stack.nb_horizons(),
                " Horizons and ", horizons_stack.nb_stratigraphic_units(),
                " Stratigraphic Units" );
            return horizons_stack;
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "Cannot load HorizonsStack from file: ",
                filename };
        }
    }

    template HorizonsStack< 2 > opengeode_geosciences_implicit_api
        load_horizons_stack( absl::string_view );
    template HorizonsStack< 3 > opengeode_geosciences_implicit_api
        load_horizons_stack( absl::string_view );
} // namespace geode
