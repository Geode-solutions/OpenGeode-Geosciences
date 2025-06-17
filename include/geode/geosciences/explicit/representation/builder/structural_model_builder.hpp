/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/model/representation/builder/brep_builder.hpp>

#include <geode/geosciences/explicit/mixin/builder/fault_blocks_builder.hpp>
#include <geode/geosciences/explicit/mixin/builder/faults_builder.hpp>
#include <geode/geosciences/explicit/mixin/builder/horizons_builder.hpp>
#include <geode/geosciences/explicit/mixin/builder/stratigraphic_units_builder.hpp>
#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_blocks.hpp>
#include <geode/geosciences/explicit/mixin/core/faults.hpp>
#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/horizons.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.hpp>

namespace geode
{
    class StructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StructuralModel
     */
    class opengeode_geosciences_explicit_api StructuralModelBuilder
        : public BRepBuilder,
          public FaultsBuilder3D,
          public HorizonsBuilder3D,
          public FaultBlocksBuilder3D,
          public StratigraphicUnitsBuilder3D
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( StructuralModelBuilder );

    public:
        explicit StructuralModelBuilder( StructuralModel& structural_model );

        ModelCopyMapping copy( const StructuralModel& structural_model );

        void copy_geological_components( ModelCopyMapping& mapping,
            const StructuralModel& structural_model );

        [[nodiscard]] const uuid& add_fault();

        [[nodiscard]] const uuid& add_fault(
            typename Fault3D::FAULT_TYPE type );

        void add_fault( const uuid& fault_id );

        void add_fault(
            const uuid& fault_id, typename Fault3D::FAULT_TYPE type );

        index_t add_surface_in_fault(
            const Surface3D& surface, const Fault3D& fault );

        void remove_fault( const Fault3D& fault );

        [[nodiscard]] const uuid& add_horizon();

        [[nodiscard]] const uuid& add_horizon(
            typename Horizon3D::CONTACT_TYPE contact_type );

        void add_horizon( const uuid& horizon_id );

        void add_horizon( const uuid& horizon_id,
            typename Horizon3D::CONTACT_TYPE contact_type );

        index_t add_surface_in_horizon(
            const Surface3D& surface, const Horizon3D& horizon );

        void remove_horizon( const Horizon3D& horizon );

        [[nodiscard]] const uuid& add_fault_block();

        void add_fault_block( const uuid& fault_block_id );

        index_t add_block_in_fault_block(
            const Block3D& block, const FaultBlock3D& fault_block );

        void remove_fault_block( const FaultBlock3D& fault_block );

        [[nodiscard]] const uuid& add_stratigraphic_unit();

        void add_stratigraphic_unit( const uuid& stratigraphic_unit_id );

        index_t add_block_in_stratigraphic_unit( const Block3D& block,
            const StratigraphicUnit3D& stratigraphic_unit );

        void remove_stratigraphic_unit(
            const StratigraphicUnit3D& stratigraphic_unit );

    private:
        StructuralModel& structural_model_;
    };
} // namespace geode
