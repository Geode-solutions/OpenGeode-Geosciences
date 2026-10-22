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

#include <cmath>
#include <fstream>
#include <iterator>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.hpp>
#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/coordinate_reference_system_manager.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.hpp>
#include <geode/geosciences/explicit/geometry/geographic_coordinate_system_helper.hpp>

#include <geode/tests_config.hpp>

void test_bitsery()
{
    auto surf = geode::load_triangulated_surface< 2 >(
        absl::StrCat( geode::DATA_PATH, "3patches.og_tsf2d" ) );
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surf );
    geode::assign_surface_mesh_geographic_coordinate_system_info(
        *surf, *builder, "test", { "EPSG", "2005", "test" } );
    geode::convert_surface_mesh_coordinate_reference_system(
        *surf, *builder, "test2", { "EPSG", "2008", "test2" } );
    geode::save_triangulated_surface( *surf, "crs.og_tsf2d" );

    geode::convert_surface_mesh_coordinate_reference_system(
        *surf, *builder, "back", { "EPSG", "2005", "back" } );
    const auto& original_crs = surf->main_coordinate_reference_system_manager()
                                   .find_coordinate_reference_system( "test" );
    for( const auto v : geode::Range{ surf->nb_vertices() } )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            surf->point( v ).inexact_equal( original_crs.point( v ) ),
            "Wrong round trip coordinate conversion" );
    }
    builder->main_coordinate_reference_system_manager_builder()
        .set_active_coordinate_reference_system( "test2" );

    const auto reloaded =
        geode::load_triangulated_surface< 2 >( "crs.og_tsf2d" );
    const auto& crs_manager =
        reloaded->main_coordinate_reference_system_manager();
    geode::OpenGeodeGeosciencesExplicitException::test(
        crs_manager.active_coordinate_reference_system_name() == "test2",
        "Wrong reloaded active CRS name" );
    const auto& crs =
        dynamic_cast< const geode::GeographicCoordinateSystem2D& >(
            crs_manager.active_coordinate_reference_system() );
    geode::OpenGeodeGeosciencesExplicitException::test(
        crs.info().authority == "EPSG" && crs.info().code == "2008"
            && crs.info().name == "test2",
        "Wrong reloaded CRS info" );
    for( const auto v : geode::Range{ surf->nb_vertices() } )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            reloaded->point( v ) == surf->point( v ),
            "Wrong reloaded CRS coordinates" );
    }
}

void test_corrupted_info_size()
{
    std::string content;
    {
        std::ifstream file{ "crs.og_tsf2d", std::ifstream::binary };
        content.assign( std::istreambuf_iterator< char >{ file },
            std::istreambuf_iterator< char >{} );
    }
    const std::string code{ "\x04"
                            "2008" };
    const auto position = content.find( code );
    geode::OpenGeodeGeosciencesExplicitException::test(
        position != std::string::npos, "[Test] CRS code not found" );
    // Largest size bitsery can encode: 0x3FFFFFFF
    content.replace( position, 1, "\xFF\xFF\xFF\xFF" );
    {
        std::ofstream file{ "corrupted_crs.og_tsf2d", std::ofstream::binary };
        file << content;
    }
    try
    {
        const auto mesh =
            geode::load_triangulated_surface< 2 >( "corrupted_crs.og_tsf2d" );
    }
    catch( const geode::OpenGeodeException& exception )
    {
        geode::Logger::info(
            "Corrupted CRS info throws as expected: ", exception.what() );
        return;
    }
    throw geode::OpenGeodeGeosciencesExplicitException{ nullptr,
        geode::OpenGeodeException::TYPE::internal,
        "[Test] Corrupted CRS info should not be loaded" };
}

template < typename Function >
void check_throws( const Function& function, std::string_view message )
{
    try
    {
        function();
    }
    catch( const geode::OpenGeodeException& exception )
    {
        geode::Logger::info(
            message, " throws as expected: ", exception.what() );
        return;
    }
    throw geode::OpenGeodeGeosciencesExplicitException{ nullptr,
        geode::OpenGeodeException::TYPE::internal, "[Test] ", message,
        " should throw" };
}

void test_failed_mesh_conversion_leaves_no_attribute()
{
    auto surf = geode::load_triangulated_surface< 2 >(
        absl::StrCat( geode::DATA_PATH, "3patches.og_tsf2d" ) );
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surf );
    const auto nb_attributes =
        surf->vertex_attribute_manager().attribute_ids().size();
    check_throws(
        [&] {
            geode::convert_surface_mesh_coordinate_reference_system(
                *surf, *builder, "geo", { "EPSG", "2008", "geo" } );
        },
        "Conversion from a non geographic CRS" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        surf->vertex_attribute_manager().attribute_ids().size()
            == nb_attributes,
        "Failed conversion from a non geographic CRS created an attribute" );

    geode::assign_surface_mesh_geographic_coordinate_system_info(
        *surf, *builder, "unknown", { "EPSG", "99999", "unknown" } );
    check_throws(
        [&] {
            geode::convert_surface_mesh_coordinate_reference_system(
                *surf, *builder, "geo", { "EPSG", "2008", "geo" } );
        },
        "Conversion from an unknown CRS" );
    const auto& crs_manager = surf->main_coordinate_reference_system_manager();
    geode::OpenGeodeGeosciencesExplicitException::test(
        surf->vertex_attribute_manager().attribute_ids().size() == nb_attributes
            && !crs_manager.coordinate_reference_system_exists( "geo" )
            && crs_manager.active_coordinate_reference_system_name()
                   == "unknown",
        "Failed conversion from an unknown CRS modified the mesh" );
}

void test_assign_validation()
{
    auto surf = geode::load_triangulated_surface< 2 >(
        absl::StrCat( geode::DATA_PATH, "3patches.og_tsf2d" ) );
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surf );
    geode::assign_surface_mesh_geographic_coordinate_system_info(
        *surf, *builder, "first", { "EPSG", "2005", "first" } );
    geode::convert_surface_mesh_coordinate_reference_system(
        *surf, *builder, "second", { "EPSG", "2008", "second" } );
    const auto& crs_manager = surf->main_coordinate_reference_system_manager();
    const auto active_crs = [&crs_manager]() -> decltype( auto ) {
        return dynamic_cast< const geode::GeographicCoordinateSystem2D& >(
            crs_manager.active_coordinate_reference_system() );
    };
    check_throws(
        [&] {
            geode::assign_surface_mesh_geographic_coordinate_system_info(
                *surf, *builder, "first", { "EPSG", "2005", "renamed" } );
        },
        "Assignment to an existing CRS name" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        crs_manager.active_coordinate_reference_system_name() == "second"
            && active_crs().attribute_name() == "second"
            && active_crs().info().code == "2008",
        "Failed assignment modified the mesh" );

    geode::assign_surface_mesh_geographic_coordinate_system_info(
        *surf, *builder, "second", { "EPSG", "2154", "renamed" } );
    geode::OpenGeodeGeosciencesExplicitException::test(
        crs_manager.active_coordinate_reference_system_name() == "second"
            && active_crs().attribute_name() == "renamed"
            && active_crs().info().code == "2154",
        "Wrong assignment to the active CRS name" );
}

struct BRepState
{
    std::vector< std::string > active_names;
    std::vector< geode::index_t > nb_attributes;
};

BRepState brep_state( const geode::BRep& brep )
{
    BRepState state;
    const auto add_state = [&state]( const auto& mesh ) {
        state.active_names.emplace_back(
            mesh.main_coordinate_reference_system_manager()
                .active_coordinate_reference_system_name() );
        state.nb_attributes.push_back(
            mesh.vertex_attribute_manager().attribute_ids().size() );
    };
    for( const auto& corner : brep.corners() )
    {
        add_state( corner.mesh() );
    }
    for( const auto& line : brep.lines() )
    {
        add_state( line.mesh() );
    }
    return state;
}

void check_brep_unchanged( const geode::BRep& brep, const BRepState& state )
{
    const auto new_state = brep_state( brep );
    geode::OpenGeodeGeosciencesExplicitException::test(
        new_state.active_names == state.active_names
            && new_state.nb_attributes == state.nb_attributes,
        "Failed BRep operation partially modified the model" );
}

// Corners are processed before lines: make only the line invalid
void test_brep_atomicity(
    const geode::GeographicCoordinateSystemInfo& line_info,
    bool register_target_on_line )
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    for( const auto c : geode::Range{ 3 } )
    {
        const auto& corner_id = builder.add_corner();
        const auto& corner = brep.corner( corner_id );
        auto mesh_builder = builder.corner_mesh_builder( corner );
        const auto value = static_cast< double >( c );
        mesh_builder->create_point( geode::Point3D{ { value, value, 0 } } );
        geode::assign_point_set_geographic_coordinate_system_info(
            corner.mesh(), *mesh_builder, "geo", { "EPSG", "27571", "geo" } );
    }
    const auto& line_id = builder.add_line();
    const auto& line = brep.line( line_id );
    auto line_builder = builder.line_mesh_builder( line );
    line_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    line_builder->create_point( geode::Point3D{ { 1, 1, 0 } } );
    line_builder->create_edge( 0, 1 );
    if( !line_info.authority.empty() )
    {
        geode::assign_edged_curve_geographic_coordinate_system_info(
            line.mesh(), *line_builder, "geo", line_info );
    }
    if( register_target_on_line )
    {
        line_builder->main_coordinate_reference_system_manager_builder()
            .register_coordinate_reference_system( "target",
                std::make_shared< geode::GeographicCoordinateSystem3D >(
                    line.mesh().vertex_attribute_manager(),
                    geode::GeographicCoordinateSystemInfo{
                        "EPSG", "27572", "target" } ) );
    }
    line_builder.reset();
    const auto state = brep_state( brep );
    check_throws(
        [&] {
            geode::convert_brep_coordinate_reference_system(
                brep, builder, "target", { "EPSG", "27572", "target" } );
        },
        "Invalid BRep conversion" );
    check_brep_unchanged( brep, state );
    if( register_target_on_line )
    {
        check_throws(
            [&] {
                geode::assign_brep_geographic_coordinate_system_info(
                    brep, builder, "target", { "EPSG", "27572", "target" } );
            },
            "Invalid BRep assignment" );
        check_brep_unchanged( brep, state );
    }
}

void test_brep_conversion_atomicity()
{
    test_brep_atomicity( { "", "", "" }, false );
    test_brep_atomicity( { "EPSG", "99999", "unknown" }, false );
    test_brep_atomicity( { "EPSG", "27571", "geo" }, true );
}

void test_brep_conversion()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    constexpr geode::index_t NB_CORNERS{ 4 };
    for( const auto c : geode::Range{ NB_CORNERS } )
    {
        const auto& corner = brep.corner( builder.add_corner() );
        auto mesh_builder = builder.corner_mesh_builder( corner );
        const auto value = static_cast< double >( c );
        mesh_builder->create_point( geode::Point3D{ { value, value, value } } );
    }
    const auto& line = brep.line( builder.add_line() );
    {
        auto line_builder = builder.line_mesh_builder( line );
        line_builder->create_point( geode::Point3D{ { 1000, 2000, 3 } } );
        line_builder->create_point( geode::Point3D{ { 4000, 5000, 6 } } );
        line_builder->create_edge( 0, 1 );
    }
    geode::assign_brep_geographic_coordinate_system_info(
        brep, builder, "lambert", { "EPSG", "27571", "I" } );
    {
        // The line is already in the target CRS
        auto line_builder = builder.line_mesh_builder( line );
        geode::assign_edged_curve_geographic_coordinate_system_info(
            line.mesh(), *line_builder, "lambert", { "EPSG", "27572", "II" } );
    }
    geode::convert_brep_coordinate_reference_system(
        brep, builder, "lambert2", { "EPSG", "27572", "II" } );
    const std::array< geode::Point3D, NB_CORNERS > answers{
        geode::Point3D{ { 4273.64251995017, 1302920.55457198, 0 } },
        geode::Point3D{ { 4274.63159306906, 1302921.55102308, 1 } },
        geode::Point3D{ { 4275.62066620018, 1302922.54747419, 2 } },
        geode::Point3D{ { 4276.60973934352, 1302923.5439253, 3 } },
    };
    for( const auto& corner : brep.corners() )
    {
        const auto& mesh = corner.mesh();
        geode::OpenGeodeGeosciencesExplicitException::test(
            mesh.main_coordinate_reference_system_manager()
                    .active_coordinate_reference_system_name()
                == "lambert2",
            "Wrong active CRS after BRep conversion" );
        const auto& point = mesh.point( 0 );
        const auto c =
            static_cast< geode::index_t >( std::round( point.value( 2 ) ) );
        geode::OpenGeodeGeosciencesExplicitException::test(
            c < NB_CORNERS && point.inexact_equal( answers[c] ),
            "Wrong corner coordinates after BRep conversion: ",
            point.string() );
    }
    geode::OpenGeodeGeosciencesExplicitException::test(
        line.mesh().point( 0 ).inexact_equal(
            geode::Point3D{ { 1000, 2000, 3 } } )
            && line.mesh().point( 1 ).inexact_equal(
                geode::Point3D{ { 4000, 5000, 6 } } ),
        "Wrong line coordinates after BRep conversion" );
}

void test_crs()
{
    const auto infos =
        geode::GeographicCoordinateSystem3D::geographic_coordinate_systems();
    geode::OpenGeodeGeosciencesExplicitException::test(
        infos.size() == 13181, "Wrong number of supported CRS" );

    constexpr geode::index_t NB_POINTS{ 4 };
    geode::AttributeManager manager;
    manager.resize( NB_POINTS );

    geode::GeographicCoordinateSystem3D lambert1{ manager,
        { "EPSG", "27571", "I" } };
    for( const auto p : geode::Range{ NB_POINTS } )
    {
        const auto value = static_cast< double >( p );
        lambert1.set_point( p, geode::Point3D{ { value, value, value } } );
    }
    geode::GeographicCoordinateSystem3D lambert2{ manager,
        { "EPSG", "27572", "II" } };
    lambert2.import_coordinates( lambert1 );
    std::array< geode::Point3D, NB_POINTS > answers{
        geode::Point3D{ { 4273.64251995017, 1302920.55457198, 0 } },
        geode::Point3D{ { 4274.63159306906, 1302921.55102308, 1 } },
        geode::Point3D{ { 4275.62066620018, 1302922.54747419, 2 } },
        geode::Point3D{ { 4276.60973934352, 1302923.5439253, 3 } },
    };
    for( const auto p : geode::Range{ NB_POINTS } )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            lambert2.point( p ).inexact_equal( answers[p] ),
            "Wrong coordinate conversion" );
    }
}

void test_axis_order()
{
    constexpr geode::index_t NB_POINTS{ 2 };
    geode::AttributeManager manager;
    manager.resize( NB_POINTS );
    // Longitude first, latitude second
    geode::GeographicCoordinateSystem2D wgs84{ manager,
        { "EPSG", "4326", "WGS 84" } };
    wgs84.set_point( 0, geode::Point2D{ { 2.35, 48.85 } } );
    wgs84.set_point( 1, geode::Point2D{ { -73.9857, 40.7484 } } );
    geode::GeographicCoordinateSystem2D mercator{ manager,
        { "EPSG", "3857", "Mercator" } };
    mercator.import_coordinates( wgs84 );
    const std::array< geode::Point2D, NB_POINTS > answers{
        geode::Point2D{ { 261600.8033641929, 6249447.752791281 } },
        geode::Point2D{ { -8236050.4499839, 4975301.253789808 } },
    };
    for( const auto p : geode::Range{ NB_POINTS } )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            mercator.point( p ).inexact_equal( answers[p] ),
            "Wrong WGS 84 to Mercator conversion: ",
            mercator.point( p ).string() );
    }
    geode::GeographicCoordinateSystem2D back{ manager,
        { "EPSG", "4326", "back" } };
    back.import_coordinates( mercator );
    for( const auto p : geode::Range{ NB_POINTS } )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            back.point( p ).inexact_equal( wgs84.point( p ) ),
            "Wrong Mercator to WGS 84 conversion: ", back.point( p ).string() );
    }
}

void test_invalid_code( const geode::GeographicCoordinateSystemInfo& from,
    const geode::GeographicCoordinateSystemInfo& to )
{
    geode::AttributeManager manager;
    manager.resize( 2 );
    geode::GeographicCoordinateSystem3D from_crs{ manager, from };
    geode::GeographicCoordinateSystem3D to_crs{ manager, to };
    try
    {
        to_crs.import_coordinates( from_crs );
    }
    catch( const geode::OpenGeodeException& exception )
    {
        geode::Logger::info(
            "Invalid CRS code throws as expected: ", exception.what() );
        return;
    }
    throw geode::OpenGeodeGeosciencesExplicitException{ nullptr,
        geode::OpenGeodeException::TYPE::internal,
        "[Test] Conversion with an invalid CRS code should throw" };
}

void test_invalid_codes()
{
    test_invalid_code( { "EPSG", "27571", "I" }, { "EPSG", "99999", "bad" } );
    test_invalid_code( { "EPSG", "99999", "bad" }, { "EPSG", "27571", "I" } );
    test_invalid_code( { "EPSG", "27571", "I" }, { "", "", "empty" } );
}

void test_failed_point_conversion()
{
    geode::AttributeManager manager;
    manager.resize( 3 );
    geode::GeographicCoordinateSystem3D wgs84{ manager,
        { "EPSG", "4326", "WGS 84" } };
    wgs84.set_point( 0, geode::Point3D{ { 1000, 1000, 0 } } );
    wgs84.set_point( 1, geode::Point3D{ { 1, 1, 0 } } );
    wgs84.set_point( 2, geode::Point3D{ { -1000, 1000, 0 } } );
    geode::GeographicCoordinateSystem3D mercator{ manager,
        { "EPSG", "3857", "Mercator" } };
    try
    {
        mercator.import_coordinates( wgs84 );
    }
    catch( const geode::OpenGeodeException& exception )
    {
        geode::Logger::info(
            "Invalid point conversion throws as expected: ", exception.what() );
        geode::OpenGeodeGeosciencesExplicitException::test(
            std::string_view{ exception.what() }.find( "2 points" )
                != std::string_view::npos,
            "Wrong number of failed points reported" );
        return;
    }
    throw geode::OpenGeodeGeosciencesExplicitException{ nullptr,
        geode::OpenGeodeException::TYPE::internal,
        "[Test] Conversion of an invalid point should throw" };
}

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesExplicitLibrary::initialize();
        test_bitsery();
        test_corrupted_info_size();
        test_crs();
        test_axis_order();
        test_invalid_codes();
        test_failed_point_conversion();
        test_failed_mesh_conversion_leaves_no_attribute();
        test_assign_validation();
        test_brep_conversion_atomicity();
        test_brep_conversion();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
