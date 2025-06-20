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

#include <geode/geosciences/explicit/mixin/builder/horizons_builder.hpp>

#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/horizons.hpp>

namespace geode
{
    template < index_t dimension >
    const uuid& HorizonsBuilder< dimension >::create_horizon()
    {
        return horizons_.create_horizon( {} );
    }

    template < index_t dimension >
    const uuid& HorizonsBuilder< dimension >::create_horizon(
        CONTACT_TYPE type )
    {
        return horizons_.create_horizon( type, {} );
    }

    template < index_t dimension >
    void HorizonsBuilder< dimension >::create_horizon( uuid horizon_id )
    {
        horizons_.create_horizon( std::move( horizon_id ), {} );
    }

    template < index_t dimension >
    void HorizonsBuilder< dimension >::create_horizon(
        uuid horizon_id, CONTACT_TYPE type )
    {
        horizons_.create_horizon( std::move( horizon_id ), type, {} );
    }

    template < index_t dimension >
    void HorizonsBuilder< dimension >::delete_horizon(
        const Horizon< dimension >& horizon )
    {
        horizons_.delete_horizon( horizon, {} );
    }

    template < index_t dimension >
    void HorizonsBuilder< dimension >::load_horizons(
        std::string_view directory )
    {
        return horizons_.load_horizons( directory, {} );
    }

    template < index_t dimension >
    void HorizonsBuilder< dimension >::set_horizon_contact_type(
        const uuid& horizon_id, CONTACT_TYPE contact_type )
    {
        horizons_.modifiable_horizon( horizon_id, {} )
            .set_contact_type( contact_type,
                typename Horizon< dimension >::HorizonsBuilderKey{} );
    }

    template < index_t dimension >
    void HorizonsBuilder< dimension >::set_horizon_name(
        const uuid& id, std::string_view name )
    {
        horizons_.modifiable_horizon( id, {} ).set_horizon_name(
            name, typename Horizon< dimension >::HorizonsBuilderKey{} );
    }

    template class opengeode_geosciences_explicit_api HorizonsBuilder< 2 >;
    template class opengeode_geosciences_explicit_api HorizonsBuilder< 3 >;
} // namespace geode
