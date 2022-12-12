/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/model/representation/builder/brep_builder.h>

#include <geode/geosciences/mixin/builder/fault_blocks_builder.h>
#include <geode/geosciences/mixin/builder/faults_builder.h>
#include <geode/geosciences/mixin/builder/horizons_builder.h>
#include <geode/geosciences/mixin/builder/stratigraphic_units_builder.h>
#include <geode/geosciences/mixin/core/fault.h>
#include <geode/geosciences/mixin/core/fault_block.h>
#include <geode/geosciences/mixin/core/fault_blocks.h>
#include <geode/geosciences/mixin/core/faults.h>
#include <geode/geosciences/mixin/core/horizon.h>
#include <geode/geosciences/mixin/core/horizons.h>
#include <geode/geosciences/mixin/core/stratigraphic_unit.h>
#include <geode/geosciences/mixin/core/stratigraphic_units.h>

namespace geode
{
    class StructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StructuralModel
     */
    class opengeode_geosciences_geosciences_api StructuralModelBuilder
        : public BRepBuilder,
          public FaultsBuilder3D,
          public HorizonsBuilder3D,
          public FaultBlocksBuilder3D,
          public StratigraphicUnitsBuilder3D
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( StructuralModelBuilder );

    public:
        StructuralModelBuilder( StructuralModel& structural_model );

        void copy( const StructuralModel& structural_model );

        void copy_geological_components( ModelCopyMapping& mapping,
            const StructuralModel& structural_model );

        const uuid& add_fault();

        const uuid& add_fault( typename Fault3D::FAULT_TYPE type );

        index_t add_surface_in_fault(
            const Surface3D& surface, const Fault3D& fault );

        void remove_fault( const Fault3D& fault );

        const uuid& add_horizon();

        const uuid& add_horizon( typename Horizon3D::HORIZON_TYPE type );

        index_t add_surface_in_horizon(
            const Surface3D& surface, const Horizon3D& horizon );

        void remove_horizon( const Horizon3D& horizon );

        const uuid& add_fault_block();

        index_t add_block_in_fault_block(
            const Block3D& block, const FaultBlock3D& fault_block );

        void remove_fault_block( const FaultBlock3D& fault_block );

        const uuid& add_stratigraphic_unit();

        index_t add_block_in_stratigraphic_unit( const Block3D& block,
            const StratigraphicUnit3D& stratigraphic_unit );

        void remove_stratigraphic_unit(
            const StratigraphicUnit3D& stratigraphic_unit );

    private:
        StructuralModel& structural_model_;
    };
} // namespace geode
