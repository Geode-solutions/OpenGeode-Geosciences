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
        using GeoCRS = typename geode::GeographicCoordinateSystem< dimension >;
        for( const auto& component : range )
        {
            const auto& mesh = component.mesh();
            const auto& crs_manager =
                mesh.main_coordinate_reference_system_manager();
            OPENGEODE_EXCEPTION(
                !crs_manager.coordinate_reference_system_exists( crs_name ),
                "[convert_components_coordinate_reference_system] New CRS name "
                "already exists" );
            auto crs_builder =
                get_mesh_builder( component.id() )
                    ->main_coordinate_reference_system_manager_builder();
            auto crs = std::make_shared< GeoCRS >(
                mesh.vertex_attribute_manager(), info );
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
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    void convert_attribute_to_geographic_coordinate_reference_system(
        CoordinateReferenceSystemManager< dimension >& crs_manager,
        AttributeManager& attribute_manager,
        absl::string_view attribute_crs_name,
        absl::string_view geographic_crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        const auto is_active =
            crs_manager.active_coordinate_reference_system_name()
            == attribute_crs_name;
        const auto& attribute_crs = dynamic_cast<
            const AttributeCoordinateReferenceSystem< dimension >& >(
            crs_manager.find_coordinate_reference_system(
                attribute_crs_name ) );
        attribute_manager.rename_attribute(
            attribute_crs.attribute_name(), info.name );
        CoordinateReferenceSystemManagerBuilder< dimension > crs_builder{
            crs_manager
        };
        crs_builder.register_coordinate_reference_system( geographic_crs_name,
            std::make_shared< GeographicCoordinateSystem< dimension > >(
                attribute_manager, std::move( info ) ) );
        crs_builder.delete_coordinate_reference_system( attribute_crs_name );
        if( is_active )
        {
            crs_builder.set_active_coordinate_reference_system(
                geographic_crs_name );
        }
    }

    void convert_brep_coordinate_reference_system( const BRep& brep,
        BRepBuilder& builder,
        const GeographicCoordinateSystem3D::Info& info,
        absl::string_view crs_name )
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
        const GeographicCoordinateSystem2D::Info& info,
        absl::string_view crs_name )
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
        convert_attribute_to_geographic_coordinate_reference_system(
            CoordinateReferenceSystemManager< 2 >&,
            AttributeManager&,
            absl::string_view,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );
    template void opengeode_geosciences_explicit_api
        convert_attribute_to_geographic_coordinate_reference_system(
            CoordinateReferenceSystemManager< 3 >&,
            AttributeManager&,
            absl::string_view,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
} // namespace geode