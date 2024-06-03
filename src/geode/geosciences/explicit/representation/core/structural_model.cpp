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

#include <geode/geosciences/explicit/representation/core/structural_model.h>

#include <geode/model/representation/core/detail/clone.h>

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.h>
#include <geode/geosciences/explicit/representation/core/detail/clone.h>

namespace
{
    void copy_faults_items_from_mappings(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& current_model,
        geode::StructuralModelBuilder& builder,
        const geode::GenericMapping< geode::uuid >& surface_mappings ) noexcept
    {
        for( const auto& fault : initial_model.faults() )
        {
            const auto& this_fault = current_model.fault( fault.id() );
            for( const auto& fault_surface :
                initial_model.fault_items( fault ) )
            {
                if( !surface_mappings.has_mapping_input( fault_surface.id() ) )
                {
                    continue;
                }
                for( const auto& out_surface_id :
                    surface_mappings.in2out( fault_surface.id() ) )
                {
                    builder.add_surface_in_fault(
                        current_model.surface( out_surface_id ), this_fault );
                }
            }
        }
    }

    void copy_horizons_items_from_mappings(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& current_model,
        geode::StructuralModelBuilder& builder,
        const geode::GenericMapping< geode::uuid >& surface_mappings ) noexcept
    {
        for( const auto& horizon : initial_model.horizons() )
        {
            const auto& this_horizon = current_model.horizon( horizon.id() );
            for( const auto& horizon_surface :
                initial_model.horizon_items( horizon ) )
            {
                if( !surface_mappings.has_mapping_input(
                        horizon_surface.id() ) )
                {
                    continue;
                }
                for( const auto& out_surface_id :
                    surface_mappings.in2out( horizon_surface.id() ) )
                {
                    builder.add_surface_in_horizon(
                        current_model.surface( out_surface_id ), this_horizon );
                }
            }
        }
    }

    void copy_fault_blocks_items_from_mappings(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& current_model,
        geode::StructuralModelBuilder& builder,
        const geode::GenericMapping< geode::uuid >& block_mappings ) noexcept
    {
        for( const auto& fault_block : initial_model.fault_blocks() )
        {
            const auto& this_fault_block =
                current_model.fault_block( fault_block.id() );
            for( const auto& fault_block_block :
                initial_model.fault_block_items( fault_block ) )
            {
                if( !block_mappings.has_mapping_input(
                        fault_block_block.id() ) )
                {
                    continue;
                }
                for( const auto& out_block_id :
                    block_mappings.in2out( fault_block_block.id() ) )
                {
                    builder.add_block_in_fault_block(
                        current_model.block( out_block_id ), this_fault_block );
                }
            }
        }
    }

    void copy_stratigraphic_units_items_from_mappings(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& current_model,
        geode::StructuralModelBuilder& builder,
        const geode::GenericMapping< geode::uuid >& block_mappings ) noexcept
    {
        for( const auto& stratigraphic_unit :
            initial_model.stratigraphic_units() )
        {
            const auto& this_stratigraphic_unit =
                current_model.stratigraphic_unit( stratigraphic_unit.id() );
            for( const auto& stratigraphic_unit_block :
                initial_model.stratigraphic_unit_items( stratigraphic_unit ) )
            {
                if( !block_mappings.has_mapping_input(
                        stratigraphic_unit_block.id() ) )
                {
                    continue;
                }
                for( const auto& out_block_id :
                    block_mappings.in2out( stratigraphic_unit_block.id() ) )
                {
                    builder.add_block_in_stratigraphic_unit(
                        current_model.block( out_block_id ),
                        this_stratigraphic_unit );
                }
            }
        }
    }
} // namespace

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

    StructuralModel::HorizonItemRange::~HorizonItemRange() = default;

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

    StructuralModel::FaultItemRange::~FaultItemRange() = default;

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

    StructuralModel::FaultBlockItemRange::~FaultBlockItemRange() = default;

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

    StructuralModel::StratigraphicUnitItemRange::~StratigraphicUnitItemRange() =
        default;

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

    StructuralModel::StructuralModel( BRep&& brep ) noexcept
        : BRep{ std::move( brep ) }
    {
    }

    StructuralModel::StructuralModel( const StructuralModel& initial_model,
        BRep&& brep,
        const ModelGenericMapping& initial_to_brep_mappings ) noexcept
        : BRep{ std::move( brep ) }
    {
        StructuralModelBuilder builder{ *this };
        ModelCopyMapping mappings;
        detail::add_geology_clone_mapping( mappings, initial_model );
        builder.copy_geological_components( mappings, initial_model );
        if( initial_to_brep_mappings.has_mapping_type(
                Surface3D::component_type_static() ) )
        {
            const auto& surface_mappings = initial_to_brep_mappings.at(
                Surface3D::component_type_static() );
            copy_faults_items_from_mappings(
                initial_model, *this, builder, surface_mappings );
            copy_horizons_items_from_mappings(
                initial_model, *this, builder, surface_mappings );
        }
        if( initial_to_brep_mappings.has_mapping_type(
                Block3D::component_type_static() ) )
        {
            const auto& block_mappings =
                initial_to_brep_mappings.at( Block3D::component_type_static() );
            copy_fault_blocks_items_from_mappings(
                initial_model, *this, builder, block_mappings );
            copy_stratigraphic_units_items_from_mappings(
                initial_model, *this, builder, block_mappings );
        }
    }

    StructuralModel StructuralModel::clone() const
    {
        StructuralModel model_clone;
        StructuralModelBuilder clone_builder{ model_clone };
        clone_builder.copy_identifier( *this );
        auto mappings = detail::brep_clone_mapping( *this );
        detail::add_geology_clone_mapping( mappings, *this );
        clone_builder.copy_components( mappings, *this );
        clone_builder.copy_component_geometry( mappings, *this );
        clone_builder.copy_geological_components( mappings, *this );
        clone_builder.copy_relationships( mappings, *this );
        return model_clone;
    }
} // namespace geode