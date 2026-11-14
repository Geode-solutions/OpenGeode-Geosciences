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

#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.hpp>

namespace geode
{
    void define_stratigraphic_model_builder( pybind11::module& module )
    {
        pybind11::class_< StratigraphicModelBuilder,
            ImplicitStructuralModelBuilder >( module,
            "StratigraphicModelBuilder",
            "Builder of a StratigraphicModel: the only way to modify it. It "
            "also inherits "
            "the methods of ImplicitStructuralModelBuilder" )
            .def( pybind11::init< StratigraphicModel& >(),
                pybind11::arg( "stratigraphic_model" ),
                pybind11::keep_alive< 1, 2 >(), remember_model(),
                "Build a builder of the model, which is kept alive by "
                "the builder" )
            .def( "copy", &StratigraphicModelBuilder::copy,
                pybind11::arg( "stratigraphic_model" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Copy the name, components, relationships, unique vertices, "
                "geometry and geological components of stratigraphic_model, "
                "its implicit "
                "information and values and stratigraphic locations into the "
                "empty model, and "
                "return the ModelCopyMapping from its uuids to the new ones. "
                "Raise an error if the model is not empty" )
            .def( "instantiate_stratigraphic_attribute_on_blocks",
                &StratigraphicModelBuilder::
                    instantiate_stratigraphic_attribute_on_blocks,
                "Create the implicit and the stratigraphic location "
                "attributes, if they do not exist, on the vertices of the "
                "meshes of the tetrahedral Blocks; the other Blocks are "
                "ignored" )
            .def( "reinitialize_stratigraphic_query_trees",
                &StratigraphicModelBuilder::
                    reinitialize_stratigraphic_query_trees,
                "Rebuild the query trees on the implicit values and the "
                "stratigraphic locations" )
            .def(
                "set_stratigraphic_location",
                []( StratigraphicModelBuilder& builder, const Block3D& block,
                    index_t vertex_id, Point2D location ) {
                    check_component_vertex(
                        "set_stratigraphic_location", block, vertex_id );
                    builder.set_stratigraphic_location(
                        block, vertex_id, std::move( location ) );
                },
                pybind11::arg( "block" ), pybind11::arg( "vertex_id" ),
                pybind11::arg( "value" ),
                "Set the stratigraphic location of the vertex of the Block; "
                "raise IndexError if the vertex is not in its mesh" )
            .def(
                "set_stratigraphic_coordinates",
                []( StratigraphicModelBuilder& builder, const Block3D& block,
                    index_t vertex_id, const StratigraphicPoint3D& value ) {
                    check_component_vertex(
                        "set_stratigraphic_coordinates", block, vertex_id );
                    builder.set_stratigraphic_coordinates(
                        block, vertex_id, value );
                },
                pybind11::arg( "block" ), pybind11::arg( "vertex_id" ),
                pybind11::arg( "value" ),
                "Set the stratigraphic location and the implicit value of the "
                "vertex of the Block from the StratigraphicPoint3D; raise "
                "IndexError if the "
                "vertex is not in its mesh" )
            .def(
                "import_old_stratigraphic_attribute_values_from_attribute_name",
                &StratigraphicModelBuilder::
                    import_old_stratigraphic_attribute_values_from_attribute_name,
                pybind11::arg( "attribute_name" ),
                "In each Block having the stratigraphic location attribute, "
                "copy the values of the first attribute whose name matches "
                "into it, then delete the old attribute" )
            .def( "copy_stratigraphic_attribute_values",
                &StratigraphicModelBuilder::copy_stratigraphic_attribute_values,
                pybind11::arg( "mapping" ), pybind11::arg( "other_model" ),
                "Copy the stratigraphic location values of the vertices of "
                "the meshes of the Blocks of the other model into the "
                "copied Blocks of this one, given by the "
                "ModelCopyMapping" );
    }
} // namespace geode
