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

#include <geode/model/representation/core/section.h>

#include <geode/geosciences/mixin/core/fault_blocks.h>
#include <geode/geosciences/mixin/core/faults.h>
#include <geode/geosciences/mixin/core/horizons.h>
#include <geode/geosciences/mixin/core/stratigraphic_units.h>

namespace geode
{
    /*!
     * A Cross Section is a Section composed of Faults and Horizons (as Lines)
     * and FaultBlocks and StratigraphicUnits (as Surfaces).
     */
    class opengeode_geosciences_geosciences_api CrossSection
        : public Section,
          public AddComponents< 2,
              Faults,
              Horizons,
              FaultBlocks,
              StratigraphicUnits >
    {
    public:
        class opengeode_geosciences_geosciences_api HorizonItemRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< HorizonItemRange >
        {
        public:
            HorizonItemRange(
                const CrossSection& cross_section, const Horizon2D& horizon );

            const Line2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        class opengeode_geosciences_geosciences_api FaultItemRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< FaultItemRange >
        {
        public:
            FaultItemRange(
                const CrossSection& cross_section, const Fault2D& fault );

            const Line2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        class opengeode_geosciences_geosciences_api FaultBlockItemRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< FaultBlockItemRange >
        {
        public:
            FaultBlockItemRange( const CrossSection& cross_section,
                const FaultBlock2D& fault_block );

            const Surface2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        class opengeode_geosciences_geosciences_api StratigraphicUnitItemRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< StratigraphicUnitItemRange >
        {
        public:
            StratigraphicUnitItemRange( const CrossSection& cross_section,
                const StratigraphicUnit2D& stratigraphic_unit );

            const Surface2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

    public:
        CrossSection() = default;
        CrossSection( CrossSection&& ) = default;
        CrossSection( Section&& section );

        static constexpr absl::string_view native_extension_static()
        {
            return "og_xsctn";
        }

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        HorizonItemRange horizon_items( const Horizon2D& horizon ) const;

        FaultItemRange fault_items( const Fault2D& fault ) const;

        FaultBlockItemRange fault_block_items(
            const FaultBlock2D& fault_block ) const;

        StratigraphicUnitItemRange stratigraphic_unit_items(
            const StratigraphicUnit2D& stratigraphic_unit ) const;
    };
} // namespace geode
