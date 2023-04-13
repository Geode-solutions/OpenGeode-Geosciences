/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
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
