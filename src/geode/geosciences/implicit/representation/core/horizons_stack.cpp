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

#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

#include <geode/geosciences/explicit/representation/core/detail/clone.h>
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.h>

namespace geode
{
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
        clone_mapping.emplace(
            geode::Horizon< dimension >::component_type_static(),
            detail::clone_horizon_mapping( *this ) );
        clone_mapping.emplace(
            geode::StratigraphicUnit< dimension >::component_type_static(),
            detail::clone_stratigraphic_unit_mapping( *this ) );
        clone_builder.copy( clone_mapping, *this );
        return stack_clone;
    }

    template < index_t dimension >
    uuid HorizonsStack< dimension >::top_horizon() const
    {
        OPENGEODE_EXCEPTION( this->nb_horizons() != 0,
            "[HorizonsStack::top_horizon] Cannot determine top horizon: "
            "no horizons were provided in the HorizonsStack." );
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
        return current_horizon_id;
    }

    template < index_t dimension >
    uuid HorizonsStack< dimension >::bottom_horizon() const
    {
        OPENGEODE_EXCEPTION( this->nb_horizons() != 0,
            "[HorizonsStack::bottom_horizon] Cannot determine bottom horizon: "
            "no horizons were provided in the HorizonsStack." );
        auto current_horizon_id = ( *this->horizons().begin() ).id();
        while( const auto su_under = this->under( current_horizon_id ) )
        {
            const auto horizon_under = this->under( su_under.value() );
            if( !horizon_under )
            {
                break;
            }
            current_horizon_id = horizon_under.value();
        }
        return current_horizon_id;
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

    template class opengeode_geosciences_implicit_api HorizonsStack< 2 >;
    template class opengeode_geosciences_implicit_api HorizonsStack< 3 >;
} // namespace geode