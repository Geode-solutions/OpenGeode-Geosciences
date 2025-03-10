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

#include <geode/basic/algorithm.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/corner_collection.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/line_collection.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/mixin/core/surface_collection.hpp>
#include <geode/model/representation/core/mapping.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/geosciences/explicit/mixin/core/fault_blocks.hpp>
#include <geode/geosciences/explicit/mixin/core/faults.hpp>
#include <geode/geosciences/explicit/mixin/core/horizons.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.hpp>

namespace geode
{
    class CrossSectionBuilder;
} // namespace geode

namespace geode
{
    /*!
     * A Cross Section is a Section composed of Faults and Horizons (as Lines)
     * and FaultBlocks and StratigraphicUnits (as Surfaces).
     */
    class opengeode_geosciences_explicit_api CrossSection
        : public Section,
          public Faults2D,
          public Horizons2D,
          public FaultBlocks2D,
          public StratigraphicUnits2D
    {
    public:
        class opengeode_geosciences_explicit_api HorizonItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            HorizonItemRange(
                const CrossSection& cross_section, const Horizon2D& horizon );
            HorizonItemRange( const HorizonItemRange& range );
            ~HorizonItemRange();

            [[nodiscard]] const HorizonItemRange& begin() const;

            [[nodiscard]] const HorizonItemRange& end() const;

            [[nodiscard]] const Line2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        class opengeode_geosciences_explicit_api FaultItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            FaultItemRange(
                const CrossSection& cross_section, const Fault2D& fault );
            FaultItemRange( const FaultItemRange& range );
            ~FaultItemRange();

            [[nodiscard]] const FaultItemRange& begin() const;

            [[nodiscard]] const FaultItemRange& end() const;

            [[nodiscard]] const Line2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        class opengeode_geosciences_explicit_api FaultBlockItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            FaultBlockItemRange( const CrossSection& cross_section,
                const FaultBlock2D& fault_block );
            FaultBlockItemRange( const FaultBlockItemRange& range );
            ~FaultBlockItemRange();

            [[nodiscard]] const FaultBlockItemRange& begin() const;

            [[nodiscard]] const FaultBlockItemRange& end() const;

            [[nodiscard]] const Surface2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        class opengeode_geosciences_explicit_api StratigraphicUnitItemRange
            : public Relationships::ItemRangeIterator
        {
        public:
            StratigraphicUnitItemRange( const CrossSection& cross_section,
                const StratigraphicUnit2D& stratigraphic_unit );
            StratigraphicUnitItemRange(
                const StratigraphicUnitItemRange& range );
            ~StratigraphicUnitItemRange();

            [[nodiscard]] const StratigraphicUnitItemRange& begin() const;

            [[nodiscard]] const StratigraphicUnitItemRange& end() const;

            [[nodiscard]] const Surface2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

    public:
        static constexpr index_t dim{ 2 };
        using Builder = CrossSectionBuilder;
        using CollectionComponents = tuple_cat< Section::CollectionComponents,
            std::tuple< Faults2D,
                Horizons2D,
                FaultBlocks2D,
                StratigraphicUnits2D > >;
        using Components = tuple_cat< MeshComponents, CollectionComponents >;

        CrossSection() = default;
        CrossSection( CrossSection&& ) noexcept = default;
        explicit CrossSection( Section&& section ) noexcept;
        CrossSection( const CrossSection& initial_model,
            Section&& section,
            const ModelGenericMapping& initial_to_section_mappings ) noexcept;

        [[nodiscard]] CrossSection clone() const;

        [[nodiscard]] static constexpr std::string_view
            native_extension_static()
        {
            return "og_xsctn";
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }

        [[nodiscard]] HorizonItemRange horizon_items(
            const Horizon2D& horizon ) const;

        [[nodiscard]] FaultItemRange fault_items( const Fault2D& fault ) const;

        [[nodiscard]] FaultBlockItemRange fault_block_items(
            const FaultBlock2D& fault_block ) const;

        [[nodiscard]] StratigraphicUnitItemRange stratigraphic_unit_items(
            const StratigraphicUnit2D& stratigraphic_unit ) const;
    };
} // namespace geode
