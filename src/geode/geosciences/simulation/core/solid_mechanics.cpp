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

#include <geode/geosciences/simulation/core/solid_mechanics.hpp>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

namespace geode
{
    template < typename Archive >
    void SolidMechanics::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, SolidMechanics >{
                { []( Archive& local_archive, SolidMechanics& mechanics ) {
                    local_archive.value4b( mechanics.law );
                    local_archive.value4b( mechanics.sign_convention );
                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, SolidMechanics );
} // namespace geode
