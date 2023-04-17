/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.h>

#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/detail/copy.h>

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>

namespace geode
{
    ImplicitCrossSectionBuilder::ImplicitCrossSectionBuilder(
        ImplicitCrossSection& implicit_section )
        : CrossSectionBuilder( implicit_section ),
          implicit_section_( implicit_section )
    {
    }

    void ImplicitCrossSectionBuilder::copy(
        const ImplicitCrossSection& implicit_section )
    {
        CrossSectionBuilder::copy( implicit_section );
        instantiate_implicit_attribute_on_surfaces();
    }

    void ImplicitCrossSectionBuilder::
        instantiate_implicit_attribute_on_surfaces()
    {
        implicit_section_.instantiate_implicit_attribute_on_surfaces( {} );
    }

    void ImplicitCrossSectionBuilder::set_implicit_value(
        const Surface2D& surface, index_t vertex_id, double value )
    {
        implicit_section_.set_implicit_value( surface, vertex_id, value, {} );
    }

    void ImplicitCrossSectionBuilder::set_stratigraphic_units_stack(
        StratigraphicUnitsStack2D&& stack )
    {
        implicit_section_.set_stratigraphic_units_stack(
            std::move( stack ), {} );
    }

    void ImplicitCrossSectionBuilder::set_horizon_isovalue(
        const Horizon2D& horizon, double isovalue )
    {
        implicit_section_.set_horizon_isovalue( horizon, isovalue, {} );
    }
} // namespace geode
