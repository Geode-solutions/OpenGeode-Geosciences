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

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system_helper.hpp>

#include <absl/container/flat_hash_map.h>

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

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < geode::index_t dimension >
    using Coordinates = std::vector< geode::Point< dimension > >;

    /*
     * Transformations toward one CRS, created once per source CRS.
     */
    class TransformationCache
    {
    public:
        explicit TransformationCache(
            const geode::GeographicCoordinateSystemInfo& to )
            : to_( to )
        {
        }

        const geode::internal::GeographicCoordinateTransformation&
            transformation( const geode::GeographicCoordinateSystemInfo& from )
        {
            auto authority_code = from.authority_code();
            const auto it = transformations_.find( authority_code );
            if( it != transformations_.end() )
            {
                return it->second;
            }
            return transformations_
                .emplace( std::move( authority_code ),
                    geode::internal::GeographicCoordinateTransformation{
                        from, to_ } )
                .first->second;
        }

    private:
        const geode::GeographicCoordinateSystemInfo& to_;
        absl::flat_hash_map< std::string,
            geode::internal::GeographicCoordinateTransformation >
            transformations_;
    };

    /*
     * Check that the mesh can be converted and return its coordinates in
     * the new CRS. Does not modify the mesh.
     */
    template < typename Mesh >
    Coordinates< Mesh::dim > converted_coordinates( const Mesh& mesh,
        std::string_view crs_name,
        TransformationCache& transformations )
    {
        using GeoCRS = typename geode::GeographicCoordinateSystem< Mesh::dim >;
        const auto& crs_manager =
            mesh.main_coordinate_reference_system_manager();
        geode::OpenGeodeGeosciencesExplicitException::check_exception(
            !crs_manager.coordinate_reference_system_exists( crs_name ),
            nullptr, geode::OpenGeodeException::TYPE::data,
            "[convert_components_coordinate_reference_system] New CRS name "
            "already exists" );
        if( mesh.nb_vertices() == 0 )
        {
            return {};
        }
        const auto* active_crs = dynamic_cast< const GeoCRS* >(
            &crs_manager.active_coordinate_reference_system() );
        geode::OpenGeodeGeosciencesExplicitException::check_exception(
            active_crs != nullptr, nullptr,
            geode::OpenGeodeException::TYPE::data,
            "[convert_components_coordinate_reference_system] Only "
            "GeographicCoordinateSystem conversion is supported" );
        return transformations.transformation( active_crs->info() )
            .transform( *active_crs );
    }

    template < typename Mesh >
    void apply_coordinate_reference_system( const Mesh& mesh,
        typename Mesh::Builder& builder,
        std::string_view crs_name,
        geode::GeographicCoordinateSystemInfo info,
        Coordinates< Mesh::dim > coordinates )
    {
        auto crs =
            std::make_shared< geode::GeographicCoordinateSystem< Mesh::dim > >(
                mesh.vertex_attribute_manager(), std::move( info ) );
        for( const auto p : geode::Indices{ coordinates } )
        {
            crs->set_point( p, std::move( coordinates[p] ) );
        }
        auto crs_builder =
            builder.main_coordinate_reference_system_manager_builder();
        crs_builder.register_coordinate_reference_system(
            crs_name, std::move( crs ) );
        crs_builder.set_active_coordinate_reference_system( crs_name );
    }

    template < typename Mesh >
    void convert_coordinate_reference_system( const Mesh& mesh,
        typename Mesh::Builder& builder,
        std::string_view crs_name,
        geode::GeographicCoordinateSystemInfo info )
    {
        TransformationCache transformations{ info };
        auto coordinates =
            converted_coordinates( mesh, crs_name, transformations );
        apply_coordinate_reference_system( mesh, builder, crs_name,
            std::move( info ), std::move( coordinates ) );
    }

    template < geode::index_t dimension, typename Range >
    std::vector< Coordinates< dimension > > components_converted_coordinates(
        Range range,
        std::string_view crs_name,
        TransformationCache& transformations )
    {
        std::vector< Coordinates< dimension > > coordinates;
        for( const auto& component : range )
        {
            coordinates.emplace_back( converted_coordinates(
                component.mesh(), crs_name, transformations ) );
        }
        return coordinates;
    }

    template < geode::index_t dimension,
        typename Range,
        typename GetMeshBuilder >
    void apply_components_coordinate_reference_system(
        const geode::GeographicCoordinateSystemInfo& info,
        std::string_view crs_name,
        Range range,
        GetMeshBuilder get_mesh_builder,
        std::vector< Coordinates< dimension > > coordinates )
    {
        geode::index_t count{ 0 };
        for( const auto& component : range )
        {
            auto builder = get_mesh_builder( component );
            apply_coordinate_reference_system( component.mesh(), *builder,
                crs_name, info, std::move( coordinates[count++] ) );
        }
    }

    /*
     * Check that the active CRS of the mesh can be assigned to the given
     * CRS name and return the attribute CRS. Does not modify the mesh.
     */
    template < typename Mesh >
    const geode::AttributeCoordinateReferenceSystem< Mesh::dim >&
        attribute_crs_to_assign( const Mesh& mesh, std::string_view crs_name )
    {
        const auto& crs_manager =
            mesh.main_coordinate_reference_system_manager();
        const auto old_crs_name =
            crs_manager.active_coordinate_reference_system_name();
        const auto* attribute_crs = dynamic_cast<
            const geode::AttributeCoordinateReferenceSystem< Mesh::dim >* >(
            &crs_manager.find_coordinate_reference_system( old_crs_name ) );
        geode::OpenGeodeGeosciencesExplicitException::check_exception(
            attribute_crs != nullptr, nullptr,
            geode::OpenGeodeException::TYPE::data,
            "[assign_geographic_coordinate_system_info] Active CRS ",
            old_crs_name, " is not based on an attribute" );
        geode::OpenGeodeGeosciencesExplicitException::check_exception(
            crs_name == old_crs_name
                || !crs_manager.coordinate_reference_system_exists( crs_name ),
            nullptr, geode::OpenGeodeException::TYPE::data,
            "[assign_geographic_coordinate_system_info] CRS named ", crs_name,
            " already exists" );
        return *attribute_crs;
    }

    template < typename Mesh >
    void convert_attribute_to_geographic_coordinate_reference_system(
        const Mesh& mesh,
        typename Mesh::Builder& builder,
        std::string_view crs_name,
        typename geode::GeographicCoordinateSystemInfo info )
    {
        const auto& attribute_crs = attribute_crs_to_assign( mesh, crs_name );
        const auto attribute_id = attribute_crs.attribute_id();
        const auto old_crs_name =
            geode::to_string( mesh.main_coordinate_reference_system_manager()
                    .active_coordinate_reference_system_name() );
        auto& attribute_manager = mesh.vertex_attribute_manager();
        auto attribute =
            attribute_manager.template find_attribute< geode::VariableAttribute,
                geode::Point< Mesh::dim > >( attribute_id );
        geode::IdentifierBuilder attribute_identifier_builder{ *attribute };
        attribute_identifier_builder.set_name( info.name );
        auto crs =
            std::make_shared< geode::GeographicCoordinateSystem< Mesh::dim > >(
                attribute_manager, attribute_id, std::move( info ) );
        auto crs_builder =
            builder.main_coordinate_reference_system_manager_builder();
        if( crs_name == old_crs_name )
        {
            crs_builder.delete_coordinate_reference_system( old_crs_name );
            crs_builder.register_coordinate_reference_system(
                crs_name, std::move( crs ) );
        }
        else
        {
            crs_builder.register_coordinate_reference_system(
                crs_name, std::move( crs ) );
            crs_builder.delete_coordinate_reference_system( old_crs_name );
        }
        crs_builder.set_active_coordinate_reference_system( crs_name );
    }

    template < typename Range >
    void check_components_attribute_to_geographic_coordinate_reference_system(
        Range range, std::string_view crs_name )
    {
        for( const auto& component : range )
        {
            static_cast< void >(
                attribute_crs_to_assign( component.mesh(), crs_name ) );
        }
    }

    template < geode::index_t dimension,
        typename Range,
        typename GetMeshBuilder >
    void convert_components_attribute_to_geographic_coordinate_reference_system(
        const geode::GeographicCoordinateSystemInfo& info,
        std::string_view crs_name,
        Range range,
        GetMeshBuilder get_mesh_builder )
    {
        for( const auto& component : range )
        {
            const auto& mesh = component.mesh();
            auto builder = get_mesh_builder( component );
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
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void assign_point_set_geographic_coordinate_system_info(
        const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder,
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void assign_solid_mesh_geographic_coordinate_system_info(
        const SolidMesh< dimension >& mesh,
        SolidMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void assign_surface_mesh_geographic_coordinate_system_info(
        const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_attribute_to_geographic_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    void assign_brep_geographic_coordinate_system_info( const BRep& brep,
        BRepBuilder& builder,
        std::string_view crs_name,
        const GeographicCoordinateSystemInfo& info )
    {
        check_components_attribute_to_geographic_coordinate_reference_system(
            brep.corners(), crs_name );
        check_components_attribute_to_geographic_coordinate_reference_system(
            brep.lines(), crs_name );
        check_components_attribute_to_geographic_coordinate_reference_system(
            brep.surfaces(), crs_name );
        check_components_attribute_to_geographic_coordinate_reference_system(
            brep.blocks(), crs_name );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >( info, crs_name, brep.corners(),
            [&builder]( const Corner3D& corner ) {
                return builder.corner_mesh_builder( corner );
            } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >(
            info, crs_name, brep.lines(), [&builder]( const Line3D& line ) {
                return builder.line_mesh_builder( line );
            } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >( info, crs_name, brep.surfaces(),
            [&builder]( const Surface3D& surface ) {
                return builder.surface_mesh_builder( surface );
            } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            3 >(
            info, crs_name, brep.blocks(), [&builder]( const Block3D& block ) {
                return builder.block_mesh_builder( block );
            } );
    }

    void assign_section_geographic_coordinate_system_info(
        const Section& section,
        SectionBuilder& builder,
        std::string_view crs_name,
        const GeographicCoordinateSystemInfo& info )
    {
        check_components_attribute_to_geographic_coordinate_reference_system(
            section.corners(), crs_name );
        check_components_attribute_to_geographic_coordinate_reference_system(
            section.lines(), crs_name );
        check_components_attribute_to_geographic_coordinate_reference_system(
            section.surfaces(), crs_name );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            2 >( info, crs_name, section.corners(),
            [&builder]( const Corner2D& corner ) {
                return builder.corner_mesh_builder( corner );
            } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            2 >(
            info, crs_name, section.lines(), [&builder]( const Line2D& line ) {
                return builder.line_mesh_builder( line );
            } );
        convert_components_attribute_to_geographic_coordinate_reference_system<
            2 >( info, crs_name, section.surfaces(),
            [&builder]( const Surface2D& surface ) {
                return builder.surface_mesh_builder( surface );
            } );
    }

    template < index_t dimension >
    void convert_edged_curve_coordinate_reference_system(
        const EdgedCurve< dimension >& mesh,
        EdgedCurveBuilder< dimension >& builder,
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void convert_point_set_coordinate_reference_system(
        const PointSet< dimension >& mesh,
        PointSetBuilder< dimension >& builder,
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void convert_solid_mesh_coordinate_reference_system(
        const SolidMesh< dimension >& mesh,
        SolidMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    template < index_t dimension >
    void convert_surface_mesh_coordinate_reference_system(
        const SurfaceMesh< dimension >& mesh,
        SurfaceMeshBuilder< dimension >& builder,
        std::string_view crs_name,
        GeographicCoordinateSystemInfo info )
    {
        convert_coordinate_reference_system(
            mesh, builder, crs_name, std::move( info ) );
    }

    void convert_brep_coordinate_reference_system( const BRep& brep,
        BRepBuilder& builder,
        std::string_view crs_name,
        const GeographicCoordinateSystemInfo& info )
    {
        TransformationCache transformations{ info };
        auto corners = components_converted_coordinates< 3 >(
            brep.corners(), crs_name, transformations );
        auto lines = components_converted_coordinates< 3 >(
            brep.lines(), crs_name, transformations );
        auto surfaces = components_converted_coordinates< 3 >(
            brep.surfaces(), crs_name, transformations );
        auto blocks = components_converted_coordinates< 3 >(
            brep.blocks(), crs_name, transformations );
        apply_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.corners(),
            [&builder]( const Corner3D& corner ) {
                return builder.corner_mesh_builder( corner );
            },
            std::move( corners ) );
        apply_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.lines(),
            [&builder]( const Line3D& line ) {
                return builder.line_mesh_builder( line );
            },
            std::move( lines ) );
        apply_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.surfaces(),
            [&builder]( const Surface3D& surface ) {
                return builder.surface_mesh_builder( surface );
            },
            std::move( surfaces ) );
        apply_components_coordinate_reference_system< 3 >(
            info, crs_name, brep.blocks(),
            [&builder]( const Block3D& block ) {
                return builder.block_mesh_builder( block );
            },
            std::move( blocks ) );
    }

    void convert_section_coordinate_reference_system( const Section& section,
        SectionBuilder& builder,
        std::string_view crs_name,
        const GeographicCoordinateSystemInfo& info )
    {
        TransformationCache transformations{ info };
        auto corners = components_converted_coordinates< 2 >(
            section.corners(), crs_name, transformations );
        auto lines = components_converted_coordinates< 2 >(
            section.lines(), crs_name, transformations );
        auto surfaces = components_converted_coordinates< 2 >(
            section.surfaces(), crs_name, transformations );
        apply_components_coordinate_reference_system< 2 >(
            info, crs_name, section.corners(),
            [&builder]( const Corner2D& corner ) {
                return builder.corner_mesh_builder( corner );
            },
            std::move( corners ) );
        apply_components_coordinate_reference_system< 2 >(
            info, crs_name, section.lines(),
            [&builder]( const Line2D& line ) {
                return builder.line_mesh_builder( line );
            },
            std::move( lines ) );
        apply_components_coordinate_reference_system< 2 >(
            info, crs_name, section.surfaces(),
            [&builder]( const Surface2D& surface ) {
                return builder.surface_mesh_builder( surface );
            },
            std::move( surfaces ) );
    }

    template void opengeode_geosciences_explicit_api
        assign_edged_curve_geographic_coordinate_system_info(
            const EdgedCurve< 2 >&,
            EdgedCurveBuilder< 2 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        assign_point_set_geographic_coordinate_system_info(
            const PointSet< 2 >&,
            PointSetBuilder< 2 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        assign_surface_mesh_geographic_coordinate_system_info(
            const SurfaceMesh< 2 >&,
            SurfaceMeshBuilder< 2 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        convert_edged_curve_coordinate_reference_system( const EdgedCurve< 2 >&,
            EdgedCurveBuilder< 2 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        convert_point_set_coordinate_reference_system( const PointSet< 2 >&,
            PointSetBuilder< 2 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        convert_surface_mesh_coordinate_reference_system(
            const SurfaceMesh< 2 >&,
            SurfaceMeshBuilder< 2 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );

    template void opengeode_geosciences_explicit_api
        assign_edged_curve_geographic_coordinate_system_info(
            const EdgedCurve< 3 >&,
            EdgedCurveBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        assign_point_set_geographic_coordinate_system_info(
            const PointSet< 3 >&,
            PointSetBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        assign_solid_mesh_geographic_coordinate_system_info(
            const SolidMesh< 3 >&,
            SolidMeshBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        assign_surface_mesh_geographic_coordinate_system_info(
            const SurfaceMesh< 3 >&,
            SurfaceMeshBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        convert_edged_curve_coordinate_reference_system( const EdgedCurve< 3 >&,
            EdgedCurveBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        convert_point_set_coordinate_reference_system( const PointSet< 3 >&,
            PointSetBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        convert_solid_mesh_coordinate_reference_system( const SolidMesh< 3 >&,
            SolidMeshBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
    template void opengeode_geosciences_explicit_api
        convert_surface_mesh_coordinate_reference_system(
            const SurfaceMesh< 3 >&,
            SurfaceMeshBuilder< 3 >&,
            std::string_view,
            GeographicCoordinateSystemInfo );
} // namespace geode