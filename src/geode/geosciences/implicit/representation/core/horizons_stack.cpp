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

#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geosciences/explicit/representation/core/detail/clone.hpp>
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>

namespace geode
{
    template < index_t dimension >
    class HorizonsStack< dimension >::Impl
    {
    public:
        std::optional< uuid > top_horizon() const
        {
            return top_horizon_;
        }

        std::optional< uuid > bottom_horizon() const
        {
            return bottom_horizon_;
        }

        void set_top_horizon( uuid horizon_id )
        {
            top_horizon_ = horizon_id;
        }

        void set_bottom_horizon( uuid horizon_id )
        {
            bottom_horizon_ = horizon_id;
        }

    private:
        std::optional< uuid > top_horizon_{ std::nullopt };
        std::optional< uuid > bottom_horizon_{ std::nullopt };
    };

    template < index_t dimension >
    HorizonsStack< dimension >::HorizonsStack() = default;

    template < index_t dimension >
    HorizonsStack< dimension >::HorizonsStack(
        HorizonsStack< dimension >&& ) noexcept = default;

    template < index_t dimension >
    HorizonsStack< dimension >::~HorizonsStack() = default;

    template < index_t dimension >
    HorizonsStack< dimension >& HorizonsStack< dimension >::operator=(
        HorizonsStack< dimension >&& ) noexcept = default;

    template < index_t dimension >
    HorizonsStack< dimension > HorizonsStack< dimension >::clone() const
    {
        HorizonsStack< dimension > stack_clone;
        HorizonsStackBuilder< dimension > clone_builder{ stack_clone };
        clone_builder.copy_identifier( *this );
        ModelCopyMapping clone_mapping;
        detail::add_horizons_clone_mapping( *this,
            clone_mapping
                [geode::Horizon< dimension >::component_type_static()] );
        detail::add_stratigraphic_units_clone_mapping(
            *this, clone_mapping[geode::StratigraphicUnit<
                       dimension >::component_type_static()] );
        clone_builder.copy( clone_mapping, *this );
        if( impl_->top_horizon() && impl_->bottom_horizon() )
        {
            clone_builder.compute_top_and_bottom_horizons();
        }
        return stack_clone;
    }

    template < index_t dimension >
    std::optional< uuid > HorizonsStack< dimension >::top_horizon() const
    {
        return impl_->top_horizon();
    }

    template < index_t dimension >
    std::optional< uuid > HorizonsStack< dimension >::bottom_horizon() const
    {
        return impl_->bottom_horizon();
    }

    template < index_t dimension >
    auto HorizonsStack< dimension >::bottom_to_top_horizons() const
        -> HorizonOrderedRange
    {
        if( !impl_->top_horizon() || !impl_->bottom_horizon() )
        {
            Logger::warn(
                "[HorizonsStack::bottom_to_top_horizons] Iteration "
                "on HorizonsStack will be empty: top and bottom "
                "horizons have not been computed, or stack is empty." );
        }
        return { *this, true };
    }

    template < index_t dimension >
    auto HorizonsStack< dimension >::bottom_to_top_units() const
        -> StratigraphicUnitOrderedRange
    {
        if( !impl_->top_horizon() || !impl_->bottom_horizon() )
        {
            Logger::warn(
                "[HorizonsStack::bottom_to_top_units] Iteration "
                "on HorizonsStack will be empty: top and bottom "
                "horizons have not been computed, or stack is empty" );
        }
        return { *this, true };
    }

    template < index_t dimension >
    auto HorizonsStack< dimension >::top_to_bottom_horizons() const
        -> HorizonOrderedRange
    {
        if( !impl_->top_horizon() || !impl_->bottom_horizon() )
        {
            Logger::warn(
                "[HorizonsStack::top_to_bottom_horizons] Iteration "
                "on HorizonsStack will be empty: top and bottom "
                "horizons have not been computed, or stack is empty." );
        }
        return { *this, false };
    }

    template < index_t dimension >
    auto HorizonsStack< dimension >::top_to_bottom_units() const
        -> StratigraphicUnitOrderedRange
    {
        if( !impl_->top_horizon() || !impl_->bottom_horizon() )
        {
            Logger::warn(
                "[HorizonsStack::top_to_bottom_units] Iteration "
                "on HorizonsStack will be empty: top and bottom "
                "horizons have not been computed, or stack is empty" );
        }
        return { *this, false };
    }

    template < index_t dimension >
    bool HorizonsStack< dimension >::is_eroded_by(
        const StratigraphicUnit< dimension >& eroded,
        const Horizon< dimension >& erosion ) const
    {
        return StratigraphicRelationships::is_eroded_by(
            eroded.id(), erosion.id() );
    }

    template < index_t dimension >
    bool HorizonsStack< dimension >::is_baselap_of(
        const Horizon< dimension >& baselap,
        const StratigraphicUnit< dimension >& baselap_top ) const
    {
        return StratigraphicRelationships::is_baselap_of(
            baselap.id(), baselap_top.id() );
    }

    template < index_t dimension >
    void HorizonsStack< dimension >::compute_top_and_bottom_horizons(
        HorizonsStackBuilderKey /*unused*/ )
    {
        if( this->nb_horizons() == 0 )
        {
            Logger::warn(
                "[HorizonsStack::compute_top_and_bottom_horizons] No horizons "
                "were provided in the HorizonsStack, top and bottom horizons "
                "will be set to std::nullopt." );
            return;
        }
        auto current_horizon_id = ( *this->horizons().begin() ).id();
        while( const auto su_above = this->above( current_horizon_id ) )
        {
            const auto horizon_above = this->above( su_above.value() );
            if( !horizon_above )
            {
                break;
            }
            current_horizon_id = horizon_above.value();
        }
        impl_->set_top_horizon( current_horizon_id );
        while( const auto su_under = this->under( current_horizon_id ) )
        {
            const auto horizon_under = this->under( su_under.value() );
            if( !horizon_under )
            {
                break;
            }
            current_horizon_id = horizon_under.value();
        }
        impl_->set_bottom_horizon( current_horizon_id );
    }

    template < index_t dimension >
    void HorizonsStack< dimension >::set_top_horizon(
        const uuid& horizon_id, HorizonsStackBuilderKey /*unused*/ )
    {
        impl_->set_top_horizon( horizon_id );
    }

    template < index_t dimension >
    void HorizonsStack< dimension >::set_bottom_horizon(
        const uuid& horizon_id, HorizonsStackBuilderKey /*unused*/ )
    {
        impl_->set_bottom_horizon( horizon_id );
    }

    template < index_t dimension >
    class HorizonsStack< dimension >::HorizonOrderedRange::Impl
    {
    public:
        Impl( const HorizonsStack< dimension >& stack, bool bottom_to_top )
            : stack_( stack ), bottom_to_top_( bottom_to_top )
        {
            auto bot_horizon = stack.bottom_horizon();
            auto top_horizon = stack.top_horizon();
            if( bot_horizon && top_horizon )
            {
                iter_ =
                    bottom_to_top_ ? bot_horizon.value() : top_horizon.value();
            }
        }

        constexpr bool operator!=( const Impl& /*unused*/ ) const
        {
            return stack_.has_horizon( iter_ );
        }

        void operator++()
        {
            if( bottom_to_top_ )
            {
                if( iter_ != stack_.top_horizon().value() )
                {
                    iter_ =
                        stack_.above( stack_.above( iter_ ).value() ).value();
                    return;
                }
            }
            else
            {
                if( iter_ != stack_.bottom_horizon().value() )
                {
                    iter_ =
                        stack_.under( stack_.under( iter_ ).value() ).value();
                    return;
                }
            }
            iter_ = uuid{};
        }

        const Horizon< dimension >& current_horizon() const
        {
            return stack_.horizon( iter_ );
        }

    private:
        const HorizonsStack< dimension >& stack_;
        bool bottom_to_top_;
        uuid iter_{};
    };

    template < index_t dimension >
    HorizonsStack< dimension >::HorizonOrderedRange::HorizonOrderedRange(
        const HorizonsStack& horizons_stack, bool bottom_to_top )
        : impl_( horizons_stack, bottom_to_top )
    {
    }

    template < index_t dimension >
    HorizonsStack< dimension >::HorizonOrderedRange::HorizonOrderedRange(
        HorizonOrderedRange&& ) noexcept = default;

    template < index_t dimension >
    HorizonsStack< dimension >::HorizonOrderedRange::HorizonOrderedRange(
        const HorizonOrderedRange& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    HorizonsStack< dimension >::HorizonOrderedRange::~HorizonOrderedRange() =
        default;

    template < index_t dimension >
    bool HorizonsStack< dimension >::HorizonOrderedRange::operator!=(
        const HorizonOrderedRange& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void HorizonsStack< dimension >::HorizonOrderedRange::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    const Horizon< dimension >&
        HorizonsStack< dimension >::HorizonOrderedRange::operator*() const
    {
        return impl_->current_horizon();
    }

    template < index_t dimension >
    class HorizonsStack< dimension >::StratigraphicUnitOrderedRange::Impl
    {
    public:
        Impl( const HorizonsStack< dimension >& stack, bool bottom_to_top )
            : stack_( stack ), bottom_to_top_( bottom_to_top )
        {
            auto bot_horizon = stack.bottom_horizon();
            auto top_horizon = stack.top_horizon();
            if( bot_horizon && top_horizon )
            {
                iter_ = bottom_to_top_
                            ? stack.under( bot_horizon.value() ).value()
                            : stack.above( top_horizon.value() ).value();
            }
        }

        constexpr bool operator!=( const Impl& /*unused*/ ) const
        {
            return stack_.has_stratigraphic_unit( iter_ );
        }

        void operator++()
        {
            if( bottom_to_top_ )
            {
                if( iter_
                    != stack_.above( stack_.top_horizon().value() ).value() )
                {
                    iter_ =
                        stack_.above( stack_.above( iter_ ).value() ).value();
                    return;
                }
            }
            else
            {
                if( iter_
                    != stack_.under( stack_.bottom_horizon().value() ).value() )
                {
                    iter_ =
                        stack_.under( stack_.under( iter_ ).value() ).value();
                    return;
                }
            }
            iter_ = uuid{};
        }

        const StratigraphicUnit< dimension >& current_stratigraphic_unit() const
        {
            return stack_.stratigraphic_unit( iter_ );
        }

    private:
        const HorizonsStack< dimension >& stack_;
        bool bottom_to_top_;
        uuid iter_{};
    };

    template < index_t dimension >
    HorizonsStack< dimension >::StratigraphicUnitOrderedRange::
        StratigraphicUnitOrderedRange(
            const HorizonsStack& horizons_stack, bool bottom_to_top )
        : impl_( horizons_stack, bottom_to_top )
    {
    }

    template < index_t dimension >
    HorizonsStack< dimension >::StratigraphicUnitOrderedRange::
        StratigraphicUnitOrderedRange(
            StratigraphicUnitOrderedRange&& ) noexcept = default;

    template < index_t dimension >
    HorizonsStack< dimension >::StratigraphicUnitOrderedRange::
        StratigraphicUnitOrderedRange(
            const StratigraphicUnitOrderedRange& other )
        : impl_( *other.impl_ )
    {
    }

    template < index_t dimension >
    HorizonsStack< dimension >::StratigraphicUnitOrderedRange::
        ~StratigraphicUnitOrderedRange() = default;

    template < index_t dimension >
    bool HorizonsStack< dimension >::StratigraphicUnitOrderedRange::operator!=(
        const StratigraphicUnitOrderedRange& /*unused*/ ) const
    {
        return impl_->operator!=( *impl_ );
    }

    template < index_t dimension >
    void HorizonsStack< dimension >::StratigraphicUnitOrderedRange::operator++()
    {
        return impl_->operator++();
    }

    template < index_t dimension >
    const StratigraphicUnit< dimension >&
        HorizonsStack< dimension >::StratigraphicUnitOrderedRange::operator*()
            const
    {
        return impl_->current_stratigraphic_unit();
    }

    template class opengeode_geosciences_implicit_api HorizonsStack< 2 >;
    template class opengeode_geosciences_implicit_api HorizonsStack< 3 >;
} // namespace geode