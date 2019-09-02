/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/geosciences/common.h>
#include <geode/geosciences/core/horizon.h>

#include <geode/mesh/core/mesh_type.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_geosciences_geosciences_api Horizons
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Horizons );
        friend class HorizonsBuilder< dimension >;

    public:
        class opengeode_geosciences_geosciences_api HorizonRangeBase
        {
        public:
            ~HorizonRangeBase();

            bool operator!=( const HorizonRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            HorizonRangeBase( const Horizons& horizons );
            HorizonRangeBase( HorizonRangeBase&& other ) noexcept;
            HorizonRangeBase( const HorizonRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_geosciences_geosciences_api HorizonRange
            : public HorizonRangeBase
        {
        public:
            HorizonRange( const Horizons& horizons );

            const HorizonRange& begin() const
            {
                return *this;
            }

            const HorizonRange& end() const
            {
                return *this;
            }

            const Horizon< dimension >& operator*() const;
        };

    public:
        ~Horizons();

        index_t nb_horizons() const;

        const Horizon< dimension >& horizon( const uuid& id ) const;

        HorizonRange horizons() const;

        std::vector< std::string > save_horizons(
            const std::string& directory ) const;

    protected:
        friend class HorizonsBuilder< dimension >;
        Horizons();

    private:
        class ModifiableHorizonRange : public HorizonRangeBase
        {
        public:
            ModifiableHorizonRange( const Horizons& horizons );

            const ModifiableHorizonRange& begin() const
            {
                return *this;
            }

            const ModifiableHorizonRange& end() const
            {
                return *this;
            }

            Horizon< dimension >& operator*() const;
        };

    private:
        const uuid& create_horizon();

        const uuid& create_horizon(
            typename Horizon< dimension >::HORIZON_TYPE type );

        void delete_horizon( const Horizon< dimension >& horizon );

        void load_horizons( const std::string& directory );

        ModifiableHorizonRange modifiable_horizons();

        Horizon< dimension >& modifiable_horizon( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Horizons );
} // namespace geode
