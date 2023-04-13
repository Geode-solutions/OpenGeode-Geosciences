/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/identifier.h>

#include <geode/geosciences/explicit/mixin/core/horizons.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>
#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStackBuilder );
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * A StratigraphicUnitsStack is a set of Horizons and StratigraphicUnits,
     * linked by relations defining how they are stacked (which unit is above
     * and under which horizon, and inversely).
     */
    template < index_t dimension >
    class opengeode_geosciences_implicit_api StratigraphicUnitsStack
        : public StratigraphicRelationships,
          public Horizons< dimension >,
          public StratigraphicUnits< dimension >,
          public Identifier
    {
        PASSKEY( StratigraphicUnitsStackBuilder< dimension >,
            StratigraphicUnitsStackBuilderKey );

    public:
        StratigraphicUnitsStack();
        StratigraphicUnitsStack(
            StratigraphicUnitsStack< dimension >&& su_stack );
        ~StratigraphicUnitsStack();

        StratigraphicUnitsStack< dimension >& operator=(
            StratigraphicUnitsStack< dimension >&& other );

        static absl::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_sus", dimension, "d" );
            return extension;
        }

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }
    };
    ALIAS_2D_AND_3D( StratigraphicUnitsStack );
} // namespace geode