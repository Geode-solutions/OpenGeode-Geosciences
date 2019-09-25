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

#include <geode/model/mixin/core/component.h>

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
        OPENGEODE_DISABLE_COPY_AND_MOVE( Fault );
        friend class Faults< dimension >;

    public:
        enum struct FAULT_TYPE
        {
            /// Default value - No fault type defined
            NO_TYPE,
            NORMAL,
            REVERSE,
            STRIKE_SLIP,
            LISTRIC,
            DECOLLEMENT
        };

    public:
        ~Fault();

        static ComponentType component_type_static()
        {
            return ComponentType{ "Fault" };
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

        FAULT_TYPE type() const;

    protected:
        friend class bitsery::Access;
        Fault();

        explicit Fault( FAULT_TYPE type );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        friend class FaultsBuilder< dimension >;
        void set_type( FAULT_TYPE type );

        friend class FaultsBuilder< dimension >;
        void set_fault_name( std::string name )
        {
            this->set_name( std::move( name ) );
        }

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Fault );
} // namespace geode
