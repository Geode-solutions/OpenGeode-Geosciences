/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/triangulated_surface.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>

namespace geode
{
    void define_stratigraphic_model( pybind11::module& module )
    {
        pybind11::class_< StratigraphicModel, ImplicitStructuralModel >(
            module, "StratigraphicModel" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( ImplicitStructuralModel& model ) {
                return StratigraphicModel{ std::move( model.clone() ) };
            } ) )
            .def( pybind11::init( []( StructuralModel& model ) {
                return StratigraphicModel{ std::move( model.clone() ) };
            } ) )
            .def( "stratigraphic_coordinates_from_vertex_id",
                static_cast< StratigraphicPoint3D ( StratigraphicModel::* )(
                    const Block3D&, index_t ) const >(
                    &StratigraphicModel::stratigraphic_coordinates ) )
            .def( "stratigraphic_coordinates_from_geometric_point",
                static_cast< absl::optional< StratigraphicPoint3D > (
                    StratigraphicModel::* )( const Block3D&, const Point3D& )
                        const >(
                    &StratigraphicModel::stratigraphic_coordinates ) )
            .def( "stratigraphic_coordinates_from_geometric_point_and_tetra_id",
                static_cast< StratigraphicPoint3D ( StratigraphicModel::* )(
                    const Block3D&, const Point3D&, index_t ) const >(
                    &StratigraphicModel::stratigraphic_coordinates ) )
            .def( "geometric_coordinates_from_stratigraphic_point",
                static_cast< absl::optional< Point3D > (
                    StratigraphicModel::* )(
                    const Block3D&, const StratigraphicPoint3D& ) const >(
                    &StratigraphicModel::geometric_coordinates ) )
            .def( "geometric_coordinates_from_stratigraphic_point_and_tetra_id",
                static_cast< Point3D ( StratigraphicModel::* )( const Block3D&,
                    const StratigraphicPoint3D&, index_t ) const >(
                    &StratigraphicModel::geometric_coordinates ) )
            .def( "stratigraphic_surface",
                &StratigraphicModel::stratigraphic_surface )
            .def( "stratigraphic_bounding_box",
                &StratigraphicModel::stratigraphic_bounding_box )
            .def( "native_extension", &StratigraphicModel::native_extension );
    }
} // namespace geode
