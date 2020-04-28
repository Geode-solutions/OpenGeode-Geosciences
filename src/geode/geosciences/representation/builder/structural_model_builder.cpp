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

#include <geode/geosciences/representation/builder/structural_model_builder.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>

#include <geode/geosciences/representation/builder/detail/copy.h>
#include <geode/geosciences/representation/core/structural_model.h>

namespace geode
{
    StructuralModelBuilder::StructuralModelBuilder(
        StructuralModel& structural_model )
        : BRepBuilder( structural_model ),
          AddComponentsBuilders< 3,
              Faults,
              Horizons,
              FaultBlocks,
              StratigraphicUnits >( structural_model ),
          structural_model_( structural_model )
    {
    }

    void StructuralModelBuilder::copy( const StructuralModel& structural_model )
    {
        auto mappings = copy_components( structural_model );
        copy_geological_components( mappings, structural_model );
    }

    void StructuralModelBuilder::copy_geological_components(
        detail::ModelCopyMapping& mappings,
        const StructuralModel& structural_model )
    {
        mappings.emplace( Fault3D::component_type_static(),
            detail::copy_faults( structural_model, structural_model_, *this ) );
        mappings.emplace( Horizon3D::component_type_static(),
            detail::copy_horizons(
                structural_model, structural_model_, *this ) );
        mappings.emplace( FaultBlock3D::component_type_static(),
            detail::copy_fault_blocks(
                structural_model, structural_model_, *this ) );
        mappings.emplace( StratigraphicUnit3D::component_type_static(),
            detail::copy_stratigraphic_units(
                structural_model, structural_model_, *this ) );
        const auto& fault_mapping =
            mappings.at( Fault3D::component_type_static() );
        const auto& horizon_mapping =
            mappings.at( Horizon3D::component_type_static() );
        const auto& fault_block_mapping =
            mappings.at( FaultBlock3D::component_type_static() );
        const auto& stratigraphic_unit_mapping =
            mappings.at( StratigraphicUnit3D::component_type_static() );
        const auto& surface_mapping =
            mappings.at( Surface3D::component_type_static() );
        const auto& block_mapping =
            mappings.at( Block3D::component_type_static() );
        detail::copy_collection_item_relationships( structural_model,
            structural_model.faults(), fault_mapping, surface_mapping, *this );
        detail::copy_collection_item_relationships( structural_model,
            structural_model.horizons(), horizon_mapping, surface_mapping,
            *this );
        detail::copy_collection_item_relationships( structural_model,
            structural_model.fault_blocks(), fault_block_mapping, block_mapping,
            *this );
        detail::copy_collection_item_relationships( structural_model,
            structural_model.stratigraphic_units(), stratigraphic_unit_mapping,
            block_mapping, *this );
    }

    const uuid& StructuralModelBuilder::add_fault()
    {
        const auto& id = create_fault();
        register_component( structural_model_.fault( id ).component_id() );
        return id;
    }

    const uuid& StructuralModelBuilder::add_fault(
        typename Fault3D::FAULT_TYPE type )
    {
        const auto& id = create_fault( type );
        register_component( structural_model_.fault( id ).component_id() );
        return id;
    }

    void StructuralModelBuilder::add_surface_in_fault(
        const Surface3D& surface, const Fault3D& fault )
    {
        add_item_in_collection( surface.id(), fault.id() );
    }

    void StructuralModelBuilder::remove_fault( const Fault3D& fault )
    {
        unregister_component( fault.id() );
        delete_fault( fault );
    }

    const uuid& StructuralModelBuilder::add_horizon()
    {
        const auto& id = create_horizon();
        register_component( structural_model_.horizon( id ).component_id() );
        return id;
    }

    const uuid& StructuralModelBuilder::add_horizon(
        typename Horizon3D::HORIZON_TYPE type )
    {
        const auto& id = create_horizon( type );
        register_component( structural_model_.horizon( id ).component_id() );
        return id;
    }

    void StructuralModelBuilder::add_surface_in_horizon(
        const Surface3D& surface, const Horizon3D& horizon )
    {
        add_item_in_collection( surface.id(), horizon.id() );
    }

    void StructuralModelBuilder::remove_horizon( const Horizon3D& horizon )
    {
        unregister_component( horizon.id() );
        delete_horizon( horizon );
    }

    const uuid& StructuralModelBuilder::add_fault_block()
    {
        const auto& id = create_fault_block();
        register_component(
            structural_model_.fault_block( id ).component_id() );
        return id;
    }

    void StructuralModelBuilder::add_block_in_fault_block(
        const Block3D& block, const FaultBlock3D& fault_block )
    {
        add_item_in_collection( block.id(), fault_block.id() );
    }

    void StructuralModelBuilder::remove_fault_block(
        const FaultBlock3D& fault_block )
    {
        unregister_component( fault_block.id() );
        delete_fault_block( fault_block );
    }

    const uuid& StructuralModelBuilder::add_stratigraphic_unit()
    {
        const auto& id = create_stratigraphic_unit();
        register_component(
            structural_model_.stratigraphic_unit( id ).component_id() );
        return id;
    }

    void StructuralModelBuilder::add_block_in_stratigraphic_unit(
        const Block3D& block, const StratigraphicUnit3D& stratigraphic_unit )
    {
        add_item_in_collection( block.id(), stratigraphic_unit.id() );
    }

    void StructuralModelBuilder::remove_stratigraphic_unit(
        const StratigraphicUnit3D& stratigraphic_unit )
    {
        unregister_component( stratigraphic_unit.id() );
        delete_stratigraphic_unit( stratigraphic_unit );
    }

} // namespace geode
