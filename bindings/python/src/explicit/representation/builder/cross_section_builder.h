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

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.h>

namespace geode
{
    void define_cross_section_builder( pybind11::module& module )
    {
        pybind11::class_< CrossSectionBuilder, SectionBuilder, FaultsBuilder2D,
            HorizonsBuilder2D, FaultBlocksBuilder2D,
            StratigraphicUnitsBuilder2D >( module, "CrossSectionBuilder" )
            .def( pybind11::init< CrossSection& >() )
            .def( "copy",
                []( CrossSectionBuilder& builder,
                    const CrossSection& other_section ) {
                    builder.copy( other_section );
                } )
            .def( "add_fault",
                static_cast< const uuid& (CrossSectionBuilder::*) () >(
                    &CrossSectionBuilder::add_fault ) )
            .def( "add_horizon",
                static_cast< const uuid& (CrossSectionBuilder::*) () >(
                    &CrossSectionBuilder::add_horizon ) )
            .def( "add_fault_block",
                static_cast< const uuid& (CrossSectionBuilder::*) () >(
                    &CrossSectionBuilder::add_fault_block ) )
            .def( "add_stratigraphic_unit",
                static_cast< const uuid& (CrossSectionBuilder::*) () >(
                    &CrossSectionBuilder::add_stratigraphic_unit ) )
            .def( "remove_fault", &CrossSectionBuilder::remove_fault )
            .def( "remove_horizon", &CrossSectionBuilder::remove_horizon )
            .def(
                "remove_fault_block", &CrossSectionBuilder::remove_fault_block )
            .def( "remove_stratigraphic_unit",
                &CrossSectionBuilder::remove_stratigraphic_unit )
            .def( "add_line_in_fault", &CrossSectionBuilder::add_line_in_fault )
            .def( "add_line_in_horizon",
                &CrossSectionBuilder::add_line_in_horizon )
            .def( "add_surface_in_fault_block",
                &CrossSectionBuilder::add_surface_in_fault_block )
            .def( "add_surface_in_stratigraphic_unit",
                &CrossSectionBuilder::add_surface_in_stratigraphic_unit );
    }
} // namespace geode
