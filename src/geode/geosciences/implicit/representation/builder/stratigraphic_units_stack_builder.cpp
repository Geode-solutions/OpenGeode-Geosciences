/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/builder/stratigraphic_units_stack_builder.h>

#include <geode/geosciences/explicit/representation/builder/detail/copy.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>

namespace geode
{
    template < index_t dimension >
    StratigraphicUnitsStackBuilder< dimension >::StratigraphicUnitsStackBuilder(
        StratigraphicUnitsStack< dimension >& units_stack )
        : StratigraphicRelationshipsBuilder( units_stack ),
          HorizonsBuilder< dimension >( units_stack ),
          StratigraphicUnitsBuilder< dimension >( units_stack ),
          IdentifierBuilder( units_stack ),
          units_stack_( units_stack )
    {
    }

    template < index_t dimension >
    ModelCopyMapping StratigraphicUnitsStackBuilder< dimension >::copy(
        const StratigraphicUnitsStack< dimension >& units_stack )
    {
        const auto nb_components =
            units_stack_.nb_horizons() + units_stack_.nb_stratigraphic_units();
        OPENGEODE_EXCEPTION( nb_components == 0,
            "[StratigraphicUnitsStackBuild::copy] StratigraphicUnitsStack "
            "should be empty before copy." );
        set_name( units_stack.name() );
        const auto mapping = copy_components( units_stack );
        copy_stratigraphic_relationships( mapping, units_stack );
        return mapping;
    }

    template < index_t dimension >
    ModelCopyMapping
        StratigraphicUnitsStackBuilder< dimension >::copy_components(
            const StratigraphicUnitsStack< dimension >& units_stack )
    {
        ModelCopyMapping mappings;
        mappings.emplace( Horizon< dimension >::component_type_static(),
            detail::copy_horizons( units_stack, *this ) );
        mappings.emplace(
            StratigraphicUnit< dimension >::component_type_static(),
            detail::copy_stratigraphic_units( units_stack, *this ) );
        return mappings;
    }

    template < index_t dimension >
    const uuid& StratigraphicUnitsStackBuilder< dimension >::add_horizon()
    {
        return this->create_horizon();
    }

    template < index_t dimension >
    const uuid&
        StratigraphicUnitsStackBuilder< dimension >::add_stratigraphic_unit()
    {
        return this->create_stratigraphic_unit();
    }

    template < index_t dimension >
    void StratigraphicUnitsStackBuilder< dimension >::remove_horizon(
        const Horizon< dimension >& horizon )
    {
        unregister_component( horizon.id() );
        this->delete_horizon( horizon );
    }

    template < index_t dimension >
    void StratigraphicUnitsStackBuilder< dimension >::remove_stratigraphic_unit(
        const StratigraphicUnit< dimension >& stratigraphic_unit )
    {
        unregister_component( stratigraphic_unit.id() );
        this->delete_stratigraphic_unit( stratigraphic_unit );
    }

    template < index_t dimension >
    void StratigraphicUnitsStackBuilder< dimension >::add_horizon_above(
        const Horizon< dimension >& horizon_above,
        const StratigraphicUnit< dimension >& strati_unit_under )
    {
        StratigraphicRelationshipsBuilder::add_above_relation(
            horizon_above.component_id(), strati_unit_under.component_id() );
    }

    template < index_t dimension >
    void StratigraphicUnitsStackBuilder< dimension >::add_horizon_under(
        const Horizon< dimension >& horizon_under,
        const StratigraphicUnit< dimension >& strati_unit_above )
    {
        StratigraphicRelationshipsBuilder::add_above_relation(
            strati_unit_above.component_id(), horizon_under.component_id() );
    }

    template class opengeode_geosciences_implicit_api
        StratigraphicUnitsStackBuilder< 2 >;
    template class opengeode_geosciences_implicit_api
        StratigraphicUnitsStackBuilder< 3 >;
} // namespace geode
