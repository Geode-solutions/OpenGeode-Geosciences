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

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.hpp>

namespace geode
{
    void define_implicit_cross_section( pybind11::module& module )
    {
        pybind11::class_< ImplicitCrossSection, CrossSection,
            pybind11::smart_holder >( module, "ImplicitCrossSection",
            "CrossSection whose Surfaces store an implicit value on their "
            "vertices, associated with a HorizonsStack: each Horizon of the "
            "stack has an implicit value (the Horizons present in the "
            "cross section have the uuid of the stack). It is modified "
            "through ImplicitCrossSectionBuilder" )
            .def( pybind11::init<>(), "Build an empty ImplicitCrossSection" )
            .def( pybind11::init( []( const ImplicitCrossSection& section ) {
                return section.clone();
            } ),
                pybind11::arg( "section" ),
                "Build an ImplicitCrossSection by cloning the section: a "
                "deep copy of its components, geometry, geological "
                "components, implicit values and horizons stack" )
            .def( pybind11::init( []( CrossSection& cross_section ) {
                return ImplicitCrossSection{ cross_section.clone() };
            } ),
                pybind11::arg( "cross_section" ),
                "Build an ImplicitCrossSection from a clone of the "
                "CrossSection, whose Surfaces get an implicit attribute" )
            .def(
                "__repr__",
                []( const ImplicitCrossSection& model ) {
                    return identifier_repr( model, "ImplicitCrossSection",
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
            .def( "clone", &ImplicitCrossSection::clone,
                "Return a deep copy of the cross section" )
            .def( "native_extension", &ImplicitCrossSection::native_extension,
                "Return the extension of the native file format of the "
                "cross section (og_ixsctn)" )
            .def( "implicit_attribute_id",
                &ImplicitCrossSection::implicit_attribute_id,
                pybind11::return_value_policy::reference_internal,
                "Return the uuid of the implicit attribute stored in the "
                "vertex attribute manager of the meshes of the Surfaces" )
            .def(
                "implicit_value_from_vertex_id",
                []( const ImplicitCrossSection& section,
                    const Surface2D& surface, index_t vertex_id ) {
                    check_component_vertex(
                        "implicit_value_from_vertex_id", surface, vertex_id );
                    return section.implicit_value( surface, vertex_id );
                },
                pybind11::arg( "surface" ), pybind11::arg( "vertex_id" ),
                "Return the implicit value of the vertex of the Surface; "
                "raise IndexError if the vertex is not in its mesh" )
            .def( "implicit_value_from_geometric_point",
                static_cast< std::optional< double > (
                    ImplicitCrossSection::* )(
                    const Surface2D&, const Point2D& ) const >(
                    &ImplicitCrossSection::implicit_value ),
                pybind11::arg( "surface" ), pybind11::arg( "point" ),
                "Return the implicit value at the point, computed in the "
                "polygon of the Surface containing it, or None if there is "
                "none" )
            .def(
                "implicit_value_from_geometric_point_and_triangle_id",
                []( const ImplicitCrossSection& section,
                    const Surface2D& surface, const Point2D& point,
                    index_t polygon_id ) {
                    check_surface_polygon(
                        "implicit_value_from_geometric_point_and_triangle_id",
                        surface, polygon_id );
                    return section.implicit_value( surface, point, polygon_id );
                },
                pybind11::arg( "surface" ), pybind11::arg( "point" ),
                pybind11::arg( "polygon_id" ),
                "Return the implicit value at the point, computed in the "
                "given polygon of the Surface; raise IndexError if it is not "
                "in its mesh" )
            .def( "containing_polygon",
                &ImplicitCrossSection::containing_polygon,
                pybind11::arg( "surface" ), pybind11::arg( "point" ),
                "Return the index of the polygon of the Surface containing "
                "the point, or None if there is none" )
            .def( "horizons_stack", &ImplicitCrossSection::horizons_stack,
                pybind11::return_value_policy::reference_internal,
                "Return the HorizonsStack2D associated with the cross "
                "section"
                ". It refers to the stack of the model: after "
                "set_horizons_stack, it and the objects obtained from it "
                "refer to freed memory and must not be used: call "
                "horizons_stack() again" )
            .def( "horizon_implicit_value",
                &ImplicitCrossSection::horizon_implicit_value,
                pybind11::arg( "horizon" ),
                "Return the implicit value (isovalue) of the Horizon, or None "
                "if none was set; raise an error if the Horizon is not in "
                "the HorizonsStack" )
            .def( "implicit_value_is_above_horizon",
                &ImplicitCrossSection::implicit_value_is_above_horizon,
                pybind11::arg( "implicit_function_value" ),
                pybind11::arg( "horizon" ),
                "Return True if the implicit value is on the side of the "
                "isovalue of the Horizon where the stack is above it (the "
                "isovalues of the stack increase or decrease from the "
                "bottom to the top). Raise an error if that order cannot be "
                "found or if the Horizon has no isovalue" )
            .def( "containing_stratigraphic_unit",
                &ImplicitCrossSection::containing_stratigraphic_unit,
                pybind11::arg( "implicit_function_value" ),
                "Return the uuid of the StratigraphicUnit of the "
                "HorizonsStack containing the implicit value, or None if "
                "the value is NaN or the order of the isovalues of the "
                "stack cannot be found" )
            .def( "implicit_cross_section_component",
                &ImplicitCrossSection::component, pybind11::arg( "id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component (mesh component, collection, fault, "
                "horizon, fault block or stratigraphic unit) with the given "
                "uuid; raise an error if the cross section has none" );
    }
} // namespace geode
