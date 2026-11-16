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

#include <geode/mesh/core/edged_curve.hpp>

#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/implicit/representation/core/stratigraphic_section.hpp>

namespace geode
{
    void define_stratigraphic_section( pybind11::module& module )
    {
        pybind11::class_< StratigraphicSection, ImplicitCrossSection,
            pybind11::smart_holder >( module, "StratigraphicSection",
            "ImplicitCrossSection whose Surfaces also store the "
            "stratigraphic location of their vertices: a point of the "
            "stratigraphic space is the stratigraphic location (1 "
            "coordinate) and the implicit value. It is modified through "
            "StratigraphicSectionBuilder" )
            .def( pybind11::init<>(), "Build an empty StratigraphicSection" )
            .def( pybind11::init( []( const StratigraphicSection& section ) {
                return section.clone();
            } ),
                pybind11::arg( "section" ),
                "Build a StratigraphicSection by cloning the section: a deep "
                "copy of its components, geometry, geological components, "
                "implicit values and stratigraphic locations" )
            .def( pybind11::init( []( ImplicitCrossSection& cross_section ) {
                return StratigraphicSection{ cross_section.clone() };
            } ),
                pybind11::arg( "implicit_section" ),
                "Build a StratigraphicSection from a clone of the "
                "ImplicitCrossSection, whose Surfaces get a stratigraphic "
                "location attribute" )
            .def( pybind11::init( []( CrossSection& cross_section ) {
                return StratigraphicSection{ cross_section.clone() };
            } ),
                pybind11::arg( "cross_section" ),
                "Build a StratigraphicSection from a clone of the "
                "CrossSection, whose Surfaces get an implicit and a "
                "stratigraphic location attribute" )
            .def(
                "__repr__",
                []( const StratigraphicSection& model ) {
                    return identifier_repr( model, "StratigraphicSection",
                        { { "nb_corners", python_repr( model.nb_corners() ) },
                            { "nb_lines", python_repr( model.nb_lines() ) },
                            { "nb_surfaces",
                                python_repr( model.nb_surfaces() ) },
                            { "nb_faults", python_repr( model.nb_faults() ) },
                            { "nb_horizons",
                                python_repr( model.nb_horizons() ) },
                            { "nb_fault_blocks",
                                python_repr( model.nb_fault_blocks() ) },
                            { "nb_stratigraphic_units",
                                python_repr(
                                    model.nb_stratigraphic_units() ) } } );
                },
                "Return an unambiguous description of the cross section: its "
                "type, its id, its name if it has one and its numbers of "
                "components and of geological components" )
            .def( "clone", &StratigraphicSection::clone,
                "Return a deep copy of the section" )
            .def(
                "stratigraphic_coordinates_from_vertex_id",
                []( const StratigraphicSection& section,
                    const Surface2D& surface, index_t vertex_id ) {
                    check_component_vertex(
                        "stratigraphic_coordinates_from_vertex_id", surface,
                        vertex_id );
                    return section.stratigraphic_coordinates(
                        surface, vertex_id );
                },
                pybind11::arg( "surface" ), pybind11::arg( "vertex_id" ),
                "Return the StratigraphicPoint2D of the vertex of the "
                "Surface; raise IndexError if the vertex is not in its mesh" )
            .def( "stratigraphic_coordinates_from_geometric_point",
                static_cast< std::optional< StratigraphicPoint2D > (
                    StratigraphicSection::* )(
                    const Surface2D&, const Point2D& ) const >(
                    &StratigraphicSection::stratigraphic_coordinates ),
                pybind11::arg( "surface" ), pybind11::arg( "geometric_point" ),
                "Return the StratigraphicPoint2D of the point, computed in "
                "the polygon of the Surface containing it, or None if there "
                "is none" )
            .def(
                "stratigraphic_coordinates_from_geometric_point_and_triangle_"
                "id",
                []( const StratigraphicSection& section,
                    const Surface2D& surface, const Point2D& geometric_point,
                    index_t polygon_id ) {
                    check_surface_polygon(
                        "stratigraphic_coordinates_from_geometric_point_and_"
                        "triangle_id",
                        surface, polygon_id );
                    return section.stratigraphic_coordinates(
                        surface, geometric_point, polygon_id );
                },
                pybind11::arg( "surface" ), pybind11::arg( "geometric_point" ),
                pybind11::arg( "polygon_id" ),
                "Return the StratigraphicPoint2D of the point, computed in "
                "the given polygon of the Surface; raise IndexError if it is "
                "not in its mesh" )
            .def( "geometric_coordinates_from_stratigraphic_point",
                static_cast< std::optional< Point2D > (
                    StratigraphicSection::* )(
                    const Surface2D&, const StratigraphicPoint2D& ) const >(
                    &StratigraphicSection::geometric_coordinates ),
                pybind11::arg( "surface" ),
                pybind11::arg( "stratigraphic_point" ),
                "Return the geometric point of the StratigraphicPoint2D, "
                "computed in the polygon of the Surface containing it in the "
                "stratigraphic space, or None if there is none" )
            .def(
                "geometric_coordinates_from_stratigraphic_point_and_triangle_"
                "id",
                []( const StratigraphicSection& section,
                    const Surface2D& surface,
                    const StratigraphicPoint2D& stratigraphic_point,
                    index_t polygon_id ) {
                    check_surface_polygon(
                        "geometric_coordinates_from_stratigraphic_point_and_"
                        "triangle_id",
                        surface, polygon_id );
                    return section.geometric_coordinates(
                        surface, stratigraphic_point, polygon_id );
                },
                pybind11::arg( "surface" ),
                pybind11::arg( "stratigraphic_point" ),
                pybind11::arg( "polygon_id" ),
                "Return the geometric point of the StratigraphicPoint2D, "
                "computed in the given polygon of the Surface; raise "
                "IndexError if it is not in its mesh" )
            .def( "stratigraphic_containing_polygon",
                &StratigraphicSection::stratigraphic_containing_polygon,
                pybind11::arg( "surface" ),
                pybind11::arg( "stratigraphic_point" ),
                "Return the index of a polygon of the Surface containing the "
                "StratigraphicPoint2D in the stratigraphic space, or None if "
                "there is none" )
            .def( "stratigraphic_line",
                &StratigraphicSection::stratigraphic_line,
                pybind11::arg( "surface" ), pybind11::arg( "line" ),
                "Return the list of the EdgedCurve2D of the Line in the "
                "stratigraphic space of the Surface, whose edges store the "
                "associated polygon edge: one curve for a boundary of the "
                "Surface, two "
                "(one per side) for an internal Line. Raise an error if the "
                "Line is neither a boundary nor an internal of the Surface" )
            .def( "stratigraphic_bounding_box",
                &StratigraphicSection::stratigraphic_bounding_box,
                "Return the BoundingBox2D of the vertices of the Surfaces in "
                "the stratigraphic space" )
            .def( "native_extension", &StratigraphicSection::native_extension,
                "Return the extension of the native file format of the "
                "section (og_stgs); OpenGeode-Geosciences registers no "
                "reader nor writer for it" )
            .def( "stratigraphic_section_component",
                &StratigraphicSection::component, pybind11::arg( "id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component (mesh component, collection, fault, "
                "horizon, fault block or stratigraphic unit) with the given "
                "uuid; raise an error if the section has none" )
            .def( "stratigraphic_location_attribute_id",
                &StratigraphicSection::stratigraphic_location_attribute_id,
                pybind11::return_value_policy::reference_internal,
                "Return the uuid of the stratigraphic location attribute "
                "stored in the vertex attribute manager of the meshes of the "
                "Surfaces" );
    }
} // namespace geode
