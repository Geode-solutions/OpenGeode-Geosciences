/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/edged_curve.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

namespace geode
{
    void define_stratigraphic_section( pybind11::module& module )
    {
        pybind11::class_< StratigraphicSection, ImplicitCrossSection >(
            module, "StratigraphicSection" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( ImplicitCrossSection& cross_section ) {
                return StratigraphicSection{ std::move( cross_section ) };
            } ) )
            .def( pybind11::init( []( CrossSection& cross_section ) {
                return StratigraphicSection{ std::move( cross_section ) };
            } ) )
            .def( "stratigraphic_coordinates_from_vertex_id",
                ( StratigraphicPoint2D( StratigraphicSection::* )(
                    const Surface2D&, index_t ) const )
                    & StratigraphicSection::stratigraphic_coordinates )
            .def( "stratigraphic_coordinates_from_geometric_point",
                ( absl::optional< StratigraphicPoint2D >(
                    StratigraphicSection::* )(
                    const Surface2D&, const Point2D& ) const )
                    & StratigraphicSection::stratigraphic_coordinates )
            .def( "stratigraphic_coordinates_from_geometric_point_and_triangle_"
                  "id",
                ( StratigraphicPoint2D( StratigraphicSection::* )(
                    const Surface2D&, const Point2D&, index_t ) const )
                    & StratigraphicSection::stratigraphic_coordinates )
            .def( "geometric_coordinates_from_stratigraphic_point",
                ( absl::optional< Point2D >( StratigraphicSection::* )(
                    const Surface2D&, const StratigraphicPoint2D& ) const )
                    & StratigraphicSection::geometric_coordinates )
            .def( "geometric_coordinates_from_stratigraphic_point_and_triangle_"
                  "id",
                ( Point2D( StratigraphicSection::* )( const Surface2D&,
                    const StratigraphicPoint2D&, index_t ) const )
                    & StratigraphicSection::geometric_coordinates )
            .def( "stratigraphic_line",
                &StratigraphicSection::stratigraphic_line )
            .def( "stratigraphic_bounding_box",
                &StratigraphicSection::stratigraphic_bounding_box )
            .def( "native_extension", &StratigraphicSection::native_extension );
    }
} // namespace geode
