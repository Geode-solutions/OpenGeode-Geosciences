/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>

#include <geode/geometry/point.hpp>

#include <geode/model/mixin/core/block.hpp>

namespace geode
{
    void define_implicit_structural_model( pybind11::module& module )
    {
        pybind11::class_< ImplicitStructuralModel, StructuralModel >(
            module, "ImplicitStructuralModel" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( StructuralModel& model ) {
                return ImplicitStructuralModel{ model.clone() };
            } ) )
            .def(
                "native_extension", &ImplicitStructuralModel::native_extension )
            .def( "implicit_value_from_vertex_id",
                static_cast< double ( ImplicitStructuralModel::* )(
                    const Block3D&, index_t ) const >(
                    &ImplicitStructuralModel::implicit_value ) )
            .def( "implicit_value_from_geometric_point",
                static_cast< std::optional< double > (
                    ImplicitStructuralModel::* )(
                    const Block3D&, const Point3D& ) const >(
                    &ImplicitStructuralModel::implicit_value ) )
            .def( "implicit_value_from_geometric_point_and_tetra_id",
                static_cast< double ( ImplicitStructuralModel::* )(
                    const Block3D&, const Point3D&, index_t ) const >(
                    &ImplicitStructuralModel::implicit_value ) )
            .def( "horizons_stack", &ImplicitStructuralModel::horizons_stack,
                pybind11::return_value_policy::reference )
            .def( "horizon_implicit_value",
                &ImplicitStructuralModel::horizon_implicit_value )
            .def( "implicit_value_is_above_horizon",
                &ImplicitStructuralModel::implicit_value_is_above_horizon )
            .def( "containing_stratigraphic_unit",
                &ImplicitStructuralModel::containing_stratigraphic_unit );
    }
} // namespace geode
