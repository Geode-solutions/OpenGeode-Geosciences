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

#include <geode/geosciences/implicit/representation/io/stratigraphic_section_output.hpp>

#include <string>
#include <string_view>
#include <vector>

#include <geode/basic/detail/geode_output_impl.hpp>
#include <geode/basic/io.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/representation/io/section_output.hpp>

#include <geode/geosciences/explicit/representation/io/cross_section_output.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.hpp>
#include <geode/geosciences/implicit/representation/io/implicit_cross_section_output.hpp>

namespace geode
{
    std::vector< std::string > save_stratigraphic_section(
        const StratigraphicSection& stratigraphic_section,
        std::string_view filename )
    {
        constexpr auto TYPE = "StratigraphicSection";
        try
        {
            return detail::geode_object_output_impl<
                StratigraphicSectionOutputFactory >(
                TYPE, stratigraphic_section, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< StratigraphicSectionOutputFactory >(
                TYPE );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< ImplicitCrossSectionOutputFactory >(
                "ImplicitCrossSection" );
            print_available_extensions< CrossSectionOutputFactory >(
                "CrossSection" );
            print_available_extensions< SectionOutputFactory >( "Section" );
            throw OpenGeodeException{
                "Cannot save StratigraphicSection in file: ", filename
            };
        }
    }

    bool is_stratigraphic_section_saveable(
        const StratigraphicSection& stratigraphic_section,
        std::string_view filename )
    {
        try
        {
            const auto output = detail::geode_object_output_writer<
                StratigraphicSectionOutputFactory >( filename );
            return output->is_saveable( stratigraphic_section );
        }
        catch( ... )
        {
            return false;
        }
    }
} // namespace geode
