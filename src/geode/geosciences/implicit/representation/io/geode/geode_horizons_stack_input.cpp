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

#include <geode/geosciences/implicit/representation/io/geode/geode_horizons_stack_input.h>

#include <async++.h>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.h>
#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

namespace geode
{
    template < index_t dimension >
    void OpenGeodeHorizonsStackInput< dimension >::load_horizons_stack_files(
        HorizonsStack< dimension >& horizons_stack,
        absl::string_view directory )
    {
        HorizonsStackBuilder< dimension > builder{ horizons_stack };
        async::parallel_invoke(
            [&builder, &directory] {
                builder.load_identifier( directory );
            },
            [&builder, &directory] {
                builder.load_horizons( directory );
                builder.load_stratigraphic_units( directory );
            },
            [&builder, &directory] {
                builder.load_stratigraphic_relationships( directory );
            } );
    }

    template < index_t dimension >
    HorizonsStack< dimension > OpenGeodeHorizonsStackInput< dimension >::read()
    {
        const UnzipFile zip_reader{ this->filename(), uuid{}.string() };
        zip_reader.extract_all();
        HorizonsStack< dimension > horizons_stack;
        load_horizons_stack_files( horizons_stack, zip_reader.directory() );
        return horizons_stack;
    }

    template class opengeode_geosciences_implicit_api
        OpenGeodeHorizonsStackInput< 2 >;
    template class opengeode_geosciences_implicit_api
        OpenGeodeHorizonsStackInput< 3 >;
} // namespace geode
