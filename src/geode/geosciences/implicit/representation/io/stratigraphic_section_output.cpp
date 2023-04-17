/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/geosciences/implicit/representation/io/stratigraphic_section_output.h>

#include <absl/strings/ascii.h>

#include <geode/basic/filename.h>
#include <geode/basic/timer.h>

#include <geode/model/representation/io/section_output.h>

#include <geode/geosciences/explicit/representation/io/cross_section_output.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>
#include <geode/geosciences/implicit/representation/io/implicit_cross_section_output.h>

namespace geode
{
    void save_stratigraphic_section(
        const StratigraphicSection& stratigraphic_section,
        absl::string_view filename )
    {
        try
        {
            Timer timer;
            const auto extension =
                absl::AsciiStrToLower( extension_from_filename( filename ) );
            if( StratigraphicSectionOutputFactory::has_creator( extension ) )
            {
                StratigraphicSectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else if( ImplicitCrossSectionOutputFactory::has_creator(
                         extension ) )
            {
                ImplicitCrossSectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else if( CrossSectionOutputFactory::has_creator( extension ) )
            {
                CrossSectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else if( SectionOutputFactory::has_creator( extension ) )
            {
                SectionOutputFactory::create( extension, filename )
                    ->write( stratigraphic_section );
            }
            else
            {
                throw OpenGeodeException{ "Unknown extension: ", extension };
            }
            Logger::info( "StratigraphicSection saved in ", filename, " in ",
                timer.duration() );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{
                "Cannot save StratigraphicSection in file: ", filename
            };
        }
    }
} // namespace geode
