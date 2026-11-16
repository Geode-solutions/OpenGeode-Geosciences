/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#pragma once

#include <geode/basic/attribute_manager.hpp>

#include <geode/geometry/coordinate_system.hpp>

#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.hpp>

#define PYTHON_CRS( dimension )                                                \
    const auto name##dimension =                                               \
        "GeographicCoordinateSystem" + std::to_string( dimension ) + "D";      \
    pybind11::class_< GeographicCoordinateSystem##dimension##D,                \
        CoordinateReferenceSystem##dimension##D >( module,                     \
        name##dimension.c_str(),                                               \
        "Coordinate reference system of a mesh whose points are geographic "   \
        "coordinates, stored in an attribute of the vertices and described "   \
        "by a GeographicCoordinateSystemInfo" )                                \
        .def( pybind11::init< AttributeManager&,                               \
                  GeographicCoordinateSystemInfo >(),                          \
            pybind11::arg( "manager" ), pybind11::arg( "info" ),               \
            pybind11::keep_alive< 1, 2 >(),                                    \
            "Build a coordinate reference system whose points are stored in "  \
            "an attribute of the AttributeManager (the vertex attribute "      \
            "manager of the mesh), which is kept alive by the system" )        \
        .def_static( "type_name_static",                                       \
            &GeographicCoordinateSystem##dimension##D::type_name_static,       \
            "Return the CRSType of the geographic coordinate systems" )        \
        .def( "info", &GeographicCoordinateSystem##dimension##D::info,         \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the GeographicCoordinateSystemInfo of the system" )        \
        .def_static( "geographic_coordinate_systems",                          \
            &GeographicCoordinateSystem##dimension##D::                        \
                geographic_coordinate_systems,                                 \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            "Return the GeographicCoordinateSystemInfo of every geographic "   \
            "coordinate system known by the PROJ database" )                   \
        .def(                                                                  \
            "import_coordinates",                                              \
            []( GeographicCoordinateSystem##dimension##D& self,                \
                const GeographicCoordinateSystem##dimension##D& crs ) {        \
                if( crs.nb_points() > self.nb_points() )                       \
                {                                                              \
                    throw pybind11::index_error( absl::StrCat(                 \
                        "[import_coordinates] the other system has ",          \
                        crs.nb_points(), " points, this one only ",            \
                        self.nb_points() ) );                                  \
                }                                                              \
                const pybind11::gil_scoped_release release;                    \
                self.import_coordinates( crs );                                \
            },                                                                 \
            pybind11::arg( "crs" ),                                            \
            "Set the first points of the system to the coordinates of the "    \
            "points of crs, transformed from the geographic coordinate "       \
            "system of crs to the one of this system; raise IndexError if "    \
            "crs has more points than this system, an error if a point "       \
            "cannot be transformed" )

#define PYTHON_GRID_CRS( dimension )                                           \
    const auto grid_name##dimension =                                          \
        "GeographicGridCoordinateSystem" + std::to_string( dimension ) + "D";  \
    pybind11::class_< GeographicGridCoordinateSystem##dimension##D,            \
        GridCoordinateReferenceSystem##dimension##D, pybind11::smart_holder >( \
        module, grid_name##dimension.c_str(),                                  \
        "Coordinate reference system of a light regular grid whose "           \
        "coordinate system (origin and directions) is expressed in a "         \
        "geographic coordinate system" )                                       \
        .def( pybind11::init< CoordinateSystem< dimension >,                   \
                  GeographicCoordinateSystemInfo >(),                          \
            pybind11::arg( "coordinate_system" ), pybind11::arg( "info" ),     \
            "Build a system from the coordinate system (origin and "           \
            "directions) of the grid and the geographic coordinate system "    \
            "it is expressed in" )                                             \
        .def_static( "type_name_static",                                       \
            &GeographicGridCoordinateSystem##dimension##D::type_name_static,   \
            "Return the CRSType of the geographic grid coordinate systems" )   \
        .def( "info", &GeographicGridCoordinateSystem##dimension##D::info,     \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the GeographicCoordinateSystemInfo of the system" )

namespace geode
{
    void define_crs( pybind11::module& module )
    {
        pybind11::class_< GeographicCoordinateSystemInfo > info_class( module,
            "GeographicCoordinateSystemInfo",
            "Identification of a geographic coordinate system: the authority "
            "and code (as EPSG:4326) and its name" );
        info_class.def( pybind11::init<>(), "Build an info with empty fields" )
            .def( pybind11::init< std::string, std::string, std::string >(),
                pybind11::arg( "authority" ), pybind11::arg( "code" ),
                pybind11::arg( "name" ),
                "Build an info from its authority (e.g. EPSG), code (e.g. "
                "4326) and name, without checking that the system exists" )
            .def( "authority_code",
                &GeographicCoordinateSystemInfo::authority_code,
                "Return the authority and the code joined by a colon, e.g. "
                "EPSG:4326" )
            .def( "string", &GeographicCoordinateSystemInfo::string,
                "Return a readable description of the info" )
            .def( "__str__", &GeographicCoordinateSystemInfo::string,
                "Return a readable description of the info" )
            .def(
                "__repr__",
                []( const GeographicCoordinateSystemInfo& info ) {
                    return python_call_repr( "GeographicCoordinateSystemInfo",
                        { python_repr( info.authority ),
                            python_repr( info.code ),
                            python_repr( info.name ) } );
                },
                "Return an unambiguous description of the info, which can be "
                "evaluated" )
            .def(
                "__eq__",
                []( const GeographicCoordinateSystemInfo& info,
                    const GeographicCoordinateSystemInfo& other ) {
                    return info.authority == other.authority
                           && info.code == other.code
                           && info.name == other.name;
                },
                pybind11::arg( "other" ),
                "Return True if the other info has the same authority, code "
                "and name" )
            .def(
                "__hash__",
                []( const GeographicCoordinateSystemInfo& info ) {
                    return pybind11::hash( pybind11::make_tuple(
                        info.authority, info.code, info.name ) );
                },
                "Return a hash of the authority, code and name" )
            .def_static( "from_authority_code",
                &GeographicCoordinateSystemInfo::from_authority_code,
                pybind11::arg( "authority_code" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                "Build the info from an authority code like EPSG:4326, whose "
                "name is read from the PROJ database; a code without "
                "authority (e.g. 4326) is an EPSG code. Raise an error if "
                "the code is unknown" );
        def_readwrite_named( info_class, "authority",
            &GeographicCoordinateSystemInfo::authority,
            "Authority of the system, e.g. EPSG" );
        def_readwrite_named( info_class, "code",
            &GeographicCoordinateSystemInfo::code,
            "Code of the system for its authority, e.g. 4326" );
        def_readwrite_named( info_class, "name",
            &GeographicCoordinateSystemInfo::name,
            "Name of the system, e.g. WGS 84" );
        PYTHON_CRS( 2 );
        PYTHON_CRS( 3 );
        PYTHON_GRID_CRS( 2 );
        PYTHON_GRID_CRS( 3 );
    }
} // namespace geode
