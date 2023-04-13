/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStack );
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStackBuilder );
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a StratigraphicUnitsStack.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     * @return Loaded StratigraphicUnitsStack.
     */
    template < index_t dimension >
    StratigraphicUnitsStack< dimension > load_stratigraphic_units_stack(
        absl::string_view filename );

    template < index_t dimension >
    class StratigraphicUnitsStackInput
        : public Input< StratigraphicUnitsStack< dimension > >
    {
    protected:
        StratigraphicUnitsStackInput( absl::string_view filename )
            : Input< StratigraphicUnitsStack< dimension > >{ filename }
        {
        }
    };

    template < index_t dimension >
    using StratigraphicUnitsStackInputFactory = Factory< std::string,
        StratigraphicUnitsStackInput< dimension >,
        absl::string_view >;
    ALIAS_2D_AND_3D( StratigraphicUnitsStackInputFactory );
} // namespace geode
