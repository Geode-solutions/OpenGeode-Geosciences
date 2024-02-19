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

#include <geode/geosciences/explicit/representation/core/cross_section.h>

#include <geode/model/representation/core/detail/clone.h>

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.h>
#include <geode/geosciences/explicit/representation/core/detail/clone.h>

namespace geode
{
    CrossSection::HorizonItemRange::HorizonItemRange(
        const CrossSection& cross_section, const Horizon2D& horizon )
        : Relationships::ItemRangeIterator( cross_section, horizon.id() ),
          cross_section_( cross_section )
    {
    }
    CrossSection::HorizonItemRange::HorizonItemRange(
        const HorizonItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::HorizonItemRange::~HorizonItemRange() = default;

    auto CrossSection::HorizonItemRange::begin() const
        -> const HorizonItemRange&
    {
        return *this;
    }

    auto CrossSection::HorizonItemRange::end() const -> const HorizonItemRange&
    {
        return *this;
    }

    const Line2D& CrossSection::HorizonItemRange::operator*() const
    {
        return cross_section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::HorizonItemRange CrossSection::horizon_items(
        const Horizon2D& horizon ) const
    {
        return { *this, horizon };
    }

    CrossSection::FaultItemRange::FaultItemRange(
        const CrossSection& cross_section, const Fault2D& fault )
        : Relationships::ItemRangeIterator( cross_section, fault.id() ),
          cross_section_( cross_section )
    {
    }

    CrossSection::FaultItemRange::FaultItemRange( const FaultItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::FaultItemRange::~FaultItemRange() = default;

    auto CrossSection::FaultItemRange::begin() const -> const FaultItemRange&
    {
        return *this;
    }

    auto CrossSection::FaultItemRange::end() const -> const FaultItemRange&
    {
        return *this;
    }

    const Line2D& CrossSection::FaultItemRange::operator*() const
    {
        return cross_section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::FaultItemRange CrossSection::fault_items(
        const Fault2D& fault ) const
    {
        return { *this, fault };
    }

    CrossSection::FaultBlockItemRange::FaultBlockItemRange(
        const CrossSection& cross_section, const FaultBlock2D& fault_block )
        : Relationships::ItemRangeIterator( cross_section, fault_block.id() ),
          cross_section_( cross_section )
    {
    }

    CrossSection::FaultBlockItemRange::FaultBlockItemRange(
        const FaultBlockItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::FaultBlockItemRange::~FaultBlockItemRange() = default;

    auto CrossSection::FaultBlockItemRange::begin() const
        -> const FaultBlockItemRange&
    {
        return *this;
    }

    auto CrossSection::FaultBlockItemRange::end() const
        -> const FaultBlockItemRange&
    {
        return *this;
    }

    const Surface2D& CrossSection::FaultBlockItemRange::operator*() const
    {
        return cross_section_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::FaultBlockItemRange CrossSection::fault_block_items(
        const FaultBlock2D& fault_block ) const
    {
        return { *this, fault_block };
    }

    CrossSection::StratigraphicUnitItemRange::StratigraphicUnitItemRange(
        const CrossSection& cross_section,
        const StratigraphicUnit2D& stratigraphic_unit )
        : Relationships::ItemRangeIterator(
            cross_section, stratigraphic_unit.id() ),
          cross_section_( cross_section )
    {
    }

    CrossSection::StratigraphicUnitItemRange::StratigraphicUnitItemRange(
        const StratigraphicUnitItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::StratigraphicUnitItemRange::~StratigraphicUnitItemRange() =
        default;

    auto CrossSection::StratigraphicUnitItemRange::begin() const
        -> const StratigraphicUnitItemRange&
    {
        return *this;
    }

    auto CrossSection::StratigraphicUnitItemRange::end() const
        -> const StratigraphicUnitItemRange&
    {
        return *this;
    }

    const Surface2D& CrossSection::StratigraphicUnitItemRange::operator*() const
    {
        return cross_section_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::StratigraphicUnitItemRange
        CrossSection::stratigraphic_unit_items(
            const StratigraphicUnit2D& stratigraphic_unit ) const
    {
        return { *this, stratigraphic_unit };
    }

    CrossSection::CrossSection( Section&& section ) noexcept
        : Section{ std::move( section ) }
    {
    }

    CrossSection CrossSection::clone() const
    {
        CrossSection model_clone{ std::move( Section::clone() ) };
        CrossSectionBuilder clone_builder{ model_clone };
        clone_builder.copy_identifier( *this );
        auto mappings = detail::section_clone_mapping( *this );
        detail::add_geology_clone_mapping( mappings, *this );
        clone_builder.copy_components( mappings, *this );
        clone_builder.copy_component_geometry( mappings, *this );
        clone_builder.copy_geological_components( mappings, *this );
        clone_builder.copy_relationships( mappings, *this );
        return model_clone;
    }
} // namespace geode