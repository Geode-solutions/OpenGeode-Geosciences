/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/tests_config.h>

#include <geode/basic/assert.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/triangulated_surface_output.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/explicit/representation/io/structural_model_input.h>
#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>

void test_model( const geode::StratigraphicModel& model )
{
    const geode::uuid block1_id{ "00000000-c271-42e7-8000-00002c3147ed" };
    const auto& block = model.block( block1_id );
    const auto& strati_pt1 = model.stratigraphic_coordinates( block, 59 );
    OPENGEODE_EXCEPTION( strati_pt1.stratigraphic_coordinates().inexact_equal(
                             { { -0.213112, -0.188148, 0.472047 } } ),
        "[Test] Wrong stratigraphic coordinates for point 59 at position [",
        model.block( block1_id ).mesh().point( 59 ).string(),
        "] with stratigraphic coordinates [", strati_pt1.string(), "]." );
    const geode::Point3D query2{ { 1, 0, 1 } };
    const auto strati_pt2 = model.stratigraphic_coordinates( block, query2 );
    OPENGEODE_EXCEPTION( strati_pt2->stratigraphic_coordinates().inexact_equal(
                             { { 0.386272, -0.109477, 0. } } ),
        "[Test] Wrong stratigraphic coordinates for point at position [",
        query2.string(), "] with stratigraphic coordinates [",
        strati_pt2->string(), "]." );
    const geode::Point3D query3{ { 0.480373621, 0.5420120955, 0.6765933633 } };
    const auto strati_pt3 = model.stratigraphic_coordinates( block, query3 );
    OPENGEODE_EXCEPTION(
        strati_pt3->stratigraphic_coordinates().inexact_equal(
            { { 0.03380647978, -0.002759957825, 0.3080064376 } } ),
        "[Test] Wrong stratigraphic coordinates for point at position [",
        query3.string(), "] with stratigraphic coordinates [",
        strati_pt3->string(), "]." );

    const auto geom_pt1 = model.geometric_coordinates( block, strati_pt1 );
    OPENGEODE_EXCEPTION( geom_pt1->inexact_equal( block.mesh().point( 59 ) ),
        "[Test] Wrong geometric coordinates for stratigraphic point at "
        "position [",
        strati_pt1.string(), "] with geometric coordinates [",
        geom_pt1->string(), "]." );
    const auto geom_pt2 =
        model.geometric_coordinates( block, strati_pt2.value() );
    OPENGEODE_EXCEPTION( geom_pt2->inexact_equal( query2 ),
        "[Test] Wrong geometric coordinates for stratigraphic point at "
        "position [",
        strati_pt2->string(), "] with geometric coordinates [",
        geom_pt2->string(), "]." );
    const auto geom_pt3 =
        model.geometric_coordinates( block, strati_pt3.value() );
    OPENGEODE_EXCEPTION( geom_pt3->inexact_equal( query3 ),
        "[Test] Wrong geometric coordinates for stratigraphic point at "
        "position [",
        strati_pt3->string(), "] with geometric coordinates [",
        geom_pt3->string(), "]." );

    const auto stratigraphic_bbox = model.stratigraphic_bounding_box();
    OPENGEODE_EXCEPTION( stratigraphic_bbox.min().inexact_equal(
                             { { -0.8098155, -0.5378192, 0 } } ),
        "[Test] Wrong stratigraphic coordinates bounding box minimum." );
    OPENGEODE_EXCEPTION( stratigraphic_bbox.max().inexact_equal(
                             { { 0.5643514, 0.6411656, 1 } } ),
        "[Test] Wrong stratigraphic coordinates bounding box minimum." );
}

void test_save_stratigraphic_surfaces( const geode::StratigraphicModel& model )
{
    geode::index_t counter{ 0 };
    for( const auto& model_block : model.blocks() )
    {
        for( const auto& surface : model.boundaries( model_block ) )
        {
            geode::save_triangulated_surface(
                surface.mesh< geode::TriangulatedSurface3D >(),
                absl::StrCat( "test_geom_surface_", counter, ".og_tsf3d" ) );
            const auto strati_surface =
                model.stratigraphic_surface( model_block, surface );
            geode::save_triangulated_surface( *strati_surface[0],
                absl::StrCat( "test_strati_surface_", counter, ".og_tsf3d" ) );
            counter++;
        }
    }
    counter = 0;
    for( const auto& model_block : model.blocks() )
    {
        for( const auto& surface : model.internal_surfaces( model_block ) )
        {
            geode::save_triangulated_surface(
                surface.mesh< geode::TriangulatedSurface3D >(),
                absl::StrCat(
                    "test_geom_int_surface_", counter, ".og_tsf3d" ) );
            const auto strati_surface =
                model.stratigraphic_surface( model_block, surface );
            geode::save_triangulated_surface(
                *strati_surface[0], absl::StrCat( "test_strati_int_surface_",
                                        counter, "_front.og_tsf3d" ) );
            geode::save_triangulated_surface(
                *strati_surface[1], absl::StrCat( "test_strati_int_surface_",
                                        counter, "_back.og_tsf3d" ) );
            counter++;
        }
    }
}

int main()
{
    try
    {
        geode::Logger::info( "Starting test" );
        geode::GeosciencesImplicitLibrary::initialize();
        geode::StratigraphicModel model{ geode::load_structural_model(
            absl::StrCat( geode::data_path, "vri2.og_strm" ) ) };
        test_model( model );
        test_save_stratigraphic_surfaces( model );

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}