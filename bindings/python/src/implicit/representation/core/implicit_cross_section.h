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
            .def( "horizons_stack", &ImplicitCrossSection::horizons_stack,
                pybind11::return_value_policy::reference )
            .def( "horizon_implicit_value",
                &ImplicitCrossSection::horizon_implicit_value );
    }
} // namespace geode
