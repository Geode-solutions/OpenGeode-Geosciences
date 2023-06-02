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

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system_helper.h>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>
#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/builder/solid_mesh_builder.h>
#include <geode/mesh/builder/surface_mesh_builder.h>
#include <geode/mesh/core/attribute_coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system_manager.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/brep_builder.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    template < typename Mesh >
    void convert_coordinate_reference_system( const Mesh& mesh,
        typename Mesh::Builder& builder,
        absl::string_view crs_name,
        typename geode::GeographicCoordinateSystem< Mesh::dim >::Info info )
    {
        using GeoCRS = typename geode::GeographicCoordinateSystem< Mesh::dim >;
        const auto& crs_manager =
            mesh.main_coordinate_reference_system_manager();
        OPENGEODE_EXCEPTION(
            !crs_manager.coordinate_reference_system_exists( crs_name ),
            "[convert_components_coordinate_reference_system] New CRS name "
            "already exists" );
        auto crs_builder =
            builder.main_coordinate_reference_system_manager_builder();
        auto crs =
            std::make_shared< GeoCRS >( mesh.vertex_attribute_manager(), info );
        if( mesh.nb_vertices() != 0 )
        {
            OPENGEODE_EXCEPTION(
                crs_manager.active_coordinate_reference_system().type_name()
                    == GeoCRS::type_name_static(),
                "[convert_components_coordinate_reference_system] Only "
                "GeographicCoordinateSystem conversion is supported" );
            crs->import_coordinates( dynamic_cast< const GeoCRS& >(
                crs_manager.active_coordinate_reference_system() ) );
        }
        crs_builder.register_coordinate_reference_system(
            crs_name, std::move( crs ) );
        crs_builder.set_active_coordinate_reference_system( crs_name );
    }

    template < geode::index_t dimension,
        typename Range,
        typename GetMeshBuilder >
    void convert_components_coordinate_reference_system(
        const typename geode::GeographicCoordinateSystem< dimension >::Info&
            info,
        absl::string_view crs_name,
        Range range,
        GetMeshBuilder get_mesh_builder )
    {
        for( const auto& component : range )
        {
            const auto& mesh = component.mesh();
            auto builder = get_mesh_builder( component.id() );
            convert_coordinate_reference_system(
                mesh, *builder, crs_name, info );
        }
    }

    template < typename Mesh >
    void convert_attribute_to_geographic_coordinate_reference_system(
        const Mesh& mesh,
        typename Mesh::Builder& builder,
        absl::string_view crs_name,
        typename geode::GeographicCoordinateSystem< Mesh::dim >::Info info )
    {
        const auto& crs_manager =
            mesh.main_coordinate_reference_system_manager();
        const auto old_crs_name =
            crs_manager.active_coordinate_reference_system_name();
        const auto& attribute_crs = dynamic_cast<
            const geode::AttributeCoordinateReferenceSystem< Mesh::dim >& >(
            crs_manager.find_coordinate_reference_system( old_crs_name ) );
        auto& attribute_manager = mesh.vertex_attribute_manager();
        attribute_manager.rename_attribute(
            attribute_crs.attribute_name(), info.name );
        auto crs_builder =
            builder.main_coordinate_reference_system_manager_builder();
        crs_builder.register_coordinate_reference_system( crs_name,
            std::make_shared< geode::GeographicCoordinateSystem< Mesh::dim > >(
                attribute_manager, std::move( info ) ) );
        crs_builder.delete_coordinate_reference_system( old_crs_name );
        crs_builder.set_active_coordinate_reference_system( crs_name );
    }

    template < geode::index_t dimension,
        typename Range,
        typename GetMeshBuilder >
    void convert_components_attribute_to_geographic_coordinate_reference_system(
        const typename geode::GeographicCoordinateSystem< dimension >::Info&
            info,
        absl::string_view crs_name,
        Range range,
        GetMeshBuilder get_mesh_builder )
    {
        for( const auto& component : range )
        {
            const auto& mesh = component.mesh();
            auto builder = get_mesh_builder( component.id() );
            convert_attribute_to_geographic_coordinate_reference_system(
                mesh, *builder, crs_name, info );
        }
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    void assign_edged_curve_geographic_coordinate_system_info(
        const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void assign_point_set_geographic_coordinate_system_info(
        const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void assign_solid_mesh_geographic_coordinate_system_info(
        const SolidMesh< dimension >& mesh,
        SolidMeshBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void assign_surface_mesh_geographic_coordinate_system_info(
        const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    void assign_brep_geographic_coordinate_system_info( const BRep& brep,
        BRepBuilder& builder,
        absl::string_view crs_name,
        const GeographicCoordinateSystem3D::Info& info )
    {
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >( info, crs_name, brep.corners(), [&builder]( const uuid& id ) {
            return builder.corner_mesh_builder( id );
        } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >( info, crs_name, brep.lines(), [&builder]( const uuid& id ) {
            return builder.line_mesh_builder( id );
        } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >( info, crs_name, brep.surfaces(), [&builder]( const uuid& id ) {
            return builder.surface_mesh_builder( id );
        } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >( info, crs_name, brep.blocks(), [&builder]( const uuid& id ) {
            return builder.block_mesh_builder( id );
        } );
    }

    void assign_section_geographic_coordinate_system_info(
        const Section& section,
        SectionBuilder& builder,
        absl::string_view crs_name,
        const GeographicCoordinateSystem2D::Info& info )
    {
        convert_components_attribute_to_geographic_coordinate_reference_system<
            2 >(
            info, crs_name, section.corners(), [&builder]( const uuid& id ) {
                return builder.corner_mesh_builder( id );
            } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            2 >( info, crs_name, section.lines(), [&builder]( const uuid& id ) {
            return builder.line_mesh_builder( id );
        } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            2 >(
            info, crs_name, section.surfaces(), [&builder]( const uuid& id ) {
                return builder.surface_mesh_builder( id );
            } );
    }

    template < index_t dimension >
    void convert_edged_curve_coordinate_reference_system(
        const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void convert_point_set_coordinate_reference_system(
        const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void convert_solid_mesh_coordinate_reference_system(
        const SolidMesh< dimension >& mesh,
        SolidMeshBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void convert_surface_mesh_coordinate_reference_system(
        const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder,
        absl::string_view crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    void convert_brep_coordinate_reference_system( const BRep& brep,
        BRepBuilder& builder,
        absl::string_view crs_name,
        const GeographicCoordinateSystem3D::Info& info )
    {
        convert_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.corners(), [&builder]( const uuid& id ) {
                return builder.corner_mesh_builder( id );
            } );
        convert_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.lines(), [&builder]( const uuid& id ) {
                return builder.line_mesh_builder( id );
            } );
        convert_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.surfaces(), [&builder]( const uuid& id ) {
                return builder.surface_mesh_builder( id );
            } );
        convert_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.blocks(), [&builder]( const uuid& id ) {
                return builder.block_mesh_builder( id );
            } );
    }

    void convert_section_coordinate_reference_system( const Section& section,
        SectionBuilder& builder,
        absl::string_view crs_name,
        const GeographicCoordinateSystem2D::Info& info )
    {
        convert_components_coordinate_reference_system< 2 >(
            info, crs_name, section.corners(), [&builder]( const uuid& id ) {
                return builder.corner_mesh_builder( id );
            } );
        convert_components_coordinate_reference_system< 2 >(
            info, crs_name, section.lines(), [&builder]( const uuid& id ) {
                return builder.line_mesh_builder( id );
            } );
        convert_components_coordinate_reference_system< 2 >(
            info, crs_name, section.surfaces(), [&builder]( const uuid& id ) {
                return builder.surface_mesh_builder( id );
            } );
    }

    template void opengeode_geosciences_explicit_api
        assign_edged_curve_geographic_coordinate_system_info(
            const EdgedCurve< 2 >&,
            EdgedCurveBuilder< 2 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );
    template void opengeode_geosciences_explicit_api
        assign_point_set_geographic_coordinate_system_info(
            const PointSet< 2 >&,
            PointSetBuilder< 2 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );
    template void opengeode_geosciences_explicit_api
        assign_surface_mesh_geographic_coordinate_system_info(
            const SurfaceMesh< 2 >&,
            SurfaceMeshBuilder< 2 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_edged_curve_coordinate_reference_system( const EdgedCurve< 2 >&,
            EdgedCurveBuilder< 2 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_point_set_coordinate_reference_system( const PointSet< 2 >&,
            PointSetBuilder< 2 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_surface_mesh_coordinate_reference_system(
            const SurfaceMesh< 2 >&,
            SurfaceMeshBuilder< 2 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );

    template void opengeode_geosciences_explicit_api
        assign_edged_curve_geographic_coordinate_system_info(
            const EdgedCurve< 3 >&,
            EdgedCurveBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
    template void opengeode_geosciences_explicit_api
        assign_point_set_geographic_coordinate_system_info(
            const PointSet< 3 >&,
            PointSetBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
    template void opengeode_geosciences_explicit_api
        assign_solid_mesh_geographic_coordinate_system_info(
            const SolidMesh< 3 >&,
            SolidMeshBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
    template void opengeode_geosciences_explicit_api
        assign_surface_mesh_geographic_coordinate_system_info(
            const SurfaceMesh< 3 >&,
            SurfaceMeshBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_edged_curve_coordinate_reference_system( const EdgedCurve< 3 >&,
            EdgedCurveBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_point_set_coordinate_reference_system( const PointSet< 3 >&,
            PointSetBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_solid_mesh_coordinate_reference_system( const SolidMesh< 3 >&,
            SolidMeshBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_surface_mesh_coordinate_reference_system(
            const SurfaceMesh< 3 >&,
            SurfaceMeshBuilder< 3 >&,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
} // namespace geode