/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

        void add_erosion_above( const Horizon< dimension >& erosion_horizon,
            const StratigraphicUnit< dimension >& eroded_unit );

        void add_baselap_under( const Horizon< dimension >& baselap_horizon,
            const StratigraphicUnit< dimension >& baselaping_unit );

        void remove_horizon( const Horizon< dimension >& horizon );

        void remove_stratigraphic_unit(
            const StratigraphicUnit< dimension >& stratigraphic_unit );

    private:
        StratigraphicUnitsStack< dimension >& units_stack_;
    };
    ALIAS_2D_AND_3D( StratigraphicUnitsStackBuilder );
} // namespace geode
