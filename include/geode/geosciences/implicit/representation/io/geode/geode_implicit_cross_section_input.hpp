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

#pragma once

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.hpp>
#include <geode/geosciences/implicit/representation/io/implicit_cross_section_input.hpp>

namespace geode
{
    class opengeode_geosciences_implicit_api OpenGeodeImplicitCrossSectionInput
        final : public ImplicitCrossSectionInput
    {
    public:
        explicit OpenGeodeImplicitCrossSectionInput( std::string_view filename )
            : ImplicitCrossSectionInput( filename )
        {
        }

        [[nodiscard]] static std::string_view extension()
        {
            return ImplicitCrossSection::native_extension_static();
        }

        [[nodiscard]] ImplicitCrossSection read() final;

        [[nodiscard]] AdditionalFiles additional_files() const final
        {
            return {};
        }

        [[nodiscard]] index_t object_priority() const final
        {
            return 0;
        }

        [[nodiscard]] Percentage is_loadable() const final
        {
            return Percentage{ 1 };
        }
    };

    namespace detail
    {
        void opengeode_geosciences_implicit_api
            load_implicit_cross_section_files(
                ImplicitCrossSection& section, std::string_view directory );
    } // namespace detail
} // namespace geode
