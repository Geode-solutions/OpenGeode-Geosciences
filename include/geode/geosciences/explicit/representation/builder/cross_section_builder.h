/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/model/representation/builder/section_builder.h>

#include <geode/geosciences/explicit/mixin/builder/fault_blocks_builder.h>
#include <geode/geosciences/explicit/mixin/builder/faults_builder.h>
#include <geode/geosciences/explicit/mixin/builder/horizons_builder.h>
#include <geode/geosciences/explicit/mixin/builder/stratigraphic_units_builder.h>
#include <geode/geosciences/explicit/mixin/core/fault.h>
#include <geode/geosciences/explicit/mixin/core/fault_block.h>
#include <geode/geosciences/explicit/mixin/core/fault_blocks.h>
#include <geode/geosciences/explicit/mixin/core/faults.h>
#include <geode/geosciences/explicit/mixin/core/horizon.h>
#include <geode/geosciences/explicit/mixin/core/horizons.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>

namespace geode
{
    class CrossSection;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a CrossSection
     */
    class opengeode_geosciences_explicit_api CrossSectionBuilder
        : public SectionBuilder,
          public FaultsBuilder2D,
          public HorizonsBuilder2D,
          public FaultBlocksBuilder2D,
          public StratigraphicUnitsBuilder2D
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( CrossSectionBuilder );

    public:
        CrossSectionBuilder( CrossSection& cross_section );

        ModelCopyMapping copy( const CrossSection& cross_section );

        void copy_geological_components(
            ModelCopyMapping& mapping, const CrossSection& cross_section );

        const uuid& add_fault();

        const uuid& add_fault( typename Fault2D::FAULT_TYPE type );

        void add_fault( uuid fault_id );

        void add_fault( uuid fault_id, typename Fault2D::FAULT_TYPE type );

        index_t add_line_in_fault( const Line2D& line, const Fault2D& fault );

        void remove_fault( const Fault2D& fault );

        const uuid& add_horizon();

        const uuid& add_horizon( typename Horizon2D::HORIZON_TYPE type );

        void add_horizon( uuid horizon_id );

        void add_horizon(
            uuid horizon_id, typename Horizon2D::HORIZON_TYPE type );

        index_t add_line_in_horizon(
            const Line2D& line, const Horizon2D& horizon );

        void remove_horizon( const Horizon2D& horizon );

        const uuid& add_fault_block();

        void add_fault_block( uuid fault_block_id );

        index_t add_surface_in_fault_block(
            const Surface2D& surface, const FaultBlock2D& fault_block );

        void remove_fault_block( const FaultBlock2D& fault_block );

        const uuid& add_stratigraphic_unit();

        void add_stratigraphic_unit( uuid stratigraphic_unit_id );

        index_t add_surface_in_stratigraphic_unit( const Surface2D& surface,
            const StratigraphicUnit2D& stratigraphic_unit );

        void remove_stratigraphic_unit(
            const StratigraphicUnit2D& stratigraphic_unit );

    private:
        CrossSection& cross_section_;
    };
} // namespace geode
