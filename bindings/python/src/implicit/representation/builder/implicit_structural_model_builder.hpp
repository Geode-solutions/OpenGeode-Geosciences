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

#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.hpp>

namespace geode
{
    void define_implicit_structural_model_builder( pybind11::module& module )
    {
        pybind11::class_< ImplicitStructuralModelBuilder,
            StructuralModelBuilder >( module, "ImplicitStructuralModelBuilder",
            "Builder of an ImplicitStructuralModel: the only way to modify it. "
            "It also inherits "
            "the methods of StructuralModelBuilder" )
            .def( pybind11::init< ImplicitStructuralModel& >(),
                pybind11::arg( "implicit_model" ),
                pybind11::keep_alive< 1, 2 >(), remember_model(),
                "Build a builder of the model, which is kept alive by "
                "the builder" )
            .def( "copy", &ImplicitStructuralModelBuilder::copy,
                pybind11::arg( "implicit_model" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Copy the name, components, relationships, unique vertices, "
                "geometry and geological components of implicit_model, its "
                "implicit "
                "information and values into the empty model, and "
                "return the ModelCopyMapping from its uuids to the new ones. "
                "Raise an error if the model is not empty" )
            .def( "instantiate_implicit_attribute_on_blocks",
                &ImplicitStructuralModelBuilder::
                    instantiate_implicit_attribute_on_blocks,
                "Create the implicit attribute, if it does not exist, on the "
                "vertices of the meshes of the tetrahedral Blocks; the other "
                "Blocks are "
                "ignored" )
            .def(
                "set_implicit_value",
                []( ImplicitStructuralModelBuilder& builder,
                    const Block3D& block, index_t vertex_id, double value ) {
                    check_component_vertex(
                        "set_implicit_value", block, vertex_id );
                    builder.set_implicit_value( block, vertex_id, value );
                },
                pybind11::arg( "block" ), pybind11::arg( "vertex_id" ),
                pybind11::arg( "value" ),
                "Set the implicit value of the vertex of the Block; "
                "raise IndexError if the vertex is not in its mesh" )
            .def(
                "set_horizons_stack",
                []( ImplicitStructuralModelBuilder& builder,
                    HorizonsStack3D& horizons_stack ) {
                    builder.set_horizons_stack( horizons_stack.clone() );
                },
                pybind11::arg( "horizons_stack" ),
                "Set a clone of the HorizonsStack associated with the model"
                ". The previous HorizonsStack is destroyed: the objects "
                "returned before by horizons_stack() and "
                "horizons_stack_builder(), and every Python object of its "
                "Horizons and StratigraphicUnits, refer to freed memory "
                "afterwards and must not be used: get them again" )
            .def( "set_horizon_implicit_value",
                &ImplicitStructuralModelBuilder::set_horizon_implicit_value,
                pybind11::arg( "horizon" ), pybind11::arg( "isovalue" ),
                "Set the implicit value (isovalue) of the Horizon; raise an "
                "error if the Horizon is not in the HorizonsStack" )
            .def( "horizons_stack_builder",
                &ImplicitStructuralModelBuilder::horizons_stack_builder,
                pybind11::keep_alive< 0, 1 >(),
                "Return the builder of the HorizonsStack of the model, "
                "which keeps this builder alive" )
            .def( "reinitialize_implicit_query_trees",
                &ImplicitStructuralModelBuilder::
                    reinitialize_implicit_query_trees,
                "Rebuild the query trees on the implicit values" )
            .def( "import_old_implicit_attribute_values_from_attribute_name",
                &ImplicitStructuralModelBuilder::
                    import_old_implicit_attribute_values_from_attribute_name,
                pybind11::arg( "old_implicit_attribute_name" ),
                "In each Block having the implicit attribute, copy the values "
                "of "
                "the first attribute whose name matches into the implicit "
                "attribute, then delete the old attribute" );
    }
} // namespace geode
