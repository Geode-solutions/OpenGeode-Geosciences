/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.hpp>

#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/relationships.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/detail/copy.hpp>
#include <geode/model/representation/builder/detail/register.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>
#include <geode/geosciences/explicit/representation/builder/detail/copy.hpp>
#include <geode/geosciences/explicit/representation/core/cross_section.hpp>

namespace geode
{
    CrossSectionBuilder::CrossSectionBuilder( CrossSection& cross_section )
        : SectionBuilder( cross_section ),
          FaultsBuilder2D( cross_section ),
          HorizonsBuilder2D( cross_section ),
          FaultBlocksBuilder2D( cross_section ),
          StratigraphicUnitsBuilder2D( cross_section ),
          cross_section_( cross_section )
    {
    }

    ModelCopyMapping CrossSectionBuilder::copy(
        const CrossSection& cross_section )
    {
        set_name( cross_section.name() );
        auto mappings = copy_components( cross_section );
        copy_component_geometry( mappings, cross_section );
        copy_geological_components( mappings, cross_section );
        copy_relationships( mappings, cross_section );
        return mappings;
    }

    void CrossSectionBuilder::copy_geological_components(
        ModelCopyMapping& mappings, const CrossSection& cross_section )
    {
        detail::copy_faults(
            cross_section, *this, mappings[Fault2D::component_type_static()] );
        detail::copy_horizons( cross_section, *this,
            mappings[Horizon2D::component_type_static()] );
        detail::copy_fault_blocks( cross_section, *this,
            mappings[FaultBlock2D::component_type_static()] );
        detail::copy_stratigraphic_units( cross_section, *this,
            mappings[StratigraphicUnit2D::component_type_static()] );
    }

    const uuid& CrossSectionBuilder::add_fault()
    {
        const auto& id = create_fault();
        detail::add_collection_component( *this, cross_section_.fault( id ) );
        return id;
    }

    const uuid& CrossSectionBuilder::add_fault(
        typename Fault2D::FAULT_TYPE type )
    {
        const auto& id = create_fault( type );
        detail::add_collection_component( *this, cross_section_.fault( id ) );
        return id;
    }

    void CrossSectionBuilder::add_fault( const uuid& fault_id )
    {
        create_fault( fault_id );
        detail::add_collection_component(
            *this, cross_section_.fault( fault_id ) );
    }

    void CrossSectionBuilder::add_fault(
        const uuid& fault_id, typename Fault2D::FAULT_TYPE type )
    {
        create_fault( fault_id, type );
        detail::add_collection_component(
            *this, cross_section_.fault( fault_id ) );
    }

    index_t CrossSectionBuilder::add_line_in_fault(
        const Line2D& line, const Fault2D& fault )
    {
        return add_item_in_collection(
            line.component_id(), fault.component_id() );
    }

    void CrossSectionBuilder::remove_fault( const Fault2D& fault )
    {
        detail::remove_collection_component( *this, fault );
        delete_fault( fault );
    }

    const uuid& CrossSectionBuilder::add_horizon()
    {
        const auto& id = create_horizon();
        detail::add_collection_component( *this, cross_section_.horizon( id ) );
        return id;
    }

    const uuid& CrossSectionBuilder::add_horizon(
        typename Horizon2D::CONTACT_TYPE contact_type )
    {
        const auto& id = create_horizon( contact_type );
        detail::add_collection_component( *this, cross_section_.horizon( id ) );
        return id;
    }

    void CrossSectionBuilder::add_horizon( const uuid& horizon_id )
    {
        create_horizon( horizon_id );
        detail::add_collection_component(
            *this, cross_section_.horizon( horizon_id ) );
    }

    void CrossSectionBuilder::add_horizon(
        const uuid& horizon_id, typename Horizon2D::CONTACT_TYPE contact_type )
    {
        create_horizon( horizon_id, contact_type );
        detail::add_collection_component(
            *this, cross_section_.horizon( horizon_id ) );
    }

    index_t CrossSectionBuilder::add_line_in_horizon(
        const Line2D& line, const Horizon2D& horizon )
    {
        return add_item_in_collection(
            line.component_id(), horizon.component_id() );
    }

    void CrossSectionBuilder::remove_horizon( const Horizon2D& horizon )
    {
        detail::remove_collection_component( *this, horizon );
        delete_horizon( horizon );
    }

    const uuid& CrossSectionBuilder::add_fault_block()
    {
        const auto& id = create_fault_block();
        detail::add_collection_component(
            *this, cross_section_.fault_block( id ) );
        return id;
    }

    void CrossSectionBuilder::add_fault_block( const uuid& fault_block_id )
    {
        create_fault_block( fault_block_id );
        detail::add_collection_component(
            *this, cross_section_.fault_block( fault_block_id ) );
    }

    index_t CrossSectionBuilder::add_surface_in_fault_block(
        const Surface2D& surface, const FaultBlock2D& fault_block )
    {
        return add_item_in_collection(
            surface.component_id(), fault_block.component_id() );
    }

    void CrossSectionBuilder::remove_fault_block(
        const FaultBlock2D& fault_block )
    {
        detail::remove_collection_component( *this, fault_block );
        delete_fault_block( fault_block );
    }

    const uuid& CrossSectionBuilder::add_stratigraphic_unit()
    {
        const auto& id = create_stratigraphic_unit();
        detail::add_collection_component(
            *this, cross_section_.stratigraphic_unit( id ) );
        return id;
    }

    void CrossSectionBuilder::add_stratigraphic_unit(
        const uuid& stratigraphic_unit_id )
    {
        create_stratigraphic_unit( stratigraphic_unit_id );
        detail::add_collection_component(
            *this, cross_section_.stratigraphic_unit( stratigraphic_unit_id ) );
    }

    index_t CrossSectionBuilder::add_surface_in_stratigraphic_unit(
        const Surface2D& surface,
        const StratigraphicUnit2D& stratigraphic_unit )
    {
        return add_item_in_collection(
            surface.component_id(), stratigraphic_unit.component_id() );
    }

    void CrossSectionBuilder::remove_stratigraphic_unit(
        const StratigraphicUnit2D& stratigraphic_unit )
    {
        detail::remove_collection_component( *this, stratigraphic_unit );
        delete_stratigraphic_unit( stratigraphic_unit );
    }

} // namespace geode
