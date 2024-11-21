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

#include <geode/geosciences/explicit/mixin/core/faults.hpp>

#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/detail/components_storage.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>

namespace geode
{
    template < index_t dimension >
    class Faults< dimension >::Impl
        : public detail::ComponentsStorage< Fault< dimension > >
    {
    };

    template < index_t dimension >
    Faults< dimension >::Faults() = default;

    template < index_t dimension >
    Faults< dimension >::~Faults() = default;

    template < index_t dimension >
    Faults< dimension >::Faults( Faults&& ) noexcept = default;

    template < index_t dimension >
    Faults< dimension >& Faults< dimension >::operator=(
        Faults< dimension >&& ) noexcept = default;

    template < index_t dimension >
    index_t Faults< dimension >::nb_faults() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    bool Faults< dimension >::has_fault( const uuid& id ) const
    {
        return impl_->has_component( id );
    }

    template < index_t dimension >
    const Fault< dimension >& Faults< dimension >::fault( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Fault< dimension >& Faults< dimension >::modifiable_fault(
        const uuid& id, FaultsBuilderKey /*unused*/ )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Faults< dimension >::save_faults( std::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/faults" ) );
    }

    template < index_t dimension >
    void Faults< dimension >::load_faults(
        std::string_view directory, FaultsBuilderKey /*unused*/ )
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
        Faults< dimension >::modifiable_faults( FaultsBuilderKey /*unused*/ )
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Faults< dimension >::create_fault( FaultsBuilderKey /*unused*/ )
    {
        typename Faults< dimension >::Impl::ComponentPtr fault{
            new Fault< dimension >{ typename Fault< dimension >::FaultsKey{} }
        };
        const auto& id = fault->id();
        impl_->add_component( std::move( fault ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Faults< dimension >::create_fault(
        typename Fault< dimension >::FAULT_TYPE type,
        FaultsBuilderKey /*unused*/ )
    {
        typename Faults< dimension >::Impl::ComponentPtr fault{
            new Fault< dimension >{
                type, typename Fault< dimension >::FaultsKey{} }
        };
        const auto& id = fault->id();
        impl_->add_component( std::move( fault ) );
        return id;
    }

    template < index_t dimension >
    void Faults< dimension >::create_fault(
        uuid fault_id, FaultsBuilderKey /*unused*/ )
    {
        typename Faults< dimension >::Impl::ComponentPtr fault{
            new Fault< dimension >{ typename Fault< dimension >::FaultsKey{} }
        };
        IdentifierBuilder{ *fault }.set_id( std::move( fault_id ) );
        impl_->add_component( std::move( fault ) );
    }

    template < index_t dimension >
    void Faults< dimension >::create_fault( uuid fault_id,
        typename Fault< dimension >::FAULT_TYPE type,
        FaultsBuilderKey /*unused*/ )
    {
        typename Faults< dimension >::Impl::ComponentPtr fault{
            new Fault< dimension >{
                type, typename Fault< dimension >::FaultsKey{} }
        };
        IdentifierBuilder{ *fault }.set_id( std::move( fault_id ) );
        impl_->add_component( std::move( fault ) );
    }

    template < index_t dimension >
    void Faults< dimension >::delete_fault(
        const Fault< dimension >& fault, FaultsBuilderKey /*unused*/ )
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
        FaultRangeBase&& ) noexcept = default;

    template < index_t dimension >
    Faults< dimension >::FaultRangeBase::FaultRangeBase(
        const FaultRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Faults< dimension >::FaultRangeBase::~FaultRangeBase() = default;

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
    Fault< dimension >&
        Faults< dimension >::ModifiableFaultRange::operator*() const
    {
        return this->impl_->fault();
    }

    template class opengeode_geosciences_explicit_api Faults< 2 >;
    template class opengeode_geosciences_explicit_api Faults< 3 >;
} // namespace geode
