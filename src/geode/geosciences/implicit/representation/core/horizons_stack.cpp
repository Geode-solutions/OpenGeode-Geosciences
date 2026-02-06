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

#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/model/representation/core/detail/model_component.hpp>

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
    const Component< dimension >& HorizonsStack< dimension >::component(
        const uuid& id ) const
    {
        return detail::model_component( *this, id );
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
        return { *this, RANGEORDER::bottom_to_top };
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
        return { *this, RANGEORDER::bottom_to_top };
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
        return { *this, RANGEORDER::top_to_bottom };
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
        return { *this, RANGEORDER::top_to_bottom };
    }

    template < index_t dimension >
    bool HorizonsStack< dimension >::is_conformal_above(
        const uuid& component ) const
    {
        auto component_is_a_horizon = this->has_horizon( component );
        OPENGEODE_EXCEPTION(
            component_is_a_horizon || this->has_stratigraphic_unit( component ),
            "[HorizonsStack::is_conformal_above] Given component with uuid ",
            component.string(), " does not exist in the stack." );
        if( component_is_a_horizon )
        {
            const auto contact_type = this->horizon( component ).contact_type();
            return contact_type == Horizon< dimension >::CONTACT_TYPE::conformal
                   || contact_type
                          == Horizon< dimension >::CONTACT_TYPE::erosion;
        }
        else
        {
            const auto& horizon_above = this->above( component );
            OPENGEODE_EXCEPTION( horizon_above.has_value(),
                "[HorizonsStack::is_conformal_above] No horizon above given "
                "stratigraphic unit with id ",
                component.string() );
            const auto contact_type =
                this->horizon( horizon_above.value() ).contact_type();
            return contact_type == Horizon< dimension >::CONTACT_TYPE::conformal
                   || contact_type
                          == Horizon< dimension >::CONTACT_TYPE::baselap;
        }
    }

    template < index_t dimension >
    bool HorizonsStack< dimension >::is_conformal_under(
        const uuid& component ) const
    {
        auto component_is_a_horizon = this->has_horizon( component );
        OPENGEODE_EXCEPTION(
            component_is_a_horizon || this->has_stratigraphic_unit( component ),
            "[HorizonsStack::is_conformal_above] Given component with uuid ",
            component.string(), " does not exist in the stack." );
        if( component_is_a_horizon )
        {
            const auto contact_type = this->horizon( component ).contact_type();
            return contact_type == Horizon< dimension >::CONTACT_TYPE::conformal
                   || contact_type
                          == Horizon< dimension >::CONTACT_TYPE::baselap;
        }
        else
        {
            const auto& horizon_under = this->under( component );
            OPENGEODE_EXCEPTION( horizon_under.has_value(),
                "[HorizonsStack::is_conformal_under] No horizon under given "
                "stratigraphic unit with id ",
                component.string() );
            const auto contact_type =
                this->horizon( horizon_under.value() ).contact_type();
            return contact_type == Horizon< dimension >::CONTACT_TYPE::conformal
                   || contact_type
                          == Horizon< dimension >::CONTACT_TYPE::erosion;
        }
    }

    template < index_t dimension >
    std::string HorizonsStack< dimension >::string() const
    {
        auto result = absl::StrCat(
            "HorizonsStack ", this->name(), " from top to bottom:" );
        const auto& top_unit = this->stratigraphic_unit(
            this->above( this->top_horizon().value() ).value() );
        absl::StrAppend( &result, "\n", top_unit.component_id().string(),
            ", named ", top_unit.name() );
        for( const auto& horizon : this->top_to_bottom_horizons() )
        {
            absl::StrAppend( &result, "\n ---", horizon.component_id().string(),
                ", named ", horizon.name(), " ---" );
            if( this->horizon( horizon.id() ).contact_type()
                == Horizon< dimension >::CONTACT_TYPE::erosion )
            {
                absl::StrAppend( &result, "\n -- Erosion --" );
            }
            if( this->horizon( horizon.id() ).contact_type()
                == Horizon< dimension >::CONTACT_TYPE::baselap )
            {
                absl::StrAppend( &result, "\n -- Baselap --" );
            }
            if( this->horizon( horizon.id() ).contact_type()
                == Horizon< dimension >::CONTACT_TYPE::discontinuity )
            {
                absl::StrAppend( &result, "\n -- Discontinuity --" );
            }
            if( this->horizon( horizon.id() ).contact_type()
                == Horizon< dimension >::CONTACT_TYPE::topography )
            {
                absl::StrAppend( &result, "\n -- Topography --" );
            }
            if( this->horizon( horizon.id() ).contact_type()
                == Horizon< dimension >::CONTACT_TYPE::intrusion )
            {
                absl::StrAppend( &result, "\n -- Intrusion --" );
            }
            const auto& under_unit =
                this->stratigraphic_unit( this->under( horizon.id() ).value() );
            absl::StrAppend( &result, "\n", under_unit.component_id().string(),
                ", named ", under_unit.name() );
        }
        return result;
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
        Impl( const HorizonsStack< dimension >& stack, RANGEORDER range_order )
            : stack_( stack ), range_order_( range_order )
        {
            auto bot_horizon = stack.bottom_horizon();
            auto top_horizon = stack.top_horizon();
            if( bot_horizon && top_horizon )
            {
                if( range_order_ == RANGEORDER::bottom_to_top )
                {
                    iter_ = bot_horizon.value();
                }
                else if( range_order_ == RANGEORDER::top_to_bottom )
                {
                    iter_ = top_horizon.value();
                }
            }
        }

        constexpr bool operator!=( const Impl& /*unused*/ ) const
        {
            return stack_.has_horizon( iter_ );
        }

        void operator++()
        {
            if( range_order_ == RANGEORDER::bottom_to_top )
            {
                if( iter_ != stack_.top_horizon().value() )
                {
                    iter_ =
                        stack_.above( stack_.above( iter_ ).value() ).value();
                    return;
                }
            }
            else if( range_order_ == RANGEORDER::top_to_bottom )
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
        RANGEORDER range_order_;
        uuid iter_{};
    };

    template < index_t dimension >
    HorizonsStack< dimension >::HorizonOrderedRange::HorizonOrderedRange(
        const HorizonsStack& horizons_stack, RANGEORDER bottom_to_top )
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
        Impl( const HorizonsStack< dimension >& stack, RANGEORDER range_order )
            : stack_( stack ), range_order_( range_order )
        {
            auto bot_horizon = stack.bottom_horizon();
            auto top_horizon = stack.top_horizon();
            if( bot_horizon && top_horizon )
            {
                if( range_order_ == RANGEORDER::bottom_to_top )
                {
                    iter_ = stack.under( bot_horizon.value() ).value();
                }
                else if( range_order_ == RANGEORDER::top_to_bottom )
                {
                    iter_ = stack.above( top_horizon.value() ).value();
                }
            }
        }

        constexpr bool operator!=( const Impl& /*unused*/ ) const
        {
            return stack_.has_stratigraphic_unit( iter_ );
        }

        void operator++()
        {
            if( range_order_ == RANGEORDER::bottom_to_top )
            {
                if( iter_
                    != stack_.above( stack_.top_horizon().value() ).value() )
                {
                    iter_ =
                        stack_.above( stack_.above( iter_ ).value() ).value();
                    return;
                }
            }
            else if( range_order_ == RANGEORDER::top_to_bottom )
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
        RANGEORDER range_order_;
        uuid iter_{};
    };

    template < index_t dimension >
    HorizonsStack< dimension >::StratigraphicUnitOrderedRange::
        StratigraphicUnitOrderedRange(
            const HorizonsStack& horizons_stack, RANGEORDER bottom_to_top )
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