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

#include <geode/bindings/python/model/model_vertex_check.hpp>

#include <geode/geometry/point.hpp>

#include <geode/geosciences/implicit/representation/builder/stratigraphic_section_builder.hpp>

namespace geode
{
    void define_stratigraphic_section_builder( pybind11::module& module )
    {
        pybind11::class_< StratigraphicSectionBuilder,
            ImplicitCrossSectionBuilder >( module,
            "StratigraphicSectionBuilder",
            "Builder of a StratigraphicSection: the only way to modify it. It "
            "also inherits "
            "the methods of ImplicitCrossSectionBuilder" )
            .def( pybind11::init< StratigraphicSection& >(),
                pybind11::arg( "stratigraphic_section" ),
                pybind11::keep_alive< 1, 2 >(), remember_model(),
                "Build a builder of the cross section, which is kept alive by "
                "the builder" )
            .def( "copy", &StratigraphicSectionBuilder::copy,
                pybind11::arg( "stratigraphic_section" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Copy the name, components, relationships, unique vertices, "
                "geometry and geological components of stratigraphic_section, "
                "its implicit "
                "information and values and stratigraphic locations into the "
                "empty cross section, and "
                "return the ModelCopyMapping from its uuids to the new ones. "
                "Raise an error if the cross section is not empty" )
            .def( "instantiate_stratigraphic_attribute_on_surfaces",
                &StratigraphicSectionBuilder::
                    instantiate_stratigraphic_attribute_on_surfaces,
                "Create the implicit and the stratigraphic location "
                "attributes, if they do not exist, on the vertices of the "
                "meshes of the triangulated Surfaces; the other Surfaces are "
                "ignored" )
            .def( "reinitialize_stratigraphic_query_trees",
                &StratigraphicSectionBuilder::
                    reinitialize_stratigraphic_query_trees,
                "Rebuild the query trees on the implicit values and the "
                "stratigraphic locations" )
            .def(
                "set_stratigraphic_location",
                []( StratigraphicSectionBuilder& builder,
                    const Surface2D& surface, index_t vertex_id,
                    Point1D location ) {
                    check_component_vertex(
                        "set_stratigraphic_location", surface, vertex_id );
                    builder.set_stratigraphic_location(
                        surface, vertex_id, std::move( location ) );
                },
                pybind11::arg( "surface" ), pybind11::arg( "vertex_id" ),
                pybind11::arg( "value" ),
                "Set the stratigraphic location of the vertex of the Surface; "
                "raise IndexError if the vertex is not in its mesh" )
            .def(
                "set_stratigraphic_coordinates",
                []( StratigraphicSectionBuilder& builder,
                    const Surface2D& surface, index_t vertex_id,
                    const StratigraphicPoint2D& value ) {
                    check_component_vertex(
                        "set_stratigraphic_coordinates", surface, vertex_id );
                    builder.set_stratigraphic_coordinates(
                        surface, vertex_id, value );
                },
                pybind11::arg( "surface" ), pybind11::arg( "vertex_id" ),
                pybind11::arg( "value" ),
                "Set the stratigraphic location and the implicit value of the "
                "vertex of the Surface from the StratigraphicPoint2D; raise "
                "IndexError if the "
                "vertex is not in its mesh" )
            .def(
                "import_old_stratigraphic_attribute_values_from_attribute_name",
                &StratigraphicSectionBuilder::
                    import_old_stratigraphic_attribute_values_from_attribute_name,
                pybind11::arg( "old_stratigraphic_attribute_name" ),
                "In each Surface having the stratigraphic location attribute, "
                "copy the values of the first attribute whose name matches "
                "into it, then delete the old attribute" )
            .def( "import_old_stratigraphic_attribute_values_from_attribute_id",
                &StratigraphicSectionBuilder::
                    import_old_stratigraphic_attribute_values_from_attribute_id,
                pybind11::arg( "old_stratigraphic_attribute_id" ),
                "In each Surface having the stratigraphic location attribute "
                "and "
                "the old one, copy the values of the attribute with this "
                "uuid into it, then delete the old attribute" )
            .def( "copy_stratigraphic_attribute_values",
                &StratigraphicSectionBuilder::
                    copy_stratigraphic_attribute_values,
                pybind11::arg( "mapping" ), pybind11::arg( "other_section" ),
                "Copy the stratigraphic location values of the vertices of "
                "the meshes of the Surfaces of the other cross section into "
                "the "
                "copied Surfaces of this one, given by the "
                "ModelCopyMapping" );
    }
} // namespace geode
