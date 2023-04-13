/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/identifier_builder.h>
#include <geode/basic/mapping.h>

#include <geode/geosciences/explicit/mixin/builder/horizons_builder.h>
#include <geode/geosciences/explicit/mixin/builder/stratigraphic_units_builder.h>
#include <geode/geosciences/explicit/mixin/core/horizons.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>
#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/mixin/builder/stratigraphic_relationships_builder.h>
#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStack );
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StratigraphicUnitsStack
     * @extends HorizonsBuilder
     * @extends StratigraphicUnitsBuilder
     * @extends StratigraphicRelationshipsBuilder
     */
    template < index_t dimension >
    class StratigraphicUnitsStackBuilder
        : public StratigraphicRelationshipsBuilder,
          public HorizonsBuilder< dimension >,
          public StratigraphicUnitsBuilder< dimension >,
          public IdentifierBuilder
    {
        OPENGEODE_DISABLE_COPY( StratigraphicUnitsStackBuilder );

    public:
        StratigraphicUnitsStackBuilder(
            StratigraphicUnitsStack< dimension >& units_stack );
        StratigraphicUnitsStackBuilder(
            StratigraphicUnitsStackBuilder< dimension >&& ) = default;

        ModelCopyMapping copy(
            const StratigraphicUnitsStack< dimension >& units_stack );

        ModelCopyMapping copy_components(
            const StratigraphicUnitsStack< dimension >& units_stack );

        const uuid& add_horizon();

        const uuid& add_stratigraphic_unit();

        void add_horizon_above( const Horizon< dimension >& horizon_above,
            const StratigraphicUnit< dimension >& strati_unit_under );

        void add_horizon_under( const Horizon< dimension >& horizon_under,
            const StratigraphicUnit< dimension >& strati_unit_above );

        void remove_horizon( const Horizon< dimension >& horizon );

        void remove_stratigraphic_unit(
            const StratigraphicUnit< dimension >& stratigraphic_unit );

    private:
        StratigraphicUnitsStack< dimension >& units_stack_;
    };
    ALIAS_2D_AND_3D( StratigraphicUnitsStackBuilder );
} // namespace geode
