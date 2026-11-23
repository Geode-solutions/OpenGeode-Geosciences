/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_cross_section_input.hpp>

#include <absl/strings/str_cat.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/detail/geode_input_impl.hpp>
#include <geode/basic/detail/input_files.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/representation/io/geode/geode_section_input.hpp>

#include <geode/geosciences/explicit/representation/io/geode/geode_cross_section_input.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_cross_section.hpp>
#include <geode/geosciences/implicit/representation/io/horizons_stack_input.hpp>

namespace geode
{
    std::vector< ImplicitCrossSection >
        OpenGeodeImplicitCrossSectionInput::read()
    {
        const detail::ArchiveInputFiles files{ this->filename() };
        ImplicitCrossSection section{ BITSERY::constructor };
        detail::load_implicit_cross_section_files( section, files );
        static constexpr auto IMPL_FILENAME = "implicit_section_impl.og_ixsctn";
        OpenGeodeGeosciencesImplicitException::check_exception(
            files.has( IMPL_FILENAME ), nullptr, OpenGeodeException::TYPE::data,
            "[OpenGeodeImplicitCrossSectionInput::read] Error in reading "
            "files: Could not find stored impl." );
        const auto content = files.content( IMPL_FILENAME );
        OpenGeodeGeosciencesImplicitException::check_exception(
            detail::deserialize_bytes( content.span(), section ), nullptr,
            OpenGeodeException::TYPE::internal,
            "[OpenGeodeImplicitCrossSectionOutput::load_section_impl] "
            "Error while reading file: ",
            files.location( IMPL_FILENAME ) );
        std::vector< ImplicitCrossSection > results;
        results.push_back( std::move( section ) );
        return results;
    }

    namespace detail
    {
        void load_implicit_cross_section_files(
            ImplicitCrossSection& section, const InputFiles& files )
        {
            const ScopedLoggerLevel scoped_level{ Logger::LEVEL::warning };
            ImplicitCrossSectionBuilder builder{ section };
            const auto stack_file = absl::StrCat(
                "horizons_stack.", HorizonsStack2D::native_extension_static() );
            builder.set_horizons_stack( load_one(
                geode_object_input_from_files< HorizonsStackInputFactory2D >(
                    "HorizonsStack", files, stack_file ),
                files.location( stack_file ) ) );
            load_cross_section_files( section, files );
        }
    } // namespace detail
} // namespace geode
