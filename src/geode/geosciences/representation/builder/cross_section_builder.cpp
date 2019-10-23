/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/geosciences/representation/core/cross_section.h>

namespace geode
{
    CrossSectionBuilder::CrossSectionBuilder( CrossSection& cross_section )
        : SectionBuilder( cross_section ),
          AddComponentsBuilders< 2, Faults, Horizons >( cross_section )
    {
    }

    const uuid& CrossSectionBuilder::add_fault()
    {
        const auto& id = create_fault();
        register_component( id );
        return id;
    }

    const uuid& CrossSectionBuilder::add_fault(
        typename Fault2D::FAULT_TYPE type )
    {
        const auto& id = create_fault( type );
        register_component( id );
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
        register_component( id );
        return id;
    }

    const uuid& CrossSectionBuilder::add_horizon(
        typename Horizon2D::HORIZON_TYPE type )
    {
        const auto& id = create_horizon( type );
        register_component( id );
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

} // namespace geode
