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

#pragma once

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/geosciences/explicit/common.hpp>
#include <geode/geosciences/explicit/mixin/core/fault.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( FaultsBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_geosciences_explicit_api Faults
    {
        OPENGEODE_DISABLE_COPY( Faults );
        PASSKEY( FaultsBuilder< dimension >, FaultsBuilderKey );

    public:
        using Builder = FaultsBuilder< dimension >;
        using Type = Fault< dimension >;

        class opengeode_geosciences_explicit_api FaultRangeBase
        {
        public:
            ~FaultRangeBase();

            [[nodiscard]] bool operator!=(
                const FaultRangeBase& /*unused*/ ) const;

            void operator++();

            void set_active_only();

        protected:
            FaultRangeBase( const Faults& faults );
            FaultRangeBase( FaultRangeBase&& other ) noexcept;
            FaultRangeBase( const FaultRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_geosciences_explicit_api FaultRange
            : public FaultRangeBase
        {
        public:
            FaultRange( const Faults& faults );

            [[nodiscard]] const FaultRange& begin() const
            {
                return *this;
            }

            [[nodiscard]] const FaultRange& end() const
            {
                return *this;
            }

            [[nodiscard]] const Fault< dimension >& operator*() const;
        };

    public:
        ~Faults();

        [[nodiscard]] index_t nb_faults() const;

        [[nodiscard]] index_t nb_active_faults() const;

        [[nodiscard]] bool has_fault( const uuid& id ) const;

        [[nodiscard]] const Fault< dimension >& fault( const uuid& id ) const;

        [[nodiscard]] FaultRange faults() const;

        [[nodiscard]] FaultRange active_faults() const;

        [[nodiscard]] FaultRange components() const
        {
            return faults();
        }

        [[nodiscard]] bool has_component( const uuid& id ) const
        {
            return has_fault( id );
        }

        [[nodiscard]] const Fault< dimension >& component(
            const uuid& id ) const
        {
            return fault( id );
        }

        void save_faults( std::string_view directory ) const;

    protected:
        friend class FaultsBuilder< dimension >;
        Faults();
        Faults( Faults&& other ) noexcept;

        Faults& operator=( Faults&& other ) noexcept;

    private:
        class ModifiableFaultRange : public FaultRangeBase
        {
        public:
            ModifiableFaultRange( const Faults& faults );

            [[nodiscard]] const ModifiableFaultRange& begin() const
            {
                return *this;
            }

            [[nodiscard]] const ModifiableFaultRange& end() const
            {
                return *this;
            }

            [[nodiscard]] Fault< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_fault( FaultsBuilderKey key );

        [[nodiscard]] const uuid& create_fault(
            typename Fault< dimension >::FAULT_TYPE type,
            FaultsBuilderKey key );

        void create_fault( uuid fault_id, FaultsBuilderKey key );

        void create_fault( uuid fault_id,
            typename Fault< dimension >::FAULT_TYPE type,
            FaultsBuilderKey key );

        void delete_fault(
            const Fault< dimension >& fault, FaultsBuilderKey key );

        void load_faults( std::string_view directory, FaultsBuilderKey key );

        [[nodiscard]] ModifiableFaultRange modifiable_faults(
            FaultsBuilderKey key );

        [[nodiscard]] Fault< dimension >& modifiable_fault(
            const uuid& id, FaultsBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Faults );
} // namespace geode
