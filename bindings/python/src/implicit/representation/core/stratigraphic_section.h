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
                return StratigraphicSection{ cross_section.clone() };
            } ) )
            .def( pybind11::init( []( CrossSection& cross_section ) {
                return StratigraphicSection{ cross_section.clone() };
            } ) )
            .def( "stratigraphic_coordinates_from_vertex_id",
                static_cast< StratigraphicPoint2D ( StratigraphicSection::* )(
                    const Surface2D&, index_t ) const >(
                    &StratigraphicSection::stratigraphic_coordinates ) )
            .def( "stratigraphic_coordinates_from_geometric_point",
                static_cast< std::optional< StratigraphicPoint2D > (
                    StratigraphicSection::* )(
                    const Surface2D&, const Point2D& ) const >(
                    &StratigraphicSection::stratigraphic_coordinates ) )
            .def( "stratigraphic_coordinates_from_geometric_point_and_triangle_"
                  "id",
                static_cast< StratigraphicPoint2D ( StratigraphicSection::* )(
                    const Surface2D&, const Point2D&, index_t ) const >(
                    &StratigraphicSection::stratigraphic_coordinates ) )
            .def( "geometric_coordinates_from_stratigraphic_point",
                static_cast< std::optional< Point2D > (
                    StratigraphicSection::* )(
                    const Surface2D&, const StratigraphicPoint2D& ) const >(
                    &StratigraphicSection::geometric_coordinates ) )
            .def( "geometric_coordinates_from_stratigraphic_point_and_triangle_"
                  "id",
                static_cast< Point2D ( StratigraphicSection::* )(
                    const Surface2D&, const StratigraphicPoint2D&, index_t )
                        const >(
                    &StratigraphicSection::geometric_coordinates ) )
            .def( "stratigraphic_line",
                &StratigraphicSection::stratigraphic_line )
            .def( "stratigraphic_bounding_box",
                &StratigraphicSection::stratigraphic_bounding_box )
            .def( "native_extension", &StratigraphicSection::native_extension );
    }
} // namespace geode
