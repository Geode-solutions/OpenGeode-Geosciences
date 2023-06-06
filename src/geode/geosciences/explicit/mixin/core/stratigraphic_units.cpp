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

#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/model/mixin/core/detail/components_storage.h>

#include <geode/geosciences/explicit/mixin/core/fault.h>

namespace geode
{
    template < index_t dimension >
    class StratigraphicUnits< dimension >::Impl
        : public detail::ComponentsStorage< StratigraphicUnit< dimension > >
    {
    };

    template < index_t dimension >
    StratigraphicUnits< dimension >::StratigraphicUnits() // NOLINT
    {
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >::~StratigraphicUnits() // NOLINT
    {
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >::StratigraphicUnits(
        StratigraphicUnits&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >& StratigraphicUnits< dimension >::operator=(
        StratigraphicUnits< dimension >&& other )
    {
        impl_ = std::move( other.impl_ );
        return *this;
    }

    template < index_t dimension >
    index_t StratigraphicUnits< dimension >::nb_stratigraphic_units() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    bool StratigraphicUnits< dimension >::has_stratigraphic_unit(
        const uuid& id ) const
    {
        return impl_->has_component( id );
    }

    template < index_t dimension >
    const StratigraphicUnit< dimension >&
        StratigraphicUnits< dimension >::stratigraphic_unit(
            const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    StratigraphicUnit< dimension >&
        StratigraphicUnits< dimension >::modifiable_stratigraphic_unit(
            const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void StratigraphicUnits< dimension >::save_stratigraphic_units(
        absl::string_view directory ) const
    {
        impl_->save_components(
            absl::StrCat( directory, "/stratigraphic_units" ) );
    }

    template < index_t dimension >
    void StratigraphicUnits< dimension >::load_stratigraphic_units(
        absl::string_view directory )
    {
        impl_->load_components(
            absl::StrCat( directory, "/stratigraphic_units" ) );
    }

    template < index_t dimension >
    typename StratigraphicUnits< dimension >::StratigraphicUnitRange
        StratigraphicUnits< dimension >::stratigraphic_units() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename StratigraphicUnits< dimension >::ModifiableStratigraphicUnitRange
        StratigraphicUnits< dimension >::modifiable_stratigraphic_units()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& StratigraphicUnits< dimension >::create_stratigraphic_unit()
    {
        typename StratigraphicUnits< dimension >::Impl::ComponentPtr
            stratigraphic_unit{ new StratigraphicUnit< dimension >{
                typename StratigraphicUnit<
                    dimension >::StratigraphicUnitsKey{} } };
        const auto& id = stratigraphic_unit->id();
        impl_->add_component( std::move( stratigraphic_unit ) );
        return id;
    }

    template < index_t dimension >
    void StratigraphicUnits< dimension >::delete_stratigraphic_unit(
        const StratigraphicUnit< dimension >& stratigraphic_unit )
    {
        impl_->delete_component( stratigraphic_unit.id() );
    }

    template < index_t dimension >
    class StratigraphicUnits< dimension >::StratigraphicUnitRangeBase::Impl
        : public BaseRange<
              typename StratigraphicUnits< dimension >::Impl::Iterator >
    {
        using Iterator =
            typename StratigraphicUnits< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        StratigraphicUnit< dimension >& stratigraphic_unit() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    StratigraphicUnits< dimension >::StratigraphicUnitRangeBase::
        StratigraphicUnitRangeBase(
            const StratigraphicUnits& stratigraphic_units )
        : impl_( stratigraphic_units.impl_->begin(),
            stratigraphic_units.impl_->end() )
    {
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >::StratigraphicUnitRangeBase::
        StratigraphicUnitRangeBase(
            StratigraphicUnitRangeBase&& other ) noexcept
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >::StratigraphicUnitRangeBase::
        StratigraphicUnitRangeBase( const StratigraphicUnitRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >::StratigraphicUnitRangeBase::
        ~StratigraphicUnitRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool
        StratigraphicUnits< dimension >::StratigraphicUnitRangeBase::operator!=(
            const StratigraphicUnitRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void StratigraphicUnits<
        dimension >::StratigraphicUnitRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >::StratigraphicUnitRange::
        StratigraphicUnitRange( const StratigraphicUnits& stratigraphic_units )
        : StratigraphicUnitRangeBase( stratigraphic_units )
    {
    }

    template < index_t dimension >
    const StratigraphicUnit< dimension >&
        StratigraphicUnits< dimension >::StratigraphicUnitRange::operator*()
            const
    {
        return this->impl_->stratigraphic_unit();
    }

    template < index_t dimension >
    StratigraphicUnits< dimension >::ModifiableStratigraphicUnitRange::
        ModifiableStratigraphicUnitRange(
            const StratigraphicUnits& stratigraphic_units )
        : StratigraphicUnitRangeBase( stratigraphic_units )
    {
    }

    template < index_t dimension >
    StratigraphicUnit< dimension >& StratigraphicUnits<
        dimension >::ModifiableStratigraphicUnitRange::operator*() const
    {
        return this->impl_->stratigraphic_unit();
    }

    template class opengeode_geosciences_explicit_api StratigraphicUnits< 2 >;
    template class opengeode_geosciences_explicit_api StratigraphicUnits< 3 >;
} // namespace geode
