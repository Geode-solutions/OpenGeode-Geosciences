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

#include <geode/tests_config.h>

#include <geode/basic/assert.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>

#include <geode/geometry/bounding_box.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/edged_curve_output.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/builder/section_builder.h>
#include <geode/model/representation/core/section.h>

#include <geode/geosciences/explicit/representation/io/cross_section_input.h>
#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>
#include <geode/geosciences/implicit/representation/builder/stratigraphic_section_builder.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

geode::StratigraphicSection import_section_with_stratigraphy()
{
    geode::StratigraphicSection implicit_model{ geode::CrossSection(
        geode::load_section(
            absl::StrCat( geode::data_path, "test_section.og_sctn" ) ) ) };
    geode::StratigraphicSectionBuilder model_builder{ implicit_model };
    for( const auto& surface : implicit_model.surfaces() )
    {
        const auto& mesh = surface.mesh();
        const auto scalar_attribute =
            mesh.vertex_attribute_manager().find_attribute< double >(
                "curvature_min" );
        for( const auto vertex_id : geode::Range{ mesh.nb_vertices() } )
        {
            model_builder.set_stratigraphic_coordinates( surface, vertex_id,
                { { mesh.point( vertex_id ).value( 0 ),
                    scalar_attribute->value( vertex_id ) } } );
        }
    }
    return implicit_model;
}

void test_section( const geode::StratigraphicSection& implicit_model )
{
    const geode::uuid surface0_id{ "00000000-2d28-4eeb-8000-000027dab659" };
    const auto& surface0 = implicit_model.surface( surface0_id );
    const auto& strati_pt1 =
        implicit_model.stratigraphic_coordinates( surface0, 1773 );
    OPENGEODE_EXCEPTION(
        strati_pt1.stratigraphic_coordinates().inexact_equal(
            { { surface0.mesh().point( 1773 ).value( 0 ), -1.940964 } } ),
        "[Test] Wrong stratigraphic coordinates for point 1773 at position [",
        surface0.mesh().point( 1773 ).string(),
        "] with stratigraphic coordinates [", strati_pt1.string(), "]." );
    const auto query2 = surface0.mesh().point( 1832 );
    const auto strati_pt2 =
        implicit_model.stratigraphic_coordinates( surface0, query2 );
    OPENGEODE_EXCEPTION(
        strati_pt2->stratigraphic_coordinates().inexact_equal(
            implicit_model.stratigraphic_coordinates( surface0, 1832 )
                .stratigraphic_coordinates() ),
        "[Test] Wrong stratigraphic coordinates for point at position [",
        query2.string(), "] with stratigraphic coordinates [",
        strati_pt2->string(), "]." );
    const geode::Point2D query3{ { 3.32700324, 4.44508266 } };
    const auto strati_pt3 =
        implicit_model.stratigraphic_coordinates( surface0, query3 );
    OPENGEODE_EXCEPTION( strati_pt3->stratigraphic_coordinates().inexact_equal(
                             { { query3.value( 0 ), -1.889619 } } ),
        "[Test] Wrong stratigraphic coordinates for point at position [",
        query3.string(), "] with stratigraphic coordinates [",
        strati_pt3->string(), "]." );
    const auto stratigraphic_bbox = implicit_model.stratigraphic_bounding_box();
    OPENGEODE_EXCEPTION(
        stratigraphic_bbox.min().inexact_equal( { { 0, -6.5798343 } } ),
        "[Test] Wrong stratigraphic coordinates bounding box minimum." );
    OPENGEODE_EXCEPTION(
        stratigraphic_bbox.max().inexact_equal( { { 15, 10.4703907 } } ),
        "[Test] Wrong stratigraphic coordinates bounding box minimum." );
}

void test_save_stratigraphic_lines(
    const geode::StratigraphicSection& implicit_model )
{
    geode::index_t counter{ 0 };
    for( const auto& model_surface : implicit_model.surfaces() )
    {
        for( const auto& line : implicit_model.boundaries( model_surface ) )
        {
            geode::save_edged_curve(
                line.mesh(), absl::StrCat( "test_section_geom_line_", counter,
                                 ".og_edc2d" ) );
            const auto strati_line =
                implicit_model.stratigraphic_line( model_surface, line );
            geode::save_edged_curve(
                *strati_line[0], absl::StrCat( "test_section_strati_line_",
                                     counter, ".og_edc2d" ) );
            counter++;
        }
    }
    counter = 0;
    for( const auto& model_surface : implicit_model.surfaces() )
    {
        for( const auto& line : implicit_model.internal_lines( model_surface ) )
        {
            geode::save_edged_curve(
                line.mesh(), absl::StrCat( "test_section_geom_int_line_",
                                 counter, ".og_edc2d" ) );
            const auto strati_lines =
                implicit_model.stratigraphic_line( model_surface, line );
            geode::save_edged_curve( *strati_lines[0],
                absl::StrCat( "test_section_strati_int_surface_", counter,
                    "_front.og_edc2d" ) );
            geode::save_edged_curve( *strati_lines[1],
                absl::StrCat( "test_section_strati_int_surface_", counter,
                    "_back.og_edc2d" ) );
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
        const auto model = import_section_with_stratigraphy();
        test_section( model );
        test_save_stratigraphic_lines( model );

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}