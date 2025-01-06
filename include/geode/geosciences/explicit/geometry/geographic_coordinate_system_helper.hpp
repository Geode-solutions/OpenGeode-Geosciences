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

#pragma once

#include <geode/geosciences/explicit/common.hpp>
#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurveBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSetBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMeshBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMeshBuilder );
    class BRep;
    class BRepBuilder;
    class Section;
    class SectionBuilder;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    void assign_edged_curve_geographic_coordinate_system_info(
        const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    template < index_t dimension >
    void assign_point_set_geographic_coordinate_system_info(
        const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    template < index_t dimension >
    void assign_solid_mesh_geographic_coordinate_system_info(
        const SolidMesh< dimension >& mesh,
        SolidMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    template < index_t dimension >
    void assign_surface_mesh_geographic_coordinate_system_info(
        const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    void opengeode_geosciences_explicit_api
        assign_brep_geographic_coordinate_system_info( const BRep& brep,
            BRepBuilder& builder,
            std::string_view crs_name,
            const GeographicCoordinateSystem3D::Info& info );

    void opengeode_geosciences_explicit_api
        assign_section_geographic_coordinate_system_info(
            const Section& section,
            SectionBuilder& builder,
            std::string_view crs_name,
            const GeographicCoordinateSystem2D::Info& info );

    template < index_t dimension >
    void convert_edged_curve_coordinate_reference_system(
        const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    template < index_t dimension >
    void convert_point_set_coordinate_reference_system(
        const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    template < index_t dimension >
    void convert_solid_mesh_coordinate_reference_system(
        const SolidMesh< dimension >& mesh,
        SolidMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    template < index_t dimension >
    void convert_surface_mesh_coordinate_reference_system(
        const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    void opengeode_geosciences_explicit_api
        convert_brep_coordinate_reference_system( const BRep& brep,
            BRepBuilder& builder,
            std::string_view crs_name,
            const GeographicCoordinateSystem3D::Info& info );

    void opengeode_geosciences_explicit_api
        convert_section_coordinate_reference_system( const Section& section,
            SectionBuilder& builder,
            std::string_view crs_name,
            const GeographicCoordinateSystem2D::Info& info );
} // namespace geode