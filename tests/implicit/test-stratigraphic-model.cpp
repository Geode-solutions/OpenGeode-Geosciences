/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.h>
#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.h>
#include <geode/geosciences/implicit/representation/core/horizons_stack.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_input.h>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_output.h>

void add_horizons_stack_to_model(
    geode::StratigraphicModel& model, const geode::uuid& block1_id )
{
    geode::StratigraphicModelBuilder builder{ model };
    auto horizons_stack_builder = builder.horizons_stack_builder();
    geode::index_t counter{ 1 };
    for( const auto& horizon : model.horizons() )
    {
        horizons_stack_builder.add_horizon( horizon.id() );
        builder.set_horizon_implicit_value( horizon, counter++ );
    }
    for( const auto& block : model.blocks() )
    {
        const auto& unit_id = builder.add_stratigraphic_unit();
        builder.add_block_in_stratigraphic_unit(
            block, model.stratigraphic_unit( unit_id ) );
        horizons_stack_builder.add_stratigraphic_unit( unit_id );
        if( block.id() != block1_id )
        {
            continue;
        }
        for( const auto& horizon : model.horizons() )
        {
            if( model.horizon_implicit_value( horizon ).value() == 3 )
            {
                horizons_stack_builder.add_horizon_above(
                    horizon, model.stratigraphic_unit( unit_id ) );
            }
        }
    }
}

void test_model(
    const geode::StratigraphicModel& model, const geode::uuid& block1_id )
{
    const auto& block = model.block( block1_id );
    const auto& strati_pt1 = model.stratigraphic_coordinates( block, 59 );
    OPENGEODE_EXCEPTION(
        strati_pt1.stratigraphic_coordinates().inexact_equal(
            geode::Point3D{ { -0.213112, -0.188148, 0.472047 } } ),
        "[Test] Wrong stratigraphic coordinates for point 59 at position [",
        model.block( block1_id ).mesh().point( 59 ).string(),
        "] with stratigraphic coordinates [", strati_pt1.string(), "]." );
    const geode::Point3D query2{ { 1, 0, 1 } };
    const auto strati_pt2 = model.stratigraphic_coordinates( block, query2 );
    OPENGEODE_EXCEPTION( strati_pt2->stratigraphic_coordinates().inexact_equal(
                             geode::Point3D{ { 0.386272, -0.109477, 0. } } ),
        "[Test] Wrong stratigraphic coordinates for point at position [",
        query2.string(), "] with stratigraphic coordinates [",
        strati_pt2->string(), "]." );
    const geode::Point3D query3{ { 0.480373621, 0.5420120955, 0.6765933633 } };
    const auto strati_pt3 = model.stratigraphic_coordinates( block, query3 );
    OPENGEODE_EXCEPTION(
        strati_pt3->stratigraphic_coordinates().inexact_equal( geode::Point3D{
            { 0.03380647978, -0.002759957825, 0.3080064376 } } ),
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
                             geode::Point3D{ { -0.8098155, -0.5378192, 0 } } ),
        "[Test] Wrong stratigraphic coordinates bounding box minimum." );
    OPENGEODE_EXCEPTION( stratigraphic_bbox.max().inexact_equal(
                             geode::Point3D{ { 0.5643514, 0.6411656, 1 } } ),
        "[Test] Wrong stratigraphic coordinates bounding box minimum." );

    for( const auto& horizon : model.horizons() )
    {
        const auto isovalue = model.horizon_implicit_value( horizon );
        OPENGEODE_EXCEPTION( isovalue,
            "[Test] Horizon should have an associated implicit value." );

        if( isovalue.value() == 3 )
        {
            const auto& strati_unit_id =
                model.horizons_stack().under( horizon.id() );
            OPENGEODE_EXCEPTION( strati_unit_id,
                "[Test] Should have found a stratigraphic unit "
                "under the horizon with implicit value 3." );
            const auto& strati_unit =
                model.stratigraphic_unit( strati_unit_id.value() );
            for( const auto& item :
                model.stratigraphic_unit_items( strati_unit ) )
            {
                OPENGEODE_EXCEPTION( item.id() == block1_id,
                    "[Test] The only item in given stratigraphic unit should "
                    "be ",
                    block.component_id().string() );
            }
        }
    }
}

void test_copy(
    const geode::StratigraphicModel& model, const geode::uuid& block1_id )
{
    geode::StratigraphicModel copy;
    geode::StratigraphicModelBuilder builder{ copy };
    const auto mappings = builder.copy( model );
    const auto& new_block_id =
        mappings.at( geode::Block3D::component_type_static() )
            .in2out( block1_id );
    test_model( copy, new_block_id );
    const auto& horizon_mapping =
        mappings.at( geode::Horizon3D::component_type_static() );
    for( const auto& horizon : model.horizons() )
    {
        OPENGEODE_EXCEPTION( copy.horizons_stack().has_horizon(
                                 horizon_mapping.in2out( horizon.id() ) ),
            "[Test] Should have found horizon in HorizonsStack." );
    }
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

void test_io(
    const geode::StratigraphicModel& model, const geode::uuid& block1_id )
{
    geode::Logger::info( "Testing IO" );
    const auto filename = "test_implicit_model_io.og_istrm";
    geode::save_implicit_structural_model( model, filename );
    geode::StratigraphicModel model_reload{
        geode::load_implicit_structural_model( filename )
    };
    geode::StratigraphicModelBuilder builder{ model_reload };
    builder.reinitialize_stratigraphic_query_trees();
    test_model( model_reload, block1_id );
}

int main()
{
    try
    {
        geode::Logger::info( "Starting test" );
        geode::GeosciencesImplicitLibrary::initialize();
        geode::StratigraphicModel model{ geode::load_structural_model(
            absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) ) };
        const geode::uuid block1_id{ "00000000-c271-42e7-8000-00002c3147ed" };
        add_horizons_stack_to_model( model, block1_id );
        test_model( model, block1_id );
        geode::Logger::info( "Testing copy" );
        test_copy( model, block1_id );
        test_save_stratigraphic_surfaces( model );
        test_io( model, block1_id );

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}