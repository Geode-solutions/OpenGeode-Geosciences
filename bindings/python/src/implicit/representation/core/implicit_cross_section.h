/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>

#include <geode/geometry/point.h>

namespace geode
{
    void define_implicit_cross_section( pybind11::module& module )
    {
        pybind11::class_< ImplicitCrossSection, CrossSection >(
            module, "ImplicitCrossSection" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( CrossSection& cross_section ) {
                return ImplicitCrossSection{ std::move( cross_section ) };
            } ) )
            .def( "native_extension", &ImplicitCrossSection::native_extension )
            .def( "implicit_value_from_vertex_id",
                ( double( ImplicitCrossSection::* )( const Surface2D&, index_t )
                        const )
                    & ImplicitCrossSection::implicit_value )
            .def( "implicit_value_from_geometric_point",
                ( absl::optional< double >( ImplicitCrossSection::* )(
                    const Surface2D&, const Point2D& ) const )
                    & ImplicitCrossSection::implicit_value )
            .def( "implicit_value_from_geometric_point_and_triangle_"
                  "id",
                ( double( ImplicitCrossSection::* )(
                    const Surface2D&, const Point2D&, index_t ) const )
                    & ImplicitCrossSection::implicit_value )
            .def( "stratigraphic_units_stack",
                &ImplicitCrossSection::stratigraphic_units_stack )
            .def( "horizon_implicit_value",
                &ImplicitCrossSection::horizon_implicit_value );
    }
} // namespace geode
