/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/geosciences/representation/builder/cross_section_builder.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>

#include <geode/geosciences/representation/builder/detail/copy.h>
#include <geode/geosciences/representation/core/cross_section.h>

namespace geode
{
    CrossSectionBuilder::CrossSectionBuilder( CrossSection& cross_section )
        : SectionBuilder( cross_section ),
          AddComponentsBuilders< 2,
              Faults,
              Horizons,
              FaultBlocks,
              StratigraphicUnits >( cross_section ),
          cross_section_( cross_section )
    {
    }

    void CrossSectionBuilder::copy( const CrossSection& cross_section )
    {
        auto mappings = copy_components( cross_section );
        copy_geological_components( mappings, cross_section );
        copy_relationships( mappings, cross_section );
    }

    void CrossSectionBuilder::copy_geological_components(
        ModelCopyMapping& mappings, const CrossSection& cross_section )
    {
        mappings.emplace( Fault2D::component_type_static(),
            detail::copy_faults( cross_section, cross_section_, *this ) );
        mappings.emplace( Horizon2D::component_type_static(),
            detail::copy_horizons( cross_section, cross_section_, *this ) );
        mappings.emplace( FaultBlock2D::component_type_static(),
            detail::copy_fault_blocks( cross_section, cross_section_, *this ) );
        mappings.emplace( StratigraphicUnit2D::component_type_static(),
            detail::copy_stratigraphic_units(
                cross_section, cross_section_, *this ) );
    }

    const uuid& CrossSectionBuilder::add_fault()
    {
        const auto& id = create_fault();
        register_component( cross_section_.fault( id ).component_id() );
        return id;
    }

    const uuid& CrossSectionBuilder::add_fault(
        typename Fault2D::FAULT_TYPE type )
    {
        const auto& id = create_fault( type );
        register_component( cross_section_.fault( id ).component_id() );
        return id;
    }

    void CrossSectionBuilder::add_line_in_fault(
        const Line2D& line, const Fault2D& fault )
    {
        add_item_in_collection( line.id(), fault.id() );
    }

    void CrossSectionBuilder::remove_fault( const Fault2D& fault )
    {
        unregister_component( fault.id() );
        delete_fault( fault );
    }

    const uuid& CrossSectionBuilder::add_horizon()
    {
        const auto& id = create_horizon();
        register_component( cross_section_.horizon( id ).component_id() );
        return id;
    }

    const uuid& CrossSectionBuilder::add_horizon(
        typename Horizon2D::HORIZON_TYPE type )
    {
        const auto& id = create_horizon( type );
        register_component( cross_section_.horizon( id ).component_id() );
        return id;
    }

    void CrossSectionBuilder::add_line_in_horizon(
        const Line2D& line, const Horizon2D& horizon )
    {
        add_item_in_collection( line.id(), horizon.id() );
    }

    void CrossSectionBuilder::remove_horizon( const Horizon2D& horizon )
    {
        unregister_component( horizon.id() );
        delete_horizon( horizon );
    }

    const uuid& CrossSectionBuilder::add_fault_block()
    {
        const auto& id = create_fault_block();
        register_component( cross_section_.fault_block( id ).component_id() );
        return id;
    }

    void CrossSectionBuilder::add_surface_in_fault_block(
        const Surface2D& surface, const FaultBlock2D& fault_block )
    {
        add_item_in_collection( surface.id(), fault_block.id() );
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
        register_component(
            cross_section_.stratigraphic_unit( id ).component_id() );
        return id;
    }

    void CrossSectionBuilder::add_surface_in_stratigraphic_unit(
        const Surface2D& surface,
        const StratigraphicUnit2D& stratigraphic_unit )
    {
        add_item_in_collection( surface.id(), stratigraphic_unit.id() );
    }

    void CrossSectionBuilder::remove_stratigraphic_unit(
        const StratigraphicUnit2D& stratigraphic_unit )
    {
        unregister_component( stratigraphic_unit.id() );
        delete_stratigraphic_unit( stratigraphic_unit );
    }

} // namespace geode
