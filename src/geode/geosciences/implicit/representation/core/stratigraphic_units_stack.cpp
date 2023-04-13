/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>

namespace geode
{
    template < index_t dimension >
    StratigraphicUnitsStack< dimension >::StratigraphicUnitsStack()
    {
    }

    template < index_t dimension >
    StratigraphicUnitsStack< dimension >::StratigraphicUnitsStack(
        StratigraphicUnitsStack< dimension >&& su_stack )
        : StratigraphicRelationships{ std::move( su_stack ) },
          Horizons< dimension >{ std::move( su_stack ) },
          StratigraphicUnits< dimension >{ std::move( su_stack ) },
          Identifier{ std::move( su_stack ) }
    {
    }

    template < index_t dimension >
    StratigraphicUnitsStack< dimension >::~StratigraphicUnitsStack()
    {
    }

    template < index_t dimension >
    StratigraphicUnitsStack< dimension >&
        StratigraphicUnitsStack< dimension >::operator=(
            StratigraphicUnitsStack< dimension >&& other )
    {
        StratigraphicRelationships::operator=( std::move( other ) );
        Horizons< dimension >::operator=( std::move( other ) );
        StratigraphicUnits< dimension >::operator=( std::move( other ) );
        Identifier::operator=( std::move( other ) );
        return *this;
    }

    template class opengeode_geosciences_implicit_api
        StratigraphicUnitsStack< 2 >;
    template class opengeode_geosciences_implicit_api
        StratigraphicUnitsStack< 3 >;
} // namespace geode