/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStack );
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStackBuilder );
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a StratigraphicUnitsStack.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] stratigraphic_units_stack StratigraphicUnitsStack to save.
     * @param[in] filename Path to the file where save the brep.
     */
    template < index_t dimension >
    void save_stratigraphic_units_stack(
        const StratigraphicUnitsStack< dimension >& stratigraphic_units_stack,
        absl::string_view filename );

    template < index_t dimension >
    class StratigraphicUnitsStackOutput
        : public Output< StratigraphicUnitsStack< dimension > >
    {
    protected:
        StratigraphicUnitsStackOutput( absl::string_view filename )
            : Output< StratigraphicUnitsStack< dimension > >{ filename }
        {
        }
    };
    ALIAS_2D_AND_3D( StratigraphicUnitsStackOutput );

    template < index_t dimension >
    using StratigraphicUnitsStackOutputFactory = Factory< std::string,
        StratigraphicUnitsStackOutput< dimension >,
        absl::string_view >;
    ALIAS_2D_AND_3D( StratigraphicUnitsStackOutputFactory );
} // namespace geode
