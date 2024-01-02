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

#pragma once

#include <geode/geosciences/implicit/representation/core/horizons_stack.h>
#include <geode/geosciences/implicit/representation/io/horizons_stack_input.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeHorizonsStackInput final
        : public HorizonsStackInput< dimension >
    {
    public:
        OpenGeodeHorizonsStackInput( absl::string_view filename )
            : HorizonsStackInput< dimension >( filename )
        {
        }

        static absl::string_view extension()
        {
            return HorizonsStack< dimension >::native_extension_static();
        }

        void load_horizons_stack_files(
            HorizonsStack< dimension >& horizons_stack,
            absl::string_view directory );

        HorizonsStack< dimension > read() final;
    };
    ALIAS_2D_AND_3D( OpenGeodeHorizonsStackInput );
} // namespace geode
