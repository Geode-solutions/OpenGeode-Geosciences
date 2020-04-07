/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/geosciences/representation/core/cross_section.h>

namespace geode
{
    CrossSection::HorizonItemRange::HorizonItemRange(
        const CrossSection& cross_section, const Horizon2D& horizon )
        : Relationships::ItemRangeIterator( cross_section, horizon.id() ),
          BeginEnd< HorizonItemRange >( *this ),
          cross_section_( cross_section )
    {
    }

    const Line2D& CrossSection::HorizonItemRange::operator*() const
    {
        return cross_section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::HorizonItemRange CrossSection::items(
        const Horizon2D& horizon ) const
    {
        return { *this, horizon };
    }

    CrossSection::FaultItemRange::FaultItemRange(
        const CrossSection& cross_section, const Fault2D& fault )
        : Relationships::ItemRangeIterator( cross_section, fault.id() ),
          BeginEnd< FaultItemRange >( *this ),
          cross_section_( cross_section )
    {
    }

    const Line2D& CrossSection::FaultItemRange::operator*() const
    {
        return cross_section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::FaultItemRange CrossSection::items(
        const Fault2D& fault ) const
    {
        return { *this, fault };
    }

    CrossSection::FaultBlockItemRange::FaultBlockItemRange(
        const CrossSection& cross_section, const FaultBlock2D& fault_block )
        : Relationships::ItemRangeIterator( cross_section, fault_block.id() ),
          BeginEnd< FaultBlockItemRange >( *this ),
          cross_section_( cross_section )
    {
    }

    const Surface2D& CrossSection::FaultBlockItemRange::operator*() const
    {
        return cross_section_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::FaultBlockItemRange CrossSection::items(
        const FaultBlock2D& fault_block ) const
    {
        return { *this, fault_block };
    }

    CrossSection::StratigraphicUnitItemRange::StratigraphicUnitItemRange(
        const CrossSection& cross_section,
        const StratigraphicUnit2D& stratigraphic_unit )
        : Relationships::ItemRangeIterator(
            cross_section, stratigraphic_unit.id() ),
          BeginEnd< StratigraphicUnitItemRange >( *this ),
          cross_section_( cross_section )
    {
    }

    const Surface2D& CrossSection::StratigraphicUnitItemRange::operator*() const
    {
        return cross_section_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::StratigraphicUnitItemRange CrossSection::items(
        const StratigraphicUnit2D& stratigraphic_unit ) const
    {
        return { *this, stratigraphic_unit };
    }
} // namespace geode