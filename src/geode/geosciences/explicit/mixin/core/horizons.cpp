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

#include <geode/geosciences/explicit/mixin/core/horizons.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/geosciences/explicit/mixin/core/detail/components_storage.h>
#include <geode/geosciences/explicit/mixin/core/horizon.h>

namespace geode
{
    template < index_t dimension >
    class Horizons< dimension >::Impl
        : public detail::GeologicalComponentsStorage< Horizon< dimension > >
    {
    };

    template < index_t dimension >
    Horizons< dimension >::Horizons() // NOLINT
    {
    }

    template < index_t dimension >
    Horizons< dimension >::~Horizons() // NOLINT
    {
    }

    template < index_t dimension >
    Horizons< dimension >::Horizons( Horizons&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Horizons< dimension >& Horizons< dimension >::operator=(
        Horizons< dimension >&& other )
    {
        impl_ = std::move( other.impl_ );
        return *this;
    }

    template < index_t dimension >
    index_t Horizons< dimension >::nb_horizons() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    bool Horizons< dimension >::has_horizon( const uuid& id ) const
    {
        return impl_->has_component( id );
    }

    template < index_t dimension >
    const Horizon< dimension >& Horizons< dimension >::horizon(
        const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Horizon< dimension >& Horizons< dimension >::modifiable_horizon(
        const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Horizons< dimension >::save_horizons(
        absl::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/horizons" ) );
    }

    template < index_t dimension >
    void Horizons< dimension >::load_horizons( absl::string_view directory )
    {
        impl_->load_components( absl::StrCat( directory, "/horizons" ) );
    }

    template < index_t dimension >
    typename Horizons< dimension >::HorizonRange
        Horizons< dimension >::horizons() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename Horizons< dimension >::ModifiableHorizonRange
        Horizons< dimension >::modifiable_horizons()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Horizons< dimension >::create_horizon()
    {
        typename Horizons< dimension >::Impl::ComponentPtr horizon{
            new Horizon< dimension >{
                typename Horizon< dimension >::HorizonsKey{} }
        };
        const auto& id = horizon->id();
        impl_->add_component( std::move( horizon ) );
        return id;
    }

    template < index_t dimension >
    const uuid& Horizons< dimension >::create_horizon(
        typename Horizon< dimension >::HORIZON_TYPE type )
    {
        typename Horizons< dimension >::Impl::ComponentPtr horizon{
            new Horizon< dimension >{
                type, typename Horizon< dimension >::HorizonsKey{} }
        };
        const auto& id = horizon->id();
        impl_->add_component( std::move( horizon ) );
        return id;
    }

    template < index_t dimension >
    void Horizons< dimension >::delete_horizon(
        const Horizon< dimension >& horizon )
    {
        impl_->delete_component( horizon.id() );
    }

    template < index_t dimension >
    class Horizons< dimension >::HorizonRangeBase::Impl
        : public BaseRange< typename Horizons< dimension >::Impl::Iterator >
    {
        using Iterator = typename Horizons< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        Horizon< dimension >& horizon() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    Horizons< dimension >::HorizonRangeBase::HorizonRangeBase(
        const Horizons& horizons )
        : impl_( horizons.impl_->begin(), horizons.impl_->end() )
    {
    }

    template < index_t dimension >
    Horizons< dimension >::HorizonRangeBase::HorizonRangeBase(
        HorizonRangeBase&& other ) noexcept
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    Horizons< dimension >::HorizonRangeBase::HorizonRangeBase(
        const HorizonRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Horizons< dimension >::HorizonRangeBase::~HorizonRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool Horizons< dimension >::HorizonRangeBase::operator!=(
        const HorizonRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Horizons< dimension >::HorizonRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    Horizons< dimension >::HorizonRange::HorizonRange(
        const Horizons& horizons )
        : HorizonRangeBase( horizons )
    {
    }

    template < index_t dimension >
    const Horizon< dimension >&
        Horizons< dimension >::HorizonRange::operator*() const
    {
        return this->impl_->horizon();
    }

    template < index_t dimension >
    Horizons< dimension >::ModifiableHorizonRange::ModifiableHorizonRange(
        const Horizons& horizons )
        : HorizonRangeBase( horizons )
    {
    }

    template < index_t dimension >
    Horizon< dimension >&
        Horizons< dimension >::ModifiableHorizonRange::operator*() const
    {
        return this->impl_->horizon();
    }

    template class opengeode_geosciences_explicit_api Horizons< 2 >;
    template class opengeode_geosciences_explicit_api Horizons< 3 >;
} // namespace geode
