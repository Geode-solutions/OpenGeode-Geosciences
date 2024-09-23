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

#pragma once

#include <geode/basic/identifier_builder.hpp>
#include <geode/basic/mapping.hpp>

#include <geode/geosciences/explicit/mixin/builder/horizons_builder.hpp>
#include <geode/geosciences/explicit/mixin/builder/stratigraphic_units_builder.hpp>
#include <geode/geosciences/explicit/mixin/core/horizons.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.hpp>
#include <geode/geosciences/implicit/common.hpp>
#include <geode/geosciences/implicit/mixin/builder/stratigraphic_relationships_builder.hpp>
#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStack );
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a HorizonsStack
     * @extends HorizonsBuilder
     * @extends StratigraphicUnitsBuilder
     * @extends StratigraphicRelationshipsBuilder
     */
    template < index_t dimension >
    class HorizonsStackBuilder : public StratigraphicRelationshipsBuilder,
                                 public HorizonsBuilder< dimension >,
                                 public StratigraphicUnitsBuilder< dimension >,
                                 public IdentifierBuilder
    {
        OPENGEODE_DISABLE_COPY( HorizonsStackBuilder );

    public:
        struct InsertedHorizonInfo
        {
            uuid new_horizon_id;
            uuid strati_unit_above_id;
            uuid strati_unit_under_id;
        };

    public:
        explicit HorizonsStackBuilder(
            HorizonsStack< dimension >& horizons_stack );
        HorizonsStackBuilder( HorizonsStackBuilder< dimension >&& ) = default;

        ModelCopyMapping copy(
            const HorizonsStack< dimension >& horizons_stack );

        ModelCopyMapping copy_components(
            const HorizonsStack< dimension >& horizons_stack );

        /*!
         * Copies the stack with the component uuids given by the mapping.
         * Components for which the mapping does not exist are created with a
         * new uuid and added to the mapping.
         */
        void copy( ModelCopyMapping& mapping,
            const HorizonsStack< dimension >& horizons_stack );

        /*!
         * Copies the components with the uuid given by the mapping. If the
         * mapping does not exist, creates a component with a new uuid and adds
         * it to the mapping.
         * This does not copy the relations between the components.
         */
        void copy_components( ModelCopyMapping& mapping,
            const HorizonsStack< dimension >& horizons_stack );

        [[nodiscard]] const uuid& add_horizon();

        void add_horizon( uuid horizon_id );

        [[nodiscard]] const uuid& add_stratigraphic_unit();

        void add_stratigraphic_unit( uuid stratigraphic_unit_id );

        InsertedHorizonInfo add_horizon_in_stratigraphic_unit(
            const StratigraphicUnit< dimension >& strati_unit );

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
        HorizonsStack< dimension >& horizons_stack_;
    };
    ALIAS_2D_AND_3D( HorizonsStackBuilder );
} // namespace geode
