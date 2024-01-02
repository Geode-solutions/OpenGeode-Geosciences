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

#include <geode/geosciences/implicit/representation/builder/stratigraphic_section_builder.h>

#include <geode/geometry/point.h>

#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

namespace geode
{
    StratigraphicSectionBuilder::StratigraphicSectionBuilder(
        StratigraphicSection& stratigraphic_section )
        : ImplicitCrossSectionBuilder( stratigraphic_section ),
          stratigraphic_section_( stratigraphic_section )
    {
    }

    ModelCopyMapping StratigraphicSectionBuilder::copy(
        const StratigraphicSection& stratigraphic_section )
    {
        auto mappings =
            ImplicitCrossSectionBuilder::copy( stratigraphic_section );
        stratigraphic_section_.initialize_stratigraphic_query_trees( {} );
        return mappings;
    }

    void StratigraphicSectionBuilder::reinitialize_stratigraphic_query_trees()
    {
        this->reinitialize_implicit_query_trees();
        stratigraphic_section_.initialize_stratigraphic_query_trees( {} );
    }

    void StratigraphicSectionBuilder::
        instantiate_stratigraphic_attribute_on_surfaces()
    {
        this->instantiate_implicit_attribute_on_surfaces();
        stratigraphic_section_.instantiate_stratigraphic_location_on_surfaces(
            {} );
    }

    void StratigraphicSectionBuilder::set_stratigraphic_location(
        const Surface2D& surface, index_t vertex_id, Point1D value )
    {
        stratigraphic_section_.set_stratigraphic_location(
            surface, vertex_id, std::move( value ), {} );
    }

    void StratigraphicSectionBuilder::set_stratigraphic_coordinates(
        const Surface2D& surface,
        index_t vertex_id,
        const StratigraphicPoint2D& value )
    {
        this->set_implicit_value( surface, vertex_id, value.implicit_value() );
        stratigraphic_section_.set_stratigraphic_location(
            surface, vertex_id, value.stratigraphic_location(), {} );
    }
} // namespace geode
