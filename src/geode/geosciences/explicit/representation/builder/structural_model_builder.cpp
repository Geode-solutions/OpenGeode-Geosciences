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

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/relationships.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/detail/copy.hpp>
#include <geode/model/representation/builder/detail/register.hpp>

#include <geode/geosciences/explicit/representation/builder/detail/copy.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>

namespace geode
{
    StructuralModelBuilder::StructuralModelBuilder(
        StructuralModel& structural_model )
        : BRepBuilder( structural_model ),
          FaultsBuilder3D( structural_model ),
          HorizonsBuilder3D( structural_model ),
          FaultBlocksBuilder3D( structural_model ),
          StratigraphicUnitsBuilder3D( structural_model ),
          structural_model_( structural_model )
    {
    }

    ModelCopyMapping StructuralModelBuilder::copy(
        const StructuralModel& structural_model )
    {
        auto mappings = copy_components( structural_model );
        copy_component_geometry( mappings, structural_model );
        copy_geological_components( mappings, structural_model );
        copy_relationships( mappings, structural_model );
        return mappings;
    }

    void StructuralModelBuilder::copy_geological_components(
        ModelCopyMapping& mappings, const StructuralModel& structural_model )
    {
        detail::copy_faults( structural_model, *this,
            mappings[Fault3D::component_type_static()] );
        detail::copy_horizons( structural_model, *this,
            mappings[Horizon3D::component_type_static()] );
        detail::copy_fault_blocks( structural_model, *this,
            mappings[FaultBlock3D::component_type_static()] );
        detail::copy_stratigraphic_units( structural_model, *this,
            mappings[StratigraphicUnit3D::component_type_static()] );
    }

    const uuid& StructuralModelBuilder::add_fault()
    {
        const auto& id = create_fault();
        detail::add_collection_component(
            *this, structural_model_.fault( id ) );
        return id;
    }

    const uuid& StructuralModelBuilder::add_fault(
        typename Fault3D::FAULT_TYPE type )
    {
        const auto& id = create_fault( type );
        detail::add_collection_component(
            *this, structural_model_.fault( id ) );
        return id;
    }

    void StructuralModelBuilder::add_fault( const uuid& fault_id )
    {
        create_fault( fault_id );
        detail::add_collection_component(
            *this, structural_model_.fault( fault_id ) );
    }

    void StructuralModelBuilder::add_fault(
        const uuid& fault_id, typename Fault3D::FAULT_TYPE type )
    {
        create_fault( fault_id, type );
        detail::add_collection_component(
            *this, structural_model_.fault( fault_id ) );
    }

    index_t StructuralModelBuilder::add_surface_in_fault(
        const Surface3D& surface, const Fault3D& fault )
    {
        return add_item_in_collection(
            surface.component_id(), fault.component_id() );
    }

    void StructuralModelBuilder::remove_fault( const Fault3D& fault )
    {
        detail::remove_collection_component( *this, fault );
        delete_fault( fault );
    }

    const uuid& StructuralModelBuilder::add_horizon()
    {
        const auto& id = create_horizon();
        detail::add_collection_component(
            *this, structural_model_.horizon( id ) );
        return id;
    }

    const uuid& StructuralModelBuilder::add_horizon(
        typename Horizon3D::HORIZON_TYPE type )
    {
        const auto& id = create_horizon( type );
        detail::add_collection_component(
            *this, structural_model_.horizon( id ) );
        return id;
    }

    void StructuralModelBuilder::add_horizon( const uuid& horizon_id )
    {
        create_horizon( horizon_id );
        detail::add_collection_component(
            *this, structural_model_.horizon( horizon_id ) );
    }

    void StructuralModelBuilder::add_horizon(
        const uuid& horizon_id, typename Horizon3D::HORIZON_TYPE type )
    {
        create_horizon( horizon_id, type );
        detail::add_collection_component(
            *this, structural_model_.horizon( horizon_id ) );
    }

    index_t StructuralModelBuilder::add_surface_in_horizon(
        const Surface3D& surface, const Horizon3D& horizon )
    {
        return add_item_in_collection(
            surface.component_id(), horizon.component_id() );
    }

    void StructuralModelBuilder::remove_horizon( const Horizon3D& horizon )
    {
        detail::remove_collection_component( *this, horizon );
        delete_horizon( horizon );
    }

    const uuid& StructuralModelBuilder::add_fault_block()
    {
        const auto& id = create_fault_block();
        detail::add_collection_component(
            *this, structural_model_.fault_block( id ) );
        return id;
    }

    void StructuralModelBuilder::add_fault_block( const uuid& fault_block_id )
    {
        create_fault_block( fault_block_id );
        detail::add_collection_component(
            *this, structural_model_.fault_block( fault_block_id ) );
    }

    index_t StructuralModelBuilder::add_block_in_fault_block(
        const Block3D& block, const FaultBlock3D& fault_block )
    {
        return add_item_in_collection(
            block.component_id(), fault_block.component_id() );
    }

    void StructuralModelBuilder::remove_fault_block(
        const FaultBlock3D& fault_block )
    {
        detail::remove_collection_component( *this, fault_block );
        delete_fault_block( fault_block );
    }

    const uuid& StructuralModelBuilder::add_stratigraphic_unit()
    {
        const auto& id = create_stratigraphic_unit();
        detail::add_collection_component(
            *this, structural_model_.stratigraphic_unit( id ) );
        return id;
    }

    void StructuralModelBuilder::add_stratigraphic_unit( const uuid& unit_id )
    {
        create_stratigraphic_unit( unit_id );
        detail::add_collection_component(
            *this, structural_model_.stratigraphic_unit( unit_id ) );
    }

    index_t StructuralModelBuilder::add_block_in_stratigraphic_unit(
        const Block3D& block, const StratigraphicUnit3D& stratigraphic_unit )
    {
        return add_item_in_collection(
            block.component_id(), stratigraphic_unit.component_id() );
    }

    void StructuralModelBuilder::remove_stratigraphic_unit(
        const StratigraphicUnit3D& stratigraphic_unit )
    {
        detail::remove_collection_component( *this, stratigraphic_unit );
        delete_stratigraphic_unit( stratigraphic_unit );
    }

} // namespace geode
