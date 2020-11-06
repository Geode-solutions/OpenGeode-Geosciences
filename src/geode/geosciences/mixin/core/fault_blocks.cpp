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

#include <geode/geosciences/mixin/core/fault_blocks.h>

#include <geode/basic/pimpl_impl.h>
#include <geode/basic/range.h>

#include <geode/geosciences/mixin/core/detail/components_storage.h>
#include <geode/geosciences/mixin/core/fault.h>

namespace geode
{
    template < index_t dimension >
    class FaultBlocks< dimension >::Impl
        : public detail::GeologicalComponentsStorage< FaultBlock< dimension > >
    {
    };

    template < index_t dimension >
    FaultBlocks< dimension >::FaultBlocks() // NOLINT
    {
    }

    template < index_t dimension >
    FaultBlocks< dimension >::~FaultBlocks() // NOLINT
    {
    }

    template < index_t dimension >
    FaultBlocks< dimension >::FaultBlocks( FaultBlocks&& other )
        : impl_( std::move( other.impl_ ) )
    {
    }

    template < index_t dimension >
    index_t FaultBlocks< dimension >::nb_fault_blocks() const
    {
        return impl_->nb_components();
    }

    template < index_t dimension >
    const FaultBlock< dimension >& FaultBlocks< dimension >::fault_block(
        const uuid& id ) const
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    FaultBlock< dimension >& FaultBlocks< dimension >::modifiable_fault_block(
        const uuid& id )
    {
        return impl_->component( id );
    }

    template < index_t dimension >
    void FaultBlocks< dimension >::save_fault_blocks(
        absl::string_view directory ) const
    {
        impl_->save_components( absl::StrCat( directory, "/fault_blocks" ) );
    }

    template < index_t dimension >
    void FaultBlocks< dimension >::load_fault_blocks(
        absl::string_view directory )
    {
        impl_->load_components( absl::StrCat( directory, "/fault_blocks" ) );
    }

    template < index_t dimension >
    typename FaultBlocks< dimension >::FaultBlockRange
        FaultBlocks< dimension >::fault_blocks() const
    {
        return { *this };
    }

    template < index_t dimension >
    typename FaultBlocks< dimension >::ModifiableFaultBlockRange
        FaultBlocks< dimension >::modifiable_fault_blocks()
    {
        return { *this };
    }

    template < index_t dimension >
    const uuid& FaultBlocks< dimension >::create_fault_block()
    {
        typename FaultBlocks< dimension >::Impl::ComponentPtr fault_block{
            new FaultBlock< dimension >{
                typename FaultBlock< dimension >::FaultBlocksKey{} }
        };
        const auto& id = fault_block->id();
        impl_->add_component( std::move( fault_block ) );
        return id;
    }

    template < index_t dimension >
    void FaultBlocks< dimension >::delete_fault_block(
        const FaultBlock< dimension >& fault_block )
    {
        impl_->delete_component( fault_block.id() );
    }

    template < index_t dimension >
    class FaultBlocks< dimension >::FaultBlockRangeBase::Impl
        : public BaseRange< typename FaultBlocks< dimension >::Impl::Iterator >
    {
        using Iterator = typename FaultBlocks< dimension >::Impl::Iterator;

    public:
        Impl( Iterator begin, Iterator end )
            : BaseRange< Iterator >( begin, end )
        {
        }

        FaultBlock< dimension >& fault_block() const
        {
            return *this->current()->second;
        }
    };

    template < index_t dimension >
    FaultBlocks< dimension >::FaultBlockRangeBase::FaultBlockRangeBase(
        const FaultBlocks& fault_blocks )
        : impl_( fault_blocks.impl_->begin(), fault_blocks.impl_->end() )
    {
    }

    template < index_t dimension >
    FaultBlocks< dimension >::FaultBlockRangeBase::FaultBlockRangeBase(
        FaultBlockRangeBase&& other ) noexcept
        : impl_( std::move( *other.impl_ ) )
    {
    }

    template < index_t dimension >
    FaultBlocks< dimension >::FaultBlockRangeBase::FaultBlockRangeBase(
        const FaultBlockRangeBase& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    FaultBlocks<
        dimension >::FaultBlockRangeBase::~FaultBlockRangeBase() // NOLINT
    {
    }

    template < index_t dimension >
    bool FaultBlocks< dimension >::FaultBlockRangeBase::operator!=(
        const FaultBlockRangeBase& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void FaultBlocks< dimension >::FaultBlockRangeBase::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    FaultBlocks< dimension >::FaultBlockRange::FaultBlockRange(
        const FaultBlocks& fault_blocks )
        : FaultBlockRangeBase( fault_blocks )
    {
    }

    template < index_t dimension >
    const FaultBlock< dimension >&
        FaultBlocks< dimension >::FaultBlockRange::operator*() const
    {
        return this->impl_->fault_block();
    }

    template < index_t dimension >
    FaultBlocks< dimension >::ModifiableFaultBlockRange::
        ModifiableFaultBlockRange( const FaultBlocks& fault_blocks )
        : FaultBlockRangeBase( fault_blocks )
    {
    }

    template < index_t dimension >
    FaultBlock< dimension >&
        FaultBlocks< dimension >::ModifiableFaultBlockRange::operator*() const
    {
        return this->impl_->fault_block();
    }

    template class opengeode_geosciences_geosciences_api FaultBlocks< 2 >;
    template class opengeode_geosciences_geosciences_api FaultBlocks< 3 >;
} // namespace geode
