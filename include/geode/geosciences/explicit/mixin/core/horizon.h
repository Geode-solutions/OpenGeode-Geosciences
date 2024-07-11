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

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/model/mixin/core/component.h>

#include <geode/geosciences/explicit/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Horizons );
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Geological component describing a Horizon.
     * This component is described by a horizon type.
     */
    template < index_t dimension >
    class Horizon final : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY( Horizon );
        PASSKEY( HorizonsBuilder< dimension >, HorizonsBuilderKey );
        PASSKEY( Horizons< dimension >, HorizonsKey );

    public:
        enum struct HORIZON_TYPE
        {
            /// Default value - No horizon type defined
            NO_TYPE,
            CONFORMAL,
            NON_CONFORMAL,
            TOPOGRAPHY,
            INTRUSION
        };

    public:
        Horizon( Horizon&& other ) noexcept;
        ~Horizon();

        static ComponentType component_type_static()
        {
            return ComponentType{ "Horizon" };
        }

        ComponentType component_type() const final
        {
            return component_type_static();
        }

        ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        bool has_type() const;

        HORIZON_TYPE type() const;

        Horizon( HorizonsKey ) : Horizon() {}
        Horizon( HORIZON_TYPE type, HorizonsKey ) : Horizon( type ) {}

        void set_type( HORIZON_TYPE type, HorizonsBuilderKey );

        void set_horizon_name( std::string_view name, HorizonsBuilderKey )
        {
            this->set_name( name );
        }

    private:
        Horizon();
        explicit Horizon( HORIZON_TYPE type );

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Horizon );
} // namespace geode
