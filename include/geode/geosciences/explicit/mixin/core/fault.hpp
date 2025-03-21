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

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/model/mixin/core/component.hpp>

#include <geode/geosciences/explicit/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Faults );
    FORWARD_DECLARATION_DIMENSION_CLASS( FaultsBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Geological component describing a Fault.
     * This component is described by a fault type.
     */
    template < index_t dimension >
    class Fault final : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY( Fault );
        PASSKEY( Faults< dimension >, FaultsKey );
        PASSKEY( FaultsBuilder< dimension >, FaultsBuilderKey );

    public:
        enum struct FAULT_TYPE
        {
            /// Default value - No fault type defined
            no_type,
            normal,
            reverse,
            strike_slip,
            listric,
            decollement
        };

    public:
        Fault( Fault&& other ) noexcept;
        ~Fault();

        [[nodiscard]] static ComponentType component_type_static()
        {
            return ComponentType{ "Fault" };
        }

        [[nodiscard]] ComponentType component_type() const final
        {
            return component_type_static();
        }

        [[nodiscard]] bool has_type() const;

        [[nodiscard]] FAULT_TYPE type() const;

    public:
        Fault( FaultsKey ) : Fault() {}
        Fault( FAULT_TYPE type, FaultsKey ) : Fault( type ) {}

        void set_type( FAULT_TYPE type, FaultsBuilderKey );

        void set_fault_name( std::string_view name, FaultsBuilderKey )
        {
            this->set_name( name );
        }

    private:
        Fault();

        explicit Fault( FAULT_TYPE type );

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Fault );
} // namespace geode
