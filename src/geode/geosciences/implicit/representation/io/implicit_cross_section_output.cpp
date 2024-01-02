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

#include <geode/geosciences/implicit/representation/io/implicit_cross_section_output.h>

#include <string>
#include <vector>

#include <absl/strings/string_view.h>

#include <geode/basic/detail/geode_output_impl.h>
#include <geode/basic/io.h>
#include <geode/basic/logger.h>

#include <geode/model/representation/io/section_output.h>

#include <geode/geosciences/explicit/representation/io/cross_section_output.h>
#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>

namespace geode
{
    std::vector< std::string > save_implicit_cross_section(
        const ImplicitCrossSection& section, absl::string_view filename )
    {
        constexpr auto TYPE = "ImplicitCrossSection";
        try
        {
            return detail::geode_object_output_impl<
                ImplicitCrossSectionOutputFactory >( TYPE, section, filename );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            print_available_extensions< ImplicitCrossSectionOutputFactory >(
                TYPE );
            Logger::info( "Other extensions are available in parent classes." );
            print_available_extensions< CrossSectionOutputFactory >(
                "CrossSection" );
            print_available_extensions< SectionOutputFactory >( "Section" );
            throw OpenGeodeException{
                "Cannot save ImplicitCrossSection in file: ", filename
            };
        }
    }

    bool is_implicit_cross_section_saveable(
        const ImplicitCrossSection& section, absl::string_view filename )
    {
        const auto output = detail::geode_object_output_writer<
            ImplicitCrossSectionOutputFactory >( filename );
        return output->is_saveable( section );
    }
} // namespace geode
