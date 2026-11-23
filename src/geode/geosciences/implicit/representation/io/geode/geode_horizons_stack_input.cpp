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

#include <geode/geosciences/implicit/representation/io/geode/geode_horizons_stack_input.hpp>

#include <async++.h>

#include <geode/basic/detail/input_files.hpp>

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

namespace geode
{
    template < index_t dimension >
    void OpenGeodeHorizonsStackInput< dimension >::load_horizons_stack_files(
        HorizonsStack< dimension >& horizons_stack,
        const detail::InputFiles& files )
    {
        HorizonsStackBuilder< dimension > builder{ horizons_stack };
        async::parallel_invoke(
            [&builder, &files] {
                builder.load_identifier( files );
            },
            [&builder, &files] {
                builder.load_horizons( files );
                builder.load_stratigraphic_units( files );
            },
            [&builder, &files] {
                builder.load_stratigraphic_relationships( files );
            } );
        builder.compute_top_and_bottom_horizons();
    }

    template < index_t dimension >
    std::vector< HorizonsStack< dimension > >
        OpenGeodeHorizonsStackInput< dimension >::read()
    {
        const detail::ArchiveInputFiles files{ this->filename() };
        HorizonsStack< dimension > horizons_stack{ BITSERY::constructor };
        load_horizons_stack_files( horizons_stack, files );
        std::vector< HorizonsStack< dimension > > results;
        results.push_back( std::move( horizons_stack ) );
        return results;
    }

    template class opengeode_geosciences_implicit_api
        OpenGeodeHorizonsStackInput< 2 >;
    template class opengeode_geosciences_implicit_api
        OpenGeodeHorizonsStackInput< 3 >;
} // namespace geode
