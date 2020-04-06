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

#include <geode/geosciences/mixin/core/layers.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/geosciences/mixin/core/detail/components_storage.h>
#include <geode/geosciences/mixin/core/fault.h>

namespace geode
{
    template < index_t dimension >
    class Layers< dimension >::Impl
        : public detail::GeologicalComponentsStorage< Layer< dimension > >
    {
    };

    template < index_t dimension >
    Layers< dimension >::Layers() // NOLINT
    {
    }

    template < index_t dimension >
    Layers< dimension >::~Layers() // NOLINT
    {
    }

    template < index_t dimension >
    index_t Layers< dimension >::nb_layers() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const Layer< dimension >& Layers< dimension >::layer( const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    Layer< dimension >& Layers< dimension >::modifiable_layer( const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void Layers< dimension >::save_layers( absl::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/layers" ) );
    }

    template < index_t dimension >
    void Layers< dimension >::load_layers( absl::string_view directory )
    {
        impl_->load_components( absl::StrCat( directory, "/layers" ) );
    }

    template < index_t dimension >
    typename Layers< dimension >::LayerRange Layers< dimension >::layers() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename Layers< dimension >::ModifiableLayerRange
        Layers< dimension >::modifiable_layers()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& Layers< dimension >::create_layer()
    {
        typename Layers< dimension >::Impl::ComponentPtr layer{
            new Layer< dimension >
        };
        const auto& id = layer->id();
        impl_->add_component( std::move( layer ) );
        return id;
    }

    template < index_t dimension >
    void Layers< dimension >::delete_layer( const Layer< dimension >& layer )
    {
        impl_->delete_component( layer.id() );
    }

    template < index_t dimension >
    class Layers< dimension >::LayerRangeBase::Impl
        : public BaseRange< typename Layers< dimension >::Impl::Iterator >
    {
        using Iterator = typename Layers< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        Layer< dimension >& layer() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    Layers< dimension >::LayerRangeBase::LayerRangeBase( const Layers& layers )
        : impl_( layers.impl_->begin(), layers.impl_->end() )
    {
    }

    template < index_t dimension >
    Layers< dimension >::LayerRangeBase::LayerRangeBase(
        LayerRangeBase&& other ) noexcept
        : impl_( std::move( *other.impl_ ) )
    {
    }

    template < index_t dimension >
    Layers< dimension >::LayerRangeBase::LayerRangeBase(
        const LayerRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    Layers< dimension >::LayerRangeBase::~LayerRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool Layers< dimension >::LayerRangeBase::operator!=(
        const LayerRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void Layers< dimension >::LayerRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    Layers< dimension >::LayerRange::LayerRange( const Layers& layers )
        : LayerRangeBase( layers )
    {
    }

    template < index_t dimension >
    const Layer< dimension >& Layers< dimension >::LayerRange::operator*() const
    {
        return this->impl_->layer();
    }

    template < index_t dimension >
    Layers< dimension >::ModifiableLayerRange::ModifiableLayerRange(
        const Layers& layers )
        : LayerRangeBase( layers )
    {
    }

    template < index_t dimension >
    Layer< dimension >& Layers< dimension >::ModifiableLayerRange::
        operator*() const
    {
        return this->impl_->layer();
    }

    template class opengeode_geosciences_geosciences_api Layers< 2 >;
    template class opengeode_geosciences_geosciences_api Layers< 3 >;
} // namespace geode
