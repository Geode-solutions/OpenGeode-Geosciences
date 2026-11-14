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

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>
#include <geode/mesh/core/coordinate_reference_system_manager.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/light_regular_grid.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.hpp>
#include <geode/geosciences/explicit/geometry/geographic_coordinate_system_helper.hpp>

namespace geode
{
    namespace detail
    {
        constexpr auto assign_mesh_doc =
            "Replace the active coordinate reference system of the mesh, "
            "whose points must be stored in an attribute, by a "
            "GeographicCoordinateSystem described by info and named "
            "crs_name (the name of the active system, or a new name), "
            "keeping its points. Raise an error if the active system is not "
            "stored in an attribute or if another system already has the "
            "name crs_name";
        constexpr auto convert_mesh_doc =
            "Register and activate a new GeographicCoordinateSystem named "
            "crs_name and described by info, whose points are those of the "
            "active GeographicCoordinateSystem converted to info. Raise an "
            "error if a system named crs_name already exists, if the active "
            "system is not a GeographicCoordinateSystem (a mesh without "
            "vertex is not checked) or if a point cannot be converted";
        constexpr auto assign_model_doc =
            "Replace the active coordinate reference system of the mesh of "
            "every component of the model, whose points must be stored in an "
            "attribute, by a GeographicCoordinateSystem described by info and "
            "named crs_name (the name of the active system, or a new name), "
            "keeping its points. Raise an error, before any change, if the "
            "active system of one component is not stored in an attribute or "
            "if another system already has the name crs_name";
        constexpr auto convert_model_doc =
            "Register and activate a new GeographicCoordinateSystem named "
            "crs_name and described by info on the mesh of every component "
            "of the model, whose points are those of the active "
            "GeographicCoordinateSystem converted to info. Raise an error, "
            "before any change, if a system named crs_name already exists on "
            "a component, if its active system is not a "
            "GeographicCoordinateSystem (a mesh without vertex is not "
            "checked) or if a point cannot be converted";
        constexpr auto assign_grid_doc =
            "Replace the active coordinate reference system of the grid by a "
            "GeographicGridCoordinateSystem with the same coordinate system "
            "(origin and directions), described by info and named crs_name "
            "(the name of the active system, or a new name). Raise an error "
            "if another system already has the name crs_name";
        constexpr auto convert_grid_doc =
            "Register and activate a new GeographicGridCoordinateSystem named "
            "crs_name and described by info, converted from the active one: "
            "the origin and the far end of each axis are converted and the "
            "grid stays regular, which is exact only for affine conversions. "
            "Raise an error if a system named crs_name already exists, if "
            "the active system is not a GeographicGridCoordinateSystem or if "
            "a point cannot be converted";
    } // namespace detail
} // namespace geode

#define PYTHON_MESH_CRS_HELPER( mesh, dimension )                              \
    const auto assign##mesh##dimension = absl::StrCat( "assign_", #mesh,       \
        "_geographic_coordinate_system_info", dimension, "D" );                \
    module.def( assign##mesh##dimension.c_str(),                               \
        &assign_##mesh##_geographic_coordinate_system_info< dimension >,       \
        pybind11::arg( "mesh" ), pybind11::arg( "builder" ),                   \
        pybind11::arg( "crs_name" ), pybind11::arg( "info" ),                  \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        detail::assign_mesh_doc );                                             \
    const auto convert##mesh##dimension = absl::StrCat(                        \
        "convert_", #mesh, "_coordinate_reference_system", dimension, "D" );   \
    module.def( convert##mesh##dimension.c_str(),                              \
        &convert_##mesh##_coordinate_reference_system< dimension >,            \
        pybind11::arg( "mesh" ), pybind11::arg( "builder" ),                   \
        pybind11::arg( "crs_name" ), pybind11::arg( "info" ),                  \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        detail::convert_mesh_doc )

#define PYTHON_GRID_CRS_HELPER( dimension )                                    \
    const auto assign_grid##dimension = absl::StrCat(                          \
        "assign_light_regular_grid_geographic_coordinate_system_info",         \
        dimension, "D" );                                                      \
    module.def( assign_grid##dimension.c_str(),                                \
        &assign_light_regular_grid_geographic_coordinate_system_info<          \
            dimension >,                                                       \
        pybind11::arg( "grid" ), pybind11::arg( "crs_name" ),                  \
        pybind11::arg( "info" ),                                               \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        detail::assign_grid_doc );                                             \
    const auto convert_grid##dimension = absl::StrCat(                         \
        "convert_light_regular_grid_coordinate_reference_system", dimension,   \
        "D" );                                                                 \
    module.def( convert_grid##dimension.c_str(),                               \
        &convert_light_regular_grid_coordinate_reference_system< dimension >,  \
        pybind11::arg( "grid" ), pybind11::arg( "crs_name" ),                  \
        pybind11::arg( "info" ),                                               \
        pybind11::call_guard< pybind11::gil_scoped_release >(),                \
        detail::convert_grid_doc )

namespace geode
{
    void define_crs_helper( pybind11::module& module )
    {
        PYTHON_MESH_CRS_HELPER( edged_curve, 2 );
        PYTHON_MESH_CRS_HELPER( edged_curve, 3 );
        PYTHON_GRID_CRS_HELPER( 2 );
        PYTHON_GRID_CRS_HELPER( 3 );
        PYTHON_MESH_CRS_HELPER( point_set, 2 );
        PYTHON_MESH_CRS_HELPER( point_set, 3 );
        PYTHON_MESH_CRS_HELPER( solid_mesh, 3 );
        PYTHON_MESH_CRS_HELPER( surface_mesh, 2 );
        PYTHON_MESH_CRS_HELPER( surface_mesh, 3 );
        module
            .def( "assign_brep_geographic_coordinate_system_info",
                &assign_brep_geographic_coordinate_system_info,
                pybind11::arg( "brep" ), pybind11::arg( "builder" ),
                pybind11::arg( "crs_name" ), pybind11::arg( "info" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                detail::assign_model_doc )
            .def( "assign_section_geographic_coordinate_system_info",
                &assign_section_geographic_coordinate_system_info,
                pybind11::arg( "section" ), pybind11::arg( "builder" ),
                pybind11::arg( "crs_name" ), pybind11::arg( "info" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                detail::assign_model_doc );
        module
            .def( "convert_brep_coordinate_reference_system",
                &convert_brep_coordinate_reference_system,
                pybind11::arg( "brep" ), pybind11::arg( "builder" ),
                pybind11::arg( "crs_name" ), pybind11::arg( "info" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                detail::convert_model_doc )
            .def( "convert_section_coordinate_reference_system",
                &convert_section_coordinate_reference_system,
                pybind11::arg( "section" ), pybind11::arg( "builder" ),
                pybind11::arg( "crs_name" ), pybind11::arg( "info" ),
                pybind11::call_guard< pybind11::gil_scoped_release >(),
                detail::convert_model_doc );
    }
} // namespace geode
