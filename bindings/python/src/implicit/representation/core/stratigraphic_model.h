/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
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
                return StratigraphicModel{ std::move( model ) };
            } ) )
            .def( pybind11::init( []( StructuralModel& model ) {
                return StratigraphicModel{ std::move( model ) };
            } ) )
            .def( "stratigraphic_coordinates_from_vertex_id",
                ( StratigraphicPoint3D( StratigraphicModel::* )(
                    const Block3D&, index_t ) const )
                    & StratigraphicModel::stratigraphic_coordinates )
            .def( "stratigraphic_coordinates_from_geometric_point",
                ( absl::optional< StratigraphicPoint3D >(
                    StratigraphicModel::* )( const Block3D&, const Point3D& )
                        const )
                    & StratigraphicModel::stratigraphic_coordinates )
            .def( "stratigraphic_coordinates_from_geometric_point_and_tetra_id",
                ( StratigraphicPoint3D( StratigraphicModel::* )(
                    const Block3D&, const Point3D&, index_t ) const )
                    & StratigraphicModel::stratigraphic_coordinates )
            .def( "geometric_coordinates_from_stratigraphic_point",
                ( absl::optional< Point3D >( StratigraphicModel::* )(
                    const Block3D&, const StratigraphicPoint3D& ) const )
                    & StratigraphicModel::geometric_coordinates )
            .def( "geometric_coordinates_from_stratigraphic_point_and_tetra_id",
                ( Point3D( StratigraphicModel::* )( const Block3D&,
                    const StratigraphicPoint3D&, index_t ) const )
                    & StratigraphicModel::geometric_coordinates )
            .def( "stratigraphic_surface",
                &StratigraphicModel::stratigraphic_surface )
            .def( "stratigraphic_bounding_box",
                &StratigraphicModel::stratigraphic_bounding_box )
            .def( "native_extension", &StratigraphicModel::native_extension );
    }
} // namespace geode
