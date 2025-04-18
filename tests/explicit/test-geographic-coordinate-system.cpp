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

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

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
}

void test_crs()
{
    const auto infos =
        geode::GeographicCoordinateSystem3D::geographic_coordinate_systems();
    OPENGEODE_EXCEPTION(
        infos.size() == 13181, "[Test] Wrong number of supported CRS" );

    constexpr geode::index_t NB_POINTS{ 4 };
    geode::AttributeManager manager;
    manager.resize( NB_POINTS );

    geode::GeographicCoordinateSystem3D lambert1{ manager,
        { "EPSG", "27571", "I" } };
    for( const auto p : geode::TRange< double >{ NB_POINTS } )
    {
        lambert1.set_point( p, geode::Point3D{ { p, p, p } } );
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
        OPENGEODE_EXCEPTION( lambert2.point( p ).inexact_equal( answers[p] ),
            "[Test] Wrong coordinate conversion" );
    }
}

int main()
{
    try
    {
        geode::GeosciencesExplicitLibrary::initialize();
        test_bitsery();
        test_crs();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
