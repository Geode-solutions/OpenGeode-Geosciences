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

#include <geode/basic/attribute_manager.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>
#include <geode/mesh/core/coordinate_reference_system_manager.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.hpp>
#include <geode/geosciences/explicit/geometry/geographic_coordinate_system_helper.hpp>

#define PYTHON_MESH_CRS_HELPER( mesh, dimension )                              \
    const auto assign##mesh##dimension = absl::StrCat( "assign_", #mesh,       \
        "_geographic_coordinate_system_info", dimension, "D" );                \
    module.def( assign##mesh##dimension.c_str(),                               \
        &assign_##mesh##_geographic_coordinate_system_info< dimension > );     \
    const auto convert##mesh##dimension = absl::StrCat(                        \
        "convert_", #mesh, "_coordinate_reference_system", dimension, "D" );   \
    module.def( convert##mesh##dimension.c_str(),                              \
        &convert_##mesh##_coordinate_reference_system< dimension > )

namespace geode
{
    void define_crs_helper( pybind11::module& module )
    {
        PYTHON_MESH_CRS_HELPER( edged_curve, 2 );
        PYTHON_MESH_CRS_HELPER( edged_curve, 3 );
        PYTHON_MESH_CRS_HELPER( point_set, 2 );
        PYTHON_MESH_CRS_HELPER( point_set, 3 );
        PYTHON_MESH_CRS_HELPER( solid_mesh, 3 );
        PYTHON_MESH_CRS_HELPER( surface_mesh, 2 );
        PYTHON_MESH_CRS_HELPER( surface_mesh, 3 );
        module
            .def( "assign_brep_geographic_coordinate_system_info",
                &assign_brep_geographic_coordinate_system_info )
            .def( "assign_section_geographic_coordinate_system_info",
                &assign_section_geographic_coordinate_system_info );
        module
            .def( "convert_brep_coordinate_reference_system",
                &convert_brep_coordinate_reference_system )
            .def( "convert_section_coordinate_reference_system",
                &convert_section_coordinate_reference_system );
    }
} // namespace geode
