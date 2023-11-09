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

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>

#include <geode/geosciences/explicit/representation/builder/detail/copy.h>
#include <geode/geosciences/explicit/representation/core/cross_section.h>

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
        auto mappings = copy_components( cross_section );
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
        return id;
    }

    const uuid& CrossSectionBuilder::add_fault(
        typename Fault2D::FAULT_TYPE type )
    {
        const auto& id = create_fault( type );
        return id;
    }

    void CrossSectionBuilder::add_fault( uuid fault_id )
    {
        create_fault( std::move( fault_id ) );
    }

    void CrossSectionBuilder::add_fault(
        uuid fault_id, typename Fault2D::FAULT_TYPE type )
    {
        create_fault( std::move( fault_id ), type );
    }

    index_t CrossSectionBuilder::add_line_in_fault(
        const Line2D& line, const Fault2D& fault )
    {
        return add_item_in_collection(
            line.component_id(), fault.component_id() );
    }

    void CrossSectionBuilder::remove_fault( const Fault2D& fault )
    {
        unregister_component( fault.id() );
        delete_fault( fault );
    }

    const uuid& CrossSectionBuilder::add_horizon()
    {
        const auto& id = create_horizon();
        return id;
    }

    const uuid& CrossSectionBuilder::add_horizon(
        typename Horizon2D::HORIZON_TYPE type )
    {
        const auto& id = create_horizon( type );
        return id;
    }

    void CrossSectionBuilder::add_horizon( uuid horizon_id )
    {
        create_horizon( std::move( horizon_id ) );
    }

    void CrossSectionBuilder::add_horizon(
        uuid horizon_id, typename Horizon2D::HORIZON_TYPE type )
    {
        create_horizon( std::move( horizon_id ), type );
    }

    index_t CrossSectionBuilder::add_line_in_horizon(
        const Line2D& line, const Horizon2D& horizon )
    {
        return add_item_in_collection(
            line.component_id(), horizon.component_id() );
    }

    void CrossSectionBuilder::remove_horizon( const Horizon2D& horizon )
    {
        unregister_component( horizon.id() );
        delete_horizon( horizon );
    }

    const uuid& CrossSectionBuilder::add_fault_block()
    {
        const auto& id = create_fault_block();
        return id;
    }

    void CrossSectionBuilder::add_fault_block( uuid fault_block_id )
    {
        create_fault_block( fault_block_id );
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
        unregister_component( fault_block.id() );
        delete_fault_block( fault_block );
    }

    const uuid& CrossSectionBuilder::add_stratigraphic_unit()
    {
        const auto& id = create_stratigraphic_unit();
        return id;
    }

    void CrossSectionBuilder::add_stratigraphic_unit(
        uuid stratigraphic_unit_id )
    {
        create_stratigraphic_unit( std::move( stratigraphic_unit_id ) );
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
        unregister_component( stratigraphic_unit.id() );
        delete_stratigraphic_unit( stratigraphic_unit );
    }

} // namespace geode
