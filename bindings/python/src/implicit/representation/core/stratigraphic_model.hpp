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

#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>

namespace geode
{
    void define_stratigraphic_model( pybind11::module& module )
    {
        pybind11::class_< StratigraphicModel, ImplicitStructuralModel,
            pybind11::smart_holder >( module, "StratigraphicModel",
            "ImplicitStructuralModel whose tetrahedral Blocks also store the "
            "stratigraphic location of their vertices: a point of the "
            "stratigraphic space is the stratigraphic location (2 "
            "coordinates) and the implicit value. It is modified through "
            "StratigraphicModelBuilder" )
            .def( pybind11::init<>(), "Build an empty StratigraphicModel" )
            .def( pybind11::init( []( const StratigraphicModel& model ) {
                return model.clone();
            } ),
                pybind11::arg( "model" ),
                "Build a StratigraphicModel by cloning the model: a deep copy "
                "of its components, geometry, geological components, "
                "implicit values and stratigraphic locations" )
            .def( pybind11::init( []( ImplicitStructuralModel& model ) {
                Logger::info( "Converting ImplicitStructuralModel to "
                              "StratigraphicModel" );
                return StratigraphicModel{ model.clone() };
            } ),
                pybind11::arg( "implicit_model" ),
                "Build a StratigraphicModel from a clone of the "
                "ImplicitStructuralModel, whose Blocks get a stratigraphic "
                "location attribute" )
            .def( pybind11::init( []( StructuralModel& model ) {
                Logger::info(
                    "Converting StructuralModel to StratigraphicModel" );
                return StratigraphicModel{ model.clone() };
            } ),
                pybind11::arg( "structural_model" ),
                "Build a StratigraphicModel from a clone of the "
                "StructuralModel, whose Blocks get an implicit and a "
                "stratigraphic location attribute" )
            .def(
                "__repr__",
                []( const StratigraphicModel& model ) {
                    return identifier_repr( model, "StratigraphicModel",
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
            .def( "clone", &StratigraphicModel::clone,
                "Return a deep copy of the model" )
            .def(
                "stratigraphic_coordinates_from_vertex_id",
                []( const StratigraphicModel& model, const Block3D& block,
                    index_t vertex_id ) {
                    check_component_vertex(
                        "stratigraphic_coordinates_from_vertex_id", block,
                        vertex_id );
                    return model.stratigraphic_coordinates( block, vertex_id );
                },
                pybind11::arg( "block" ), pybind11::arg( "vertex_id" ),
                "Return the StratigraphicPoint3D of the vertex of the Block; "
                "raise IndexError if the vertex is not in its mesh" )
            .def( "stratigraphic_coordinates_from_geometric_point",
                static_cast< std::optional< StratigraphicPoint3D > (
                    StratigraphicModel::* )( const Block3D&, const Point3D& )
                        const >(
                    &StratigraphicModel::stratigraphic_coordinates ),
                pybind11::arg( "block" ), pybind11::arg( "geometric_point" ),
                "Return the StratigraphicPoint3D of the point, computed in "
                "the polyhedron of the Block containing it, or None if there "
                "is none" )
            .def(
                "stratigraphic_coordinates_from_geometric_point_and_tetra_id",
                []( const StratigraphicModel& model, const Block3D& block,
                    const Point3D& geometric_point, index_t polyhedron_id ) {
                    check_block_polyhedron(
                        "stratigraphic_coordinates_from_geometric_point_and_"
                        "tetra_id",
                        block, polyhedron_id );
                    return model.stratigraphic_coordinates(
                        block, geometric_point, polyhedron_id );
                },
                pybind11::arg( "block" ), pybind11::arg( "geometric_point" ),
                pybind11::arg( "polyhedron_id" ),
                "Return the StratigraphicPoint3D of the point, computed in "
                "the given polyhedron of the Block; raise IndexError if it is "
                "not in its mesh" )
            .def( "geometric_coordinates_from_stratigraphic_point",
                static_cast< std::optional< Point3D > ( StratigraphicModel::* )(
                    const Block3D&, const StratigraphicPoint3D& ) const >(
                    &StratigraphicModel::geometric_coordinates ),
                pybind11::arg( "block" ),
                pybind11::arg( "stratigraphic_point" ),
                "Return the geometric point of the StratigraphicPoint3D, "
                "computed in the polyhedron of the Block containing it in the "
                "stratigraphic space, or None if there is none" )
            .def(
                "geometric_coordinates_from_stratigraphic_point_and_tetra_id",
                []( const StratigraphicModel& model, const Block3D& block,
                    const StratigraphicPoint3D& stratigraphic_point,
                    index_t polyhedron_id ) {
                    check_block_polyhedron(
                        "geometric_coordinates_from_stratigraphic_point_and_"
                        "tetra_id",
                        block, polyhedron_id );
                    return model.geometric_coordinates(
                        block, stratigraphic_point, polyhedron_id );
                },
                pybind11::arg( "block" ),
                pybind11::arg( "stratigraphic_point" ),
                pybind11::arg( "polyhedron_id" ),
                "Return the geometric point of the StratigraphicPoint3D, "
                "computed in the given polyhedron of the Block; raise "
                "IndexError if it is not in its mesh" )
            .def( "stratigraphic_containing_polyhedron",
                &StratigraphicModel::stratigraphic_containing_polyhedron,
                pybind11::arg( "block" ),
                pybind11::arg( "stratigraphic_point" ),
                "Return the index of a polyhedron of the Block containing the "
                "StratigraphicPoint3D in the stratigraphic space, or None if "
                "there is none" )
            .def( "stratigraphic_surface",
                &StratigraphicModel::stratigraphic_surface,
                pybind11::arg( "block" ), pybind11::arg( "surface" ),
                "Return the list of the TriangulatedSurface3D of the Surface "
                "in the stratigraphic space of the Block, whose polygons "
                "store the associated polyhedron facet: one surface for a "
                "boundary of the Block, two (one per side) for an internal "
                "Surface. Raise an error if the Surface is neither a boundary "
                "nor an internal of the Block" )
            .def( "stratigraphic_bounding_box",
                &StratigraphicModel::stratigraphic_bounding_box,
                "Return the BoundingBox3D of the vertices of the tetrahedral "
                "Blocks in the stratigraphic space" )
            .def( "native_extension", &StratigraphicModel::native_extension,
                "Return the extension of the native file format of the "
                "model (og_stgm); OpenGeode-Geosciences registers no reader "
                "nor writer for it" )
            .def( "stratigraphic_model_component",
                &StratigraphicModel::component, pybind11::arg( "id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component (mesh component, collection, fault, "
                "horizon, fault block or stratigraphic unit) with the given "
                "uuid; raise an error if the model has none" )
            .def( "stratigraphic_location_attribute_id",
                &StratigraphicModel::stratigraphic_location_attribute_id,
                pybind11::return_value_policy::reference_internal,
                "Return the uuid of the stratigraphic location attribute "
                "stored in the vertex attribute manager of the meshes of the "
                "Blocks" );
    }
} // namespace geode
