/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/geosciences/mixin/core/faults.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/geosciences/mixin/core/detail/components_storage.h>
#include <geode/geosciences/mixin/core/fault.h>

namespace geode
{
    template < index_t dimension >
    class Faults< dimension >::Impl
        : public detail::GeologicalComponentsStorage< Fault< dimension > >
    {
    };

    template < index_t dimension >
    Faults< dimension >::Faults() // NOLINT
    {
    }

    template < index_t dimension >
    Faults< dimension >::~Faults() // NOLINT
    {
    }

    template < index_t dimension >
    index_t Faults< dimension >::nb_faults() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const Fault< dimension >& Faults< dimension >::fault( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Fault< dimension >& Faults< dimension >::modifiable_fault( const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Faults< dimension >::save_faults( absl::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/faults" ) );
    }

    template < index_t dimension >
    void Faults< dimension >::load_faults( absl::string_view directory )
    {
        impl_->load_components( absl::StrCat( directory, "/faults" ) );
    }

    template < index_t dimension >
    typename Faults< dimension >::FaultRange Faults< dimension >::faults() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename Faults< dimension >::ModifiableFaultRange
        Faults< dimension >::modifiable_faults()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Faults< dimension >::create_fault()
    {
        typename Faults< dimension >::Impl::ComponentPtr fault{
            new Fault< dimension >
        };
        const auto& id = fault->id();
        impl_->add_component( std::move( fault ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Faults< dimension >::create_fault(
        typename Fault< dimension >::FAULT_TYPE type )
    {
        typename Faults< dimension >::Impl::ComponentPtr fault{
            new Fault< dimension >{ type }
        };
        const auto& id = fault->id();
        impl_->add_component( std::move( fault ) );
        return id;
    }

    template < index_t dimension >
    void Faults< dimension >::delete_fault( const Fault< dimension >& fault )
    {
        impl_->delete_component( fault.id() );
    }

    template < index_t dimension >
    class Faults< dimension >::FaultRangeBase::Impl
        : public BaseRange< typename Faults< dimension >::Impl::Iterator >
    {
        using Iterator = typename Faults< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        Fault< dimension >& fault() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    Faults< dimension >::FaultRangeBase::FaultRangeBase( const Faults& faults )
        : impl_( faults.impl_->begin(), faults.impl_->end() )
    {
    }

    template < index_t dimension >
    Faults< dimension >::FaultRangeBase::FaultRangeBase(
        FaultRangeBase&& other ) noexcept
        : impl_( std::move( *other.impl_ ) )
    {
    }

    template < index_t dimension >
    Faults< dimension >::FaultRangeBase::FaultRangeBase(
        const FaultRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Faults< dimension >::FaultRangeBase::~FaultRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool Faults< dimension >::FaultRangeBase::operator!=(
        const FaultRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Faults< dimension >::FaultRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    Faults< dimension >::FaultRange::FaultRange( const Faults& faults )
        : FaultRangeBase( faults )
    {
    }

    template < index_t dimension >
    const Fault< dimension >& Faults< dimension >::FaultRange::operator*() const
    {
        return this->impl_->fault();
    }

    template < index_t dimension >
    Faults< dimension >::ModifiableFaultRange::ModifiableFaultRange(
        const Faults& faults )
        : FaultRangeBase( faults )
    {
    }

    template < index_t dimension >
    Fault< dimension >& Faults< dimension >::ModifiableFaultRange::
        operator*() const
    {
        return this->impl_->fault();
    }

    template class opengeode_geosciences_geosciences_api Faults< 2 >;
    template class opengeode_geosciences_geosciences_api Faults< 3 >;
} // namespace geode
