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

#pragma once

#include <geode/bindings/python/model/model_vertex_check.hpp>

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
            StratigraphicUnitsBuilder3D >( module, "StructuralModelBuilder",
            "Builder of a StructuralModel: the only way to modify it. It also "
            "inherits the methods of BRepBuilder and of the builders of "
            "the geological components" )
            .def( pybind11::init< StructuralModel& >(),
                pybind11::arg( "model" ), pybind11::keep_alive< 1, 2 >(),
                remember_model(),
                "Build a builder of the model, which is kept alive by the "
                "builder" )
            .def( "copy", &StructuralModelBuilder::copy,
                pybind11::arg( "model" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Copy the name, components, relationships, unique vertices, "
                "geometry, geological components and physical properties of "
                "model into the empty "
                "model, and return the ModelCopyMapping from the uuids of "
                "model to the new ones. Raise an error if the model is not "
                "empty" )
            .def( "add_fault",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_fault ),
                "Create a Fault without type (no_type), add it to the model "
                "and "
                "return its uuid" )
            .def( "add_fault",
                static_cast< const uuid& (
                    StructuralModelBuilder::*) ( Fault3D::FAULT_TYPE ) >(
                    &StructuralModelBuilder::add_fault ),
                pybind11::arg( "type" ),
                "Create a Fault of the given FaultType, add it to the model "
                "and "
                "return its uuid" )
            .def( "add_fault",
                static_cast< void ( StructuralModelBuilder::* )(
                    const uuid& ) >( &StructuralModelBuilder::add_fault ),
                pybind11::arg( "fault_id" ),
                "Create a Fault without type (no_type) with the given uuid and "
                "add it to the model" )
            .def( "add_fault",
                static_cast< void ( StructuralModelBuilder::* )(
                    const uuid&, Fault3D::FAULT_TYPE ) >(
                    &StructuralModelBuilder::add_fault ),
                pybind11::arg( "fault_id" ), pybind11::arg( "type" ),
                "Create a Fault of the given FaultType with the given uuid and "
                "add it to the model" )
            .def( "add_horizon",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_horizon ),
                "Create a conformal Horizon, add it to the model and return "
                "its "
                "uuid" )
            .def( "add_horizon",
                static_cast< const uuid& (
                    StructuralModelBuilder::*) ( Horizon3D::CONTACT_TYPE ) >(
                    &StructuralModelBuilder::add_horizon ),
                pybind11::arg( "contact_type" ),
                "Create a Horizon of the given ContactType, add it to the "
                "model and return its uuid" )
            .def( "add_horizon",
                static_cast< void ( StructuralModelBuilder::* )(
                    const uuid& ) >( &StructuralModelBuilder::add_horizon ),
                pybind11::arg( "horizon_id" ),
                "Create a conformal Horizon with the given uuid and add it to "
                "the model" )
            .def( "add_horizon",
                static_cast< void ( StructuralModelBuilder::* )(
                    const uuid&, Horizon3D::CONTACT_TYPE ) >(
                    &StructuralModelBuilder::add_horizon ),
                pybind11::arg( "horizon_id" ), pybind11::arg( "contact_type" ),
                "Create a Horizon of the given ContactType with the given uuid "
                "and add it to the model" )
            .def( "add_fault_block",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_fault_block ),
                "Create a FaultBlock, add it to the model and return its uuid" )
            .def( "add_fault_block",
                static_cast< void ( StructuralModelBuilder::* )(
                    const uuid& ) >( &StructuralModelBuilder::add_fault_block ),
                pybind11::arg( "fault_block_id" ),
                "Create a FaultBlock with the given uuid and add it to the "
                "model" )
            .def( "add_stratigraphic_unit",
                static_cast< const uuid& (StructuralModelBuilder::*) () >(
                    &StructuralModelBuilder::add_stratigraphic_unit ),
                "Create a StratigraphicUnit, add it to the model and return "
                "its uuid" )
            .def( "add_stratigraphic_unit",
                static_cast< void ( StructuralModelBuilder::* )(
                    const uuid& ) >(
                    &StructuralModelBuilder::add_stratigraphic_unit ),
                pybind11::arg( "stratigraphic_unit_id" ),
                "Create a StratigraphicUnit with the given uuid and add it to "
                "the "
                "model" )
            .def( "remove_fault", &StructuralModelBuilder::remove_fault,
                pybind11::arg( "fault" ),
                "Remove the Fault and its relationships; its items are kept"
                ". The object passed, and every other Python object of this "
                "Fault (from fault(), faults()...), refer to freed "
                "memory afterwards and must not be used: keep its uuid "
                "instead (has_fault)" )
            .def( "remove_horizon", &StructuralModelBuilder::remove_horizon,
                pybind11::arg( "horizon" ),
                "Remove the Horizon and its relationships; its items are kept"
                ". The object passed, and every other Python object of this "
                "Horizon (from horizon(), horizons()...), refer to freed "
                "memory afterwards and must not be used: keep its uuid "
                "instead (has_horizon)" )
            .def( "remove_fault_block",
                &StructuralModelBuilder::remove_fault_block,
                pybind11::arg( "fault_block" ),
                "Remove the FaultBlock and its relationships; its items are "
                "kept"
                ". The object passed, and every other Python object of this "
                "FaultBlock (from fault_block(), fault_blocks()...), refer to "
                "freed "
                "memory afterwards and must not be used: keep its uuid "
                "instead (has_fault_block)" )
            .def( "remove_stratigraphic_unit",
                &StructuralModelBuilder::remove_stratigraphic_unit,
                pybind11::arg( "stratigraphic_unit" ),
                "Remove the StratigraphicUnit and its relationships; its items "
                "are kept"
                ". The object passed, and every other Python object of this "
                "StratigraphicUnit (from stratigraphic_unit(), "
                "stratigraphic_units()...), refer to freed "
                "memory afterwards and must not be used: keep its uuid "
                "instead (has_stratigraphic_unit)" )
            .def( "add_surface_in_fault",
                &StructuralModelBuilder::add_surface_in_fault,
                pybind11::arg( "surface" ), pybind11::arg( "fault" ),
                "Add the Surface to the Fault: item-collection relation. "
                "Return the index of the relation; if the two components are "
                "already related, log a warning and return the index of the "
                "existing relation, left unchanged" )
            .def( "add_surface_in_horizon",
                &StructuralModelBuilder::add_surface_in_horizon,
                pybind11::arg( "surface" ), pybind11::arg( "horizon" ),
                "Add the Surface to the Horizon: item-collection relation. "
                "Return the index of the relation; if the two components are "
                "already related, log a warning and return the index of the "
                "existing relation, left unchanged" )
            .def( "add_block_in_fault_block",
                &StructuralModelBuilder::add_block_in_fault_block,
                pybind11::arg( "block" ), pybind11::arg( "fault_block" ),
                "Add the Block to the FaultBlock: item-collection relation. "
                "Return the index of the relation; if the two components are "
                "already related, log a warning and return the index of the "
                "existing relation, left unchanged" )
            .def( "add_block_in_stratigraphic_unit",
                &StructuralModelBuilder::add_block_in_stratigraphic_unit,
                pybind11::arg( "block" ), pybind11::arg( "stratigraphic_unit" ),
                "Add the Block to the StratigraphicUnit: item-collection "
                "relation. "
                "Return the index of the relation; if the two components are "
                "already related, log a warning and return the index of the "
                "existing relation, left unchanged" );
    }
} // namespace geode
