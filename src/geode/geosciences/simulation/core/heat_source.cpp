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

#include <geode/geosciences/simulation/core/heat_source.hpp>

#include <bitsery/ext/std_optional.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

namespace geode
{
    template < typename Archive >
    void HeatSource::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, HeatSource >{
                { []( Archive& local_archive, HeatSource& source ) {
                    local_archive.text1b( source.name, source.name.max_size() );
                    local_archive.ext( source.block,
                        bitsery::ext::StdOptional{},
                        []( Archive& block_archive, uuid& block_id ) {
                            block_archive.object( block_id );
                        } );
                    local_archive.container8b(
                        source.times, source.times.max_size() );
                    local_archive.container8b(
                        source.powers, source.powers.max_size() );
                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, HeatSource );
} // namespace geode
