/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geosciences/implicit/representation/core/implicit_structural_model.h>

#include <geode/geometry/point.h>

#include <geode/model/mixin/core/block.h>

namespace geode
{
    void define_implicit_structural_model( pybind11::module& module )
    {
        pybind11::class_< ImplicitStructuralModel, StructuralModel >(
            module, "ImplicitStructuralModel" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( StructuralModel& model ) {
                return ImplicitStructuralModel{ std::move( model ) };
            } ) )
            .def(
                "native_extension", &ImplicitStructuralModel::native_extension )
            .def( "implicit_value_from_vertex_id",
                ( double( ImplicitStructuralModel::* )(
                    const Block3D&, index_t ) const )
                    & ImplicitStructuralModel::implicit_value )
            .def( "implicit_value_from_geometric_point",
                ( absl::optional< double >( ImplicitStructuralModel::* )(
                    const Block3D&, const Point3D& ) const )
                    & ImplicitStructuralModel::implicit_value )
            .def( "implicit_value_from_geometric_point_and_tetra_id",
                ( double( ImplicitStructuralModel::* )(
                    const Block3D&, const Point3D&, index_t ) const )
                    & ImplicitStructuralModel::implicit_value )
            .def( "stratigraphic_units_stack",
                &ImplicitStructuralModel::stratigraphic_units_stack )
            .def( "horizon_implicit_value",
                &ImplicitStructuralModel::horizon_implicit_value );
    }
} // namespace geode
