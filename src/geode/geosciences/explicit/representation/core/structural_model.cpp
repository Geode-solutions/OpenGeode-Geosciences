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

#include <geode/geosciences/explicit/representation/core/structural_model.h>

namespace geode
{
    StructuralModel::HorizonItemRange::HorizonItemRange(
        const StructuralModel& structural_model, const Horizon3D& horizon )
        : Relationships::ItemRangeIterator( structural_model, horizon.id() ),
          structural_model_( structural_model )
    {
    }

    StructuralModel::HorizonItemRange::HorizonItemRange(
        const HorizonItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          structural_model_( range.structural_model_ )
    {
    }

    StructuralModel::HorizonItemRange::~HorizonItemRange() {}

    auto StructuralModel::HorizonItemRange::begin() const
        -> const HorizonItemRange&
    {
        return *this;
    }

    auto StructuralModel::HorizonItemRange::end() const
        -> const HorizonItemRange&
    {
        return *this;
    }

    const Surface3D& StructuralModel::HorizonItemRange::operator*() const
    {
        return structural_model_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    StructuralModel::HorizonItemRange StructuralModel::horizon_items(
        const Horizon3D& horizon ) const
    {
        return { *this, horizon };
    }

    StructuralModel::FaultItemRange::FaultItemRange(
        const StructuralModel& structural_model, const Fault3D& fault )
        : Relationships::ItemRangeIterator( structural_model, fault.id() ),
          structural_model_( structural_model )
    {
    }

    StructuralModel::FaultItemRange::FaultItemRange(
        const FaultItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          structural_model_( range.structural_model_ )
    {
    }

    StructuralModel::FaultItemRange::~FaultItemRange() {}

    auto StructuralModel::FaultItemRange::begin() const -> const FaultItemRange&
    {
        return *this;
    }

    auto StructuralModel::FaultItemRange::end() const -> const FaultItemRange&
    {
        return *this;
    }

    const Surface3D& StructuralModel::FaultItemRange::operator*() const
    {
        return structural_model_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    StructuralModel::FaultItemRange StructuralModel::fault_items(
        const Fault3D& fault ) const
    {
        return { *this, fault };
    }

    StructuralModel::FaultBlockItemRange::FaultBlockItemRange(
        const StructuralModel& structural_model,
        const FaultBlock3D& fault_block )
        : Relationships::ItemRangeIterator(
            structural_model, fault_block.id() ),
          structural_model_( structural_model )
    {
    }

    StructuralModel::FaultBlockItemRange::FaultBlockItemRange(
        const FaultBlockItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          structural_model_( range.structural_model_ )
    {
    }

    StructuralModel::FaultBlockItemRange::~FaultBlockItemRange() {}

    auto StructuralModel::FaultBlockItemRange::begin() const
        -> const FaultBlockItemRange&
    {
        return *this;
    }

    auto StructuralModel::FaultBlockItemRange::end() const
        -> const FaultBlockItemRange&
    {
        return *this;
    }

    const Block3D& StructuralModel::FaultBlockItemRange::operator*() const
    {
        return structural_model_.block(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    StructuralModel::FaultBlockItemRange StructuralModel::fault_block_items(
        const FaultBlock3D& fault_block ) const
    {
        return { *this, fault_block };
    }

    StructuralModel::StratigraphicUnitItemRange::StratigraphicUnitItemRange(
        const StructuralModel& structural_model,
        const StratigraphicUnit3D& stratigraphic_unit )
        : Relationships::ItemRangeIterator(
            structural_model, stratigraphic_unit.id() ),
          structural_model_( structural_model )
    {
    }

    StructuralModel::StratigraphicUnitItemRange::StratigraphicUnitItemRange(
        const StratigraphicUnitItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          structural_model_( range.structural_model_ )
    {
    }

    StructuralModel::StratigraphicUnitItemRange::~StratigraphicUnitItemRange()
    {
    }

    auto StructuralModel::StratigraphicUnitItemRange::begin() const
        -> const StratigraphicUnitItemRange&
    {
        return *this;
    }

    auto StructuralModel::StratigraphicUnitItemRange::end() const
        -> const StratigraphicUnitItemRange&
    {
        return *this;
    }

    const Block3D&
        StructuralModel::StratigraphicUnitItemRange::operator*() const
    {
        return structural_model_.block(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    StructuralModel::StratigraphicUnitItemRange
        StructuralModel::stratigraphic_unit_items(
            const StratigraphicUnit3D& stratigraphic_unit ) const
    {
        return { *this, stratigraphic_unit };
    }

    StructuralModel::StructuralModel( BRep&& brep ) : BRep{ std::move( brep ) }
    {
    }
} // namespace geode