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

#include <geode/basic/algorithm.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/model_boundary.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/geosciences/explicit/mixin/core/fault_blocks.h>
#include <geode/geosciences/explicit/mixin/core/faults.h>
#include <geode/geosciences/explicit/mixin/core/horizons.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>

namespace geode
{
    class StructuralModelBuilder;
} // namespace geode

namespace geode
{
    /*!
     * A Structural Model is a Boundary Representation composed of Faults and
     * Horizons (as Surfaces) and FaultBlocks and StratigraphicUnits (as
     * Blocks).
     */
    class opengeode_geosciences_explicit_api StructuralModel
        : public BRep,
          public Faults3D,
          public Horizons3D,
          public FaultBlocks3D,
          public StratigraphicUnits3D
    {
    public:
        class opengeode_geosciences_explicit_api HorizonItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            HorizonItemRange( const StructuralModel& structural_model,
                const Horizon3D& horizon );
            HorizonItemRange( const HorizonItemRange& range );
            ~HorizonItemRange();

            const HorizonItemRange& begin() const;

            const HorizonItemRange& end() const;

            const Surface3D& operator*() const;

        private:
            const StructuralModel& structural_model_;
        };

        class opengeode_geosciences_explicit_api FaultItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            FaultItemRange(
                const StructuralModel& structural_model, const Fault3D& fault );
            FaultItemRange( const FaultItemRange& range );
            ~FaultItemRange();

            const FaultItemRange& begin() const;

            const FaultItemRange& end() const;

            const Surface3D& operator*() const;

        private:
            const StructuralModel& structural_model_;
        };

        class opengeode_geosciences_explicit_api FaultBlockItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            FaultBlockItemRange( const StructuralModel& structural_model,
                const FaultBlock3D& fault_block );
            FaultBlockItemRange( const FaultBlockItemRange& range );
            ~FaultBlockItemRange();

            const FaultBlockItemRange& begin() const;

            const FaultBlockItemRange& end() const;

            const Block3D& operator*() const;

        private:
            const StructuralModel& structural_model_;
        };

        class opengeode_geosciences_explicit_api StratigraphicUnitItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            StratigraphicUnitItemRange( const StructuralModel& structural_model,
                const StratigraphicUnit3D& stratigraphic_unit );
            StratigraphicUnitItemRange(
                const StratigraphicUnitItemRange& range );
            ~StratigraphicUnitItemRange();

            const StratigraphicUnitItemRange& begin() const;

            const StratigraphicUnitItemRange& end() const;

            const Block3D& operator*() const;

        private:
            const StructuralModel& structural_model_;
        };

    public:
        static constexpr index_t dim{ 3 };
        using Builder = StructuralModelBuilder;
        using Components = tuple_cat< BRep::Components,
            std::tuple< Fault3D,
                Horizon3D,
                FaultBlock3D,
                StratigraphicUnit3D > >;

        StructuralModel() = default;
        StructuralModel( StructuralModel&& ) noexcept = default;
        StructuralModel( BRep&& brep ) noexcept;

        static constexpr absl::string_view native_extension_static()
        {
            return "og_strm";
        }

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        HorizonItemRange horizon_items( const Horizon3D& horizon ) const;

        FaultItemRange fault_items( const Fault3D& fault ) const;

        FaultBlockItemRange fault_block_items(
            const FaultBlock3D& fault_block ) const;

        StratigraphicUnitItemRange stratigraphic_unit_items(
            const StratigraphicUnit3D& stratigraphic_unit ) const;
    };
} // namespace geode
