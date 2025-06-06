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

#include <geode/basic/attribute_manager.hpp>

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.hpp>

#define PYTHON_CRS( dimension )                                                \
    const auto info##dimension =                                               \
        "GeographicCoordinateSystemInfo" + std::to_string( dimension ) + "D";  \
    pybind11::class_< GeographicCoordinateSystem##dimension##D::Info >(        \
        module, info##dimension.c_str() )                                      \
        .def( pybind11::init< std::string, std::string, std::string >() )      \
        .def( "authority_code",                                                \
            &GeographicCoordinateSystem##dimension##D::Info::authority_code )  \
        .def( "string",                                                        \
            &GeographicCoordinateSystem##dimension##D::Info::string )          \
        .def_readwrite( "authority",                                           \
            &GeographicCoordinateSystem##dimension##D::Info::authority )       \
        .def_readwrite(                                                        \
            "code", &GeographicCoordinateSystem##dimension##D::Info::code )    \
        .def_readwrite(                                                        \
            "name", &GeographicCoordinateSystem##dimension##D::Info::name );   \
    const auto name##dimension =                                               \
        "GeographicCoordinateSystem" + std::to_string( dimension ) + "D";      \
    pybind11::class_< GeographicCoordinateSystem##dimension##D,                \
        CoordinateReferenceSystem##dimension##D >(                             \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< AttributeManager&,                               \
            GeographicCoordinateSystem##dimension##D::Info >() )               \
        .def( "type_name_static",                                              \
            &GeographicCoordinateSystem##dimension##D::type_name_static )      \
        .def( "info", &GeographicCoordinateSystem##dimension##D::info )        \
        .def( "geographic_coordinate_systems",                                 \
            &GeographicCoordinateSystem##dimension##D::                        \
                geographic_coordinate_systems )                                \
        .def( "import_coordinates",                                            \
            &GeographicCoordinateSystem##dimension##D::import_coordinates )

namespace geode
{
    void define_crs( pybind11::module& module )
    {
        PYTHON_CRS( 2 );
        PYTHON_CRS( 3 );
    }
} // namespace geode
