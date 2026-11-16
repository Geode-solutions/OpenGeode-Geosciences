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

#include "../../index_check.hpp"

#include <geode/geometry/point.hpp>

#include <geode/model/mixin/core/block.hpp>

#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>

namespace geode
{
    void define_implicit_structural_model( pybind11::module& module )
    {
        pybind11::class_< ImplicitStructuralModel, StructuralModel,
            pybind11::smart_holder >( module, "ImplicitStructuralModel",
            "StructuralModel whose Blocks store an implicit value on their "
            "vertices, associated with a HorizonsStack: each Horizon of the "
            "stack has an implicit value (the Horizons present in the model "
            "have the uuid of the stack). It is modified through "
            "ImplicitStructuralModelBuilder" )
            .def( pybind11::init<>(), "Build an empty ImplicitStructuralModel" )
            .def( pybind11::init( []( const ImplicitStructuralModel& model ) {
                return model.clone();
            } ),
                pybind11::arg( "model" ),
                "Build an ImplicitStructuralModel by cloning the model: a "
                "deep copy of its components, geometry, geological "
                "components, implicit values and horizons stack" )
            .def( pybind11::init( []( StructuralModel& model ) {
                return ImplicitStructuralModel{ model.clone() };
            } ),
                pybind11::arg( "structural_model" ),
                "Build an ImplicitStructuralModel from a clone of the "
                "StructuralModel, whose Blocks get an implicit attribute" )
            .def(
                "__repr__",
                []( const ImplicitStructuralModel& model ) {
                    return identifier_repr( model, "ImplicitStructuralModel",
                        { { "nb_corners", python_repr( model.nb_corners() ) },
                            { "nb_lines", python_repr( model.nb_lines() ) },
                            { "nb_surfaces",
                                python_repr( model.nb_surfaces() ) },
                            { "nb_blocks", python_repr( model.nb_blocks() ) },
                            { "nb_faults", python_repr( model.nb_faults() ) },
                            { "nb_horizons",
                                python_repr( model.nb_horizons() ) },
                            { "nb_fault_blocks",
                                python_repr( model.nb_fault_blocks() ) },
                            { "nb_stratigraphic_units",
                                python_repr(
                                    model.nb_stratigraphic_units() ) } } );
                },
                "Return an unambiguous description of the model: its type, "
                "its id, its name if it has one and its numbers of components "
                "and of geological components" )
            .def( "clone", &ImplicitStructuralModel::clone,
                "Return a deep copy of the model" )
            .def( "native_extension",
                &ImplicitStructuralModel::native_extension,
                "Return the extension of the native file format of the "
                "model (og_istrm)" )
            .def( "implicit_attribute_id",
                &ImplicitStructuralModel::implicit_attribute_id,
                pybind11::return_value_policy::reference_internal,
                "Return the uuid of the implicit attribute stored in the "
                "vertex attribute manager of the meshes of the Blocks" )
            .def(
                "implicit_value_from_vertex_id",
                []( const ImplicitStructuralModel& model, const Block3D& block,
                    index_t vertex_id ) {
                    check_component_vertex(
                        "implicit_value_from_vertex_id", block, vertex_id );
                    return model.implicit_value( block, vertex_id );
                },
                pybind11::arg( "block" ), pybind11::arg( "vertex_id" ),
                "Return the implicit value of the vertex of the Block; raise "
                "IndexError if the vertex is not in its mesh" )
            .def( "implicit_value_from_geometric_point",
                static_cast< std::optional< double > (
                    ImplicitStructuralModel::* )(
                    const Block3D&, const Point3D& ) const >(
                    &ImplicitStructuralModel::implicit_value ),
                pybind11::arg( "block" ), pybind11::arg( "point" ),
                "Return the implicit value at the point, computed in the "
                "polyhedron of the Block containing it, or None if there is "
                "none" )
            .def(
                "implicit_value_from_geometric_point_and_tetra_id",
                []( const ImplicitStructuralModel& model, const Block3D& block,
                    const Point3D& point, index_t polyhedron_id ) {
                    check_block_polyhedron(
                        "implicit_value_from_geometric_point_and_tetra_id",
                        block, polyhedron_id );
                    return model.implicit_value( block, point, polyhedron_id );
                },
                pybind11::arg( "block" ), pybind11::arg( "point" ),
                pybind11::arg( "polyhedron_id" ),
                "Return the implicit value at the point, computed in the "
                "given polyhedron of the Block; raise IndexError if it is not "
                "in its mesh" )
            .def( "containing_polyhedron",
                &ImplicitStructuralModel::containing_polyhedron,
                pybind11::arg( "block" ), pybind11::arg( "point" ),
                "Return the index of the polyhedron of the Block containing "
                "the point, or None if there is none" )
            .def( "horizons_stack", &ImplicitStructuralModel::horizons_stack,
                pybind11::return_value_policy::reference_internal,
                "Return the HorizonsStack3D associated with the model"
                ". It refers to the stack of the model: after "
                "set_horizons_stack, it and the objects obtained from it "
                "refer to freed memory and must not be used: call "
                "horizons_stack() again" )
            .def( "horizon_implicit_value",
                &ImplicitStructuralModel::horizon_implicit_value,
                pybind11::arg( "horizon" ),
                "Return the implicit value (isovalue) of the Horizon, or None "
                "if none was set; raise an error if the Horizon is not in "
                "the HorizonsStack" )
            .def( "implicit_value_is_above_horizon",
                &ImplicitStructuralModel::implicit_value_is_above_horizon,
                pybind11::arg( "implicit_function_value" ),
                pybind11::arg( "horizon" ),
                "Return True if the implicit value is on the side of the "
                "isovalue of the Horizon where the stack is above it (the "
                "isovalues of the stack increase or decrease from the "
                "bottom to the top). Raise an error if that order cannot be "
                "found or if the Horizon has no isovalue" )
            .def( "containing_stratigraphic_unit",
                &ImplicitStructuralModel::containing_stratigraphic_unit,
                pybind11::arg( "implicit_function_value" ),
                "Return the uuid of the StratigraphicUnit of the "
                "HorizonsStack containing the implicit value, or None if "
                "the value is NaN or the order of the isovalues of the "
                "stack cannot be found" )
            .def( "implicit_structural_model_component",
                &ImplicitStructuralModel::component, pybind11::arg( "id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component (mesh component, collection, fault, "
                "horizon, fault block or stratigraphic unit) with the given "
                "uuid; raise an error if the model has none" );
    }
} // namespace geode
