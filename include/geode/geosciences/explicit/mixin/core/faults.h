/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geosciences/explicit/common.h>
#include <geode/geosciences/explicit/mixin/core/fault.h>

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

    public:
        using Builder = FaultsBuilder< dimension >;
        friend Builder;

        class opengeode_geosciences_explicit_api FaultRangeBase
        {
        public:
            ~FaultRangeBase();

            bool operator!=( const FaultRangeBase& /*unused*/ ) const;

            void operator++();

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

            const FaultRange& begin() const
            {
                return *this;
            }

            const FaultRange& end() const
            {
                return *this;
            }

            const Fault< dimension >& operator*() const;
        };

    public:
        ~Faults();

        index_t nb_faults() const;

        bool has_fault( const uuid& id ) const;

        const Fault< dimension >& fault( const uuid& id ) const;

        FaultRange faults() const;

        void save_faults( absl::string_view directory ) const;

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

            const ModifiableFaultRange& begin() const
            {
                return *this;
            }

            const ModifiableFaultRange& end() const
            {
                return *this;
            }

            Fault< dimension >& operator*() const;
        };

    private:
        const uuid& create_fault();

        const uuid& create_fault(
            typename Fault< dimension >::FAULT_TYPE type );

        void create_fault( uuid fault_id );

        void create_fault(
            uuid fault_id, typename Fault< dimension >::FAULT_TYPE type );

        void delete_fault( const Fault< dimension >& fault );

        void load_faults( absl::string_view directory );

        ModifiableFaultRange modifiable_faults();

        Fault< dimension >& modifiable_fault( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Faults );
} // namespace geode
