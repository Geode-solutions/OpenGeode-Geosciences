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

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>

namespace geode
{
    void define_structural_model_builder( pybind11::module& module )
    {
        pybind11::class_< StructuralModelBuilder, BRepBuilder, FaultsBuilder3D,
            HorizonsBuilder3D, FaultBlocksBuilder3D,
            StratigraphicUnitsBuilder3D >( module, "StructuralModelBuilder" )
            .def( pybind11::init< StructuralModel& >() )
            .def( "copy",
                []( StructuralModelBuilder& builder,
                    const StructuralModel& other_model ) {
                    builder.copy( other_model );
                } )
            .def( "add_fault",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_fault ) )
            .def( "add_horizon",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_horizon ) )
            .def( "add_fault_block",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_fault_block ) )
            .def( "add_stratigraphic_unit",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_stratigraphic_unit ) )
            .def( "remove_fault", &StructuralModelBuilder::remove_fault )
            .def( "remove_horizon", &StructuralModelBuilder::remove_horizon )
            .def( "remove_fault_block",
                &StructuralModelBuilder::remove_fault_block )
            .def( "remove_stratigraphic_unit",
                &StructuralModelBuilder::remove_stratigraphic_unit )
            .def( "add_surface_in_fault",
                &StructuralModelBuilder::add_surface_in_fault )
            .def( "add_surface_in_horizon",
                &StructuralModelBuilder::add_surface_in_horizon )
            .def( "add_block_in_fault_block",
                &StructuralModelBuilder::add_block_in_fault_block )
            .def( "add_block_in_stratigraphic_unit",
                &StructuralModelBuilder::add_block_in_stratigraphic_unit );
    }
} // namespace geode
