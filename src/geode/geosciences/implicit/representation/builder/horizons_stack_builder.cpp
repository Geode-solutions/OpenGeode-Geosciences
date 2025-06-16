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

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>

#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>
#include <geode/geosciences/explicit/representation/builder/detail/copy.hpp>

#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

namespace geode
{
    template < index_t dimension >
    HorizonsStackBuilder< dimension >::HorizonsStackBuilder(
        HorizonsStack< dimension >& horizons_stack )
        : StratigraphicRelationshipsBuilder( horizons_stack ),
          HorizonsBuilder< dimension >( horizons_stack ),
          StratigraphicUnitsBuilder< dimension >( horizons_stack ),
          IdentifierBuilder( horizons_stack ),
          horizons_stack_( horizons_stack )
    {
    }

    template < index_t dimension >
    ModelCopyMapping HorizonsStackBuilder< dimension >::copy(
        const HorizonsStack< dimension >& horizons_stack )
    {
        const auto nb_components = horizons_stack_.nb_horizons()
                                   + horizons_stack_.nb_stratigraphic_units();
        OPENGEODE_EXCEPTION( nb_components == 0,
            "[HorizonsStackBuild::copy] HorizonsStack "
            "should be empty before copy." );
        set_name( horizons_stack.name() );
        auto mapping = copy_components( horizons_stack );
        copy_stratigraphic_relationships( mapping, horizons_stack );
        return mapping;
    }

    template < index_t dimension >
    ModelCopyMapping HorizonsStackBuilder< dimension >::copy_components(
        const HorizonsStack< dimension >& horizons_stack )
    {
        ModelCopyMapping mappings;
        detail::copy_horizons( horizons_stack, *this,
            mappings[Horizon< dimension >::component_type_static()] );
        detail::copy_stratigraphic_units( horizons_stack, *this,
            mappings[StratigraphicUnit< dimension >::component_type_static()] );
        return mappings;
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::copy( ModelCopyMapping& mapping,
        const HorizonsStack< dimension >& horizons_stack )
    {
        const auto nb_components = horizons_stack_.nb_horizons()
                                   + horizons_stack_.nb_stratigraphic_units();
        OPENGEODE_EXCEPTION( nb_components == 0,
            "[HorizonsStackBuild::copy] HorizonsStack "
            "should be empty before copy." );
        set_name( horizons_stack.name() );
        copy_components( mapping, horizons_stack );
        copy_stratigraphic_relationships( mapping, horizons_stack );
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::copy_components(
        ModelCopyMapping& mapping,
        const HorizonsStack< dimension >& horizons_stack )
    {
        detail::copy_horizons( horizons_stack, *this,
            mapping[Horizon< dimension >::component_type_static()] );
        detail::copy_stratigraphic_units( horizons_stack, *this,
            mapping[StratigraphicUnit< dimension >::component_type_static()] );
    }

    template < index_t dimension >
    const uuid& HorizonsStackBuilder< dimension >::add_horizon()
    {
        return this->create_horizon();
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::add_horizon( uuid horizon_id )
    {
        this->create_horizon( horizon_id );
    }

    template < index_t dimension >
    const uuid& HorizonsStackBuilder< dimension >::add_stratigraphic_unit()
    {
        return this->create_stratigraphic_unit();
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::add_stratigraphic_unit(
        uuid stratigraphic_unit_id )
    {
        this->create_stratigraphic_unit( stratigraphic_unit_id );
    }

    template < index_t dimension >
    typename HorizonsStackBuilder< dimension >::InsertedHorizonInfo
        HorizonsStackBuilder< dimension >::add_horizon_in_stratigraphic_unit(
            const StratigraphicUnit< dimension >& strati_unit )
    {
        OPENGEODE_EXCEPTION(
            horizons_stack_.has_stratigraphic_unit( strati_unit.id() ),
            "[HorizonsStackBuilder::add_horizon_in_stratigraphic_"
            "unit] Cannot add horizon: statigraphic unit ",
            strati_unit.component_id().string() );
        InsertedHorizonInfo info;
        info.new_horizon_id = this->create_horizon();
        info.strati_unit_above_id = this->create_stratigraphic_unit();
        info.strati_unit_under_id = this->create_stratigraphic_unit();
        this->add_above_relation(
            { StratigraphicUnit< dimension >::component_type_static(),
                info.strati_unit_above_id },
            { Horizon< dimension >::component_type_static(),
                info.new_horizon_id } );
        this->add_above_relation(
            { Horizon< dimension >::component_type_static(),
                info.new_horizon_id },
            { StratigraphicUnit< dimension >::component_type_static(),
                info.strati_unit_under_id } );
        const auto above = horizons_stack_.above( strati_unit.id() );
        if( above )
        {
            this->remove_above_relation( above.value(), strati_unit.id() );
            this->add_above_relation(
                { Horizon< dimension >::component_type_static(),
                    above.value() },
                { StratigraphicUnit< dimension >::component_type_static(),
                    info.strati_unit_above_id } );
        }
        const auto under = horizons_stack_.under( strati_unit.id() );
        if( under )
        {
            this->remove_above_relation( under.value(), strati_unit.id() );
            this->add_above_relation(
                { StratigraphicUnit< dimension >::component_type_static(),
                    info.strati_unit_under_id },
                { Horizon< dimension >::component_type_static(),
                    under.value() } );
        }
        this->remove_stratigraphic_unit( strati_unit );
        return info;
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::remove_horizon(
        const Horizon< dimension >& horizon )
    {
        /// Change top/bottom horizon or recompute ?
        unregister_component( horizon.id() );
        this->delete_horizon( horizon );
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::remove_stratigraphic_unit(
        const StratigraphicUnit< dimension >& stratigraphic_unit )
    {
        unregister_component( stratigraphic_unit.id() );
        this->delete_stratigraphic_unit( stratigraphic_unit );
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::set_horizon_above(
        const Horizon< dimension >& horizon_above,
        const StratigraphicUnit< dimension >& strati_unit_under )
    {
        StratigraphicRelationshipsBuilder::add_above_relation(
            horizon_above.component_id(), strati_unit_under.component_id() );
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::set_horizon_under(
        const Horizon< dimension >& horizon_under,
        const StratigraphicUnit< dimension >& strati_unit_above )
    {
        StratigraphicRelationshipsBuilder::add_above_relation(
            strati_unit_above.component_id(), horizon_under.component_id() );
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::set_horizon_type(
        const Horizon< dimension >& horizon,
        typename HorizonsStack< dimension >::HORIZON_TYPE type )
    {
        if( type == HorizonsStack< dimension >::HORIZON_TYPE::conformal )
        {
            return;
        }
        if( type == HorizonsStack< dimension >::HORIZON_TYPE::erosion
            || type == HorizonsStack< dimension >::HORIZON_TYPE::discontinuity )
        {
            const auto& unit_under_id = horizons_stack_.under( horizon.id() );
            if( unit_under_id )
            {
                StratigraphicRelationshipsBuilder::add_unconformity_relation(
                    horizon.component_id(),
                    horizons_stack_.stratigraphic_unit( unit_under_id.value() )
                        .component_id(),
                    type );
            }
            else
            {
                geode::Logger::warn(
                    "[HorizonsStackBuilder::set_horizon_type] Could not find "
                    "unit under horizon ",
                    horizon.id().string(),
                    ", cannot set erosional contact between the two." );
            }
        }
        if( type == HorizonsStack< dimension >::HORIZON_TYPE::baselap
            || type == HorizonsStack< dimension >::HORIZON_TYPE::discontinuity )
        {
            const auto& unit_above_id = horizons_stack_.above( horizon.id() );
            if( unit_above_id )
            {
                StratigraphicRelationshipsBuilder::add_unconformity_relation(
                    horizon.component_id(),
                    horizons_stack_.stratigraphic_unit( unit_above_id.value() )
                        .component_id(),
                    type );
            }
            else
            {
                geode::Logger::warn(
                    "[HorizonsStackBuilder::set_horizon_type] Could not find "
                    "unit above horizon ",
                    horizon.id().string(),
                    ", cannot set baselap contact between the two." );
            }
        }
    }

    template < index_t dimension >
    void HorizonsStackBuilder< dimension >::compute_top_and_bottom_horizons()
    {
        horizons_stack_.compute_top_and_bottom_horizons( {} );
    }

    template class opengeode_geosciences_implicit_api HorizonsStackBuilder< 2 >;
    template class opengeode_geosciences_implicit_api HorizonsStackBuilder< 3 >;
} // namespace geode
