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

#include <geode/geosciences/explicit/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Horizon );
    FORWARD_DECLARATION_DIMENSION_CLASS( Horizons );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class HorizonsBuilder
    {
    public:
        void load_horizons( absl::string_view directory );

        void set_horizon_type( const uuid& horizon_id,
            typename Horizon< dimension >::HORIZON_TYPE type );

        void set_horizon_name( const uuid& id, absl::string_view name );

    protected:
        explicit HorizonsBuilder( Horizons< dimension >& horizons )
            : horizons_( horizons )
        {
        }

        const uuid& create_horizon();

        const uuid& create_horizon(
            typename Horizon< dimension >::HORIZON_TYPE type );

        void create_horizon( uuid horizon_id );

        void create_horizon(
            uuid horizon_id, typename Horizon< dimension >::HORIZON_TYPE type );

        void delete_horizon( const Horizon< dimension >& horizon );

    private:
        Horizons< dimension >& horizons_;
    };
    ALIAS_2D_AND_3D( HorizonsBuilder );
} // namespace geode
