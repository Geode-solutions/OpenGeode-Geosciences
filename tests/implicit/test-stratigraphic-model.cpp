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

#include <geode/tests_config.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/sign.hpp>

#include <geode/mesh/core/geode/geode_polyhedral_solid.hpp>
#include <geode/mesh/core/geode/geode_tetrahedral_solid.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/implicit/geometry/stratigraphic_point.hpp>
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.hpp>
#include <geode/geosciences/implicit/representation/core/detail/helpers.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_input.hpp>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_output.hpp>

void add_horizons_stack_to_model(
    geode::StratigraphicModel& model, const geode::uuid& block1_id )
{
    geode::StratigraphicModelBuilder builder{ model };
    auto horizons_stack_builder = builder.horizons_stack_builder();
    geode::index_t counter{ 1 };
    for( const auto& horizon : model.horizons() )
    {
        horizons_stack_builder.add_horizon( horizon.id() );
        horizons_stack_builder.set_horizon_name(
            horizon, absl::StrCat( "horizon_", counter ) );
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
                horizons_stack_builder.set_horizon_above(
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
    geode::OpenGeodeGeosciencesImplicitException::test(
        strati_pt1.stratigraphic_coordinates().inexact_equal(
            geode::Point3D{ { -0.213112, -0.188148, 0.472047 } } ),
        "Wrong stratigraphic coordinates for point 59 at position [",
        model.block( block1_id ).mesh().point( 59 ).string(),
        "] with stratigraphic coordinates [", strati_pt1.string(), "]." );
    const geode::Point3D query2{ { 1, 0, 1 } };
    const auto strati_pt2 = model.stratigraphic_coordinates( block, query2 );
    geode::OpenGeodeGeosciencesImplicitException::test(
        strati_pt2->stratigraphic_coordinates().inexact_equal(
            geode::Point3D{ { 0.386272, -0.109477, 0. } } ),
        "Wrong stratigraphic coordinates for point at position [",
        query2.string(), "] with stratigraphic coordinates [",
        strati_pt2->string(), "]." );
    const geode::Point3D query3{ { 0.480373621, 0.5420120955, 0.6765933633 } };
    const auto strati_pt3 = model.stratigraphic_coordinates( block, query3 );
    geode::OpenGeodeGeosciencesImplicitException::test(
        strati_pt3->stratigraphic_coordinates().inexact_equal( geode::Point3D{
            { 0.03380647978, -0.002759957825, 0.3080064376 } } ),
        "Wrong stratigraphic coordinates for point at position [",
        query3.string(), "] with stratigraphic coordinates [",
        strati_pt3->string(), "]." );

    const auto geom_pt1 = model.geometric_coordinates( block, strati_pt1 );
    geode::OpenGeodeGeosciencesImplicitException::test(
        geom_pt1->inexact_equal( block.mesh().point( 59 ) ),
        "Wrong geometric coordinates for stratigraphic point at "
        "position [",
        strati_pt1.string(), "] with geometric coordinates [",
        geom_pt1->string(), "]." );
    const auto geom_pt2 =
        model.geometric_coordinates( block, strati_pt2.value() );
    geode::OpenGeodeGeosciencesImplicitException::test(
        geom_pt2->inexact_equal( query2 ),
        "Wrong geometric coordinates for stratigraphic point at "
        "position [",
        strati_pt2->string(), "] with geometric coordinates [",
        geom_pt2->string(), "]." );
    const auto geom_pt3 =
        model.geometric_coordinates( block, strati_pt3.value() );
    geode::OpenGeodeGeosciencesImplicitException::test(
        geom_pt3->inexact_equal( query3 ),
        "Wrong geometric coordinates for stratigraphic point at "
        "position [",
        strati_pt3->string(), "] with geometric coordinates [",
        geom_pt3->string(), "]." );

    const auto stratigraphic_bbox = model.stratigraphic_bounding_box();
    geode::OpenGeodeGeosciencesImplicitException::test(
        stratigraphic_bbox.min().inexact_equal(
            geode::Point3D{ { -0.8098155, -0.5378192, 0 } } ),
        "Wrong stratigraphic coordinates bounding box minimum." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stratigraphic_bbox.max().inexact_equal(
            geode::Point3D{ { 0.5643514, 0.6411656, 1 } } ),
        "Wrong stratigraphic coordinates bounding box minimum." );

    bool found_horizon{ false };
    for( const auto& horizon : model.horizons_stack().horizons() )
    {
        const auto isovalue = model.horizon_implicit_value( horizon );
        geode::OpenGeodeGeosciencesImplicitException::test(
            isovalue.has_value(),
            "Horizon should have an associated implicit value." );
        if( horizon.name() == "horizon_3" )
        {
            found_horizon = true;
            geode::OpenGeodeGeosciencesImplicitException::test(
                isovalue.value() == 3, "Horizon '", horizon.name().value(),
                "' should have an implicit value of 3, not ",
                isovalue.value() );
            const auto& strati_unit_id =
                model.horizons_stack().under( horizon.id() );
            geode::OpenGeodeGeosciencesImplicitException::test(
                strati_unit_id.has_value(),
                "Should have found a stratigraphic unit "
                "under the horizon with implicit value 3." );
            const auto& strati_unit =
                model.stratigraphic_unit( strati_unit_id.value() );
            for( const auto& item :
                model.stratigraphic_unit_items( strati_unit ) )
            {
                geode::OpenGeodeGeosciencesImplicitException::test(
                    item.id() == block1_id,
                    "The only item in given stratigraphic unit should "
                    "be ",
                    block.component_id().string() );
            }
        }
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        found_horizon, "Should have found horizon 'horizon_3'." );
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
    for( const auto& block : copy.blocks() )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            !block.mesh().vertex_attribute_manager().attribute_exists(
                model.stratigraphic_location_attribute_id() ),
            "Copied stratigraphic attribute should have been removed." );
    }
    const auto& horizon_mapping =
        mappings.at( geode::Horizon3D::component_type_static() );
    for( const auto& horizon : model.horizons() )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            copy.horizons_stack().has_horizon(
                horizon_mapping.in2out( horizon.id() ) ),
            "Should have found horizon in HorizonsStack." );
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

bool is_block_vertex_stratigraphic_point(
    const geode::StratigraphicModel& model,
    const geode::Block3D& block,
    const geode::ComponentMeshVertex& surface_vertex,
    const geode::Point3D& stratigraphic_point )
{
    const auto unique_vertex = model.unique_vertex( surface_vertex );
    for( const auto& block_vertex :
        model.component_mesh_vertices( unique_vertex ) )
    {
        if( block_vertex.component_id.id != block.id() )
        {
            continue;
        }
        if( model.stratigraphic_coordinates( block, block_vertex.vertex )
                .stratigraphic_coordinates()
                .inexact_equal( stratigraphic_point ) )
        {
            return true;
        }
    }
    return false;
}

void test_stratigraphic_surfaces_values(
    const geode::StratigraphicModel& model )
{
    geode::index_t nb_checks{ 0 };
    for( const auto& block : model.blocks() )
    {
        for( const auto& surface : model.boundaries( block ) )
        {
            const auto strati_surfaces =
                model.stratigraphic_surface( block, surface );
            for( const auto v : geode::Range{ surface.mesh().nb_vertices() } )
            {
                for( const auto& strati_surface : strati_surfaces )
                {
                    geode::OpenGeodeGeosciencesImplicitException::test(
                        is_block_vertex_stratigraphic_point( model, block,
                            { surface.component_id(), v },
                            strati_surface->point( v ) ),
                        "Wrong stratigraphic surface point ", v, " of surface ",
                        surface.id().string() );
                    nb_checks++;
                }
            }
        }
        for( const auto& surface : model.internal_surfaces( block ) )
        {
            const auto strati_surfaces =
                model.stratigraphic_surface( block, surface );
            for( const auto v : geode::Range{ surface.mesh().nb_vertices() } )
            {
                for( const auto& strati_surface : strati_surfaces )
                {
                    geode::OpenGeodeGeosciencesImplicitException::test(
                        is_block_vertex_stratigraphic_point( model, block,
                            { surface.component_id(), v },
                            strati_surface->point( v ) ),
                        "Wrong internal stratigraphic surface point ", v,
                        " of surface ", surface.id().string() );
                    nb_checks++;
                }
            }
        }
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        nb_checks > 0, "No stratigraphic surface point checked." );
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
    builder.import_old_stratigraphic_attribute_values_from_attribute_name(
        geode::StratigraphicModel::STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME );
    test_model( model_reload, block1_id );
}

void test_move( geode::StratigraphicModel& implicit_model )
{
    const auto old_implicit_id = implicit_model.implicit_attribute_id();
    const auto old_strati_id =
        implicit_model.stratigraphic_location_attribute_id();
    geode::StratigraphicModel moved_model{ std::move( implicit_model ) };
    geode::OpenGeodeGeosciencesImplicitException::test(
        moved_model.implicit_attribute_id() == old_implicit_id,
        "Implicit attribute id not moved." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        moved_model.stratigraphic_location_attribute_id() == old_strati_id,
        "Stratigraphic location attribute id not moved." );
}

void test_copy_stratigraphic_values_with_non_tetrahedral_block(
    const geode::StratigraphicModel& model,
    const geode::uuid& polyhedral_block_id )
{
    auto other_structural_model = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) );
    geode::StructuralModelBuilder other_structural_builder{
        other_structural_model
    };
    const auto other_polyhedral_block_id = other_structural_builder.add_block(
        geode::OpenGeodePolyhedralSolid3D::impl_name_static() );
    geode::StratigraphicModel other{ std::move( other_structural_model ) };
    geode::StratigraphicModelBuilder other_builder{ other };
    geode::ModelCopyMapping mappings;
    auto& block_mapping = mappings[geode::Block3D::component_type_static()];
    for( const auto& block : model.blocks() )
    {
        if( block.id() == polyhedral_block_id )
        {
            block_mapping.map( block.id(), other_polyhedral_block_id );
            continue;
        }
        block_mapping.map( block.id(), block.id() );
        for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
        {
            geode::Point2D location{ { 1. * v, 2. } };
            other_builder.set_stratigraphic_location(
                other.block( block.id() ), v, location );
        }
    }
    other_builder.copy_stratigraphic_attribute_values( mappings, model );
    for( const auto& block : model.blocks() )
    {
        if( block.id() == polyhedral_block_id )
        {
            continue;
        }
        for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
        {
            geode::OpenGeodeGeosciencesImplicitException::test(
                other.stratigraphic_coordinates( other.block( block.id() ), v )
                        .stratigraphic_location()
                    == model.stratigraphic_coordinates( block, v )
                        .stratigraphic_location(),
                "Wrong copied stratigraphic location." );
        }
    }
}

void test_non_tetrahedral_block()
{
    geode::Logger::info( "Testing non tetrahedral block" );
    auto structural_model = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) );
    geode::StructuralModelBuilder structural_builder{ structural_model };
    const auto polyhedral_block_id = structural_builder.add_block(
        geode::OpenGeodePolyhedralSolid3D::impl_name_static() );
    geode::StratigraphicModel model{ std::move( structural_model ) };
    geode::OpenGeodeGeosciencesImplicitException::test(
        !model.block( polyhedral_block_id )
            .mesh()
            .vertex_attribute_manager()
            .attribute_exists( model.stratigraphic_location_attribute_id() ),
        "Non tetrahedral block should not have stratigraphic location." );
    for( const auto& block : model.blocks() )
    {
        if( block.id() == polyhedral_block_id )
        {
            continue;
        }
        geode::OpenGeodeGeosciencesImplicitException::test(
            block.mesh().vertex_attribute_manager().attribute_exists(
                model.stratigraphic_location_attribute_id() ),
            "Tetrahedral block should have stratigraphic location." );
    }
    geode::StratigraphicModelBuilder import_builder{ model };
    import_builder
        .import_old_stratigraphic_attribute_values_from_attribute_name(
            geode::StratigraphicModel::STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME );
    import_builder
        .import_old_stratigraphic_attribute_values_from_attribute_name(
            "unknown_attribute" );
    for( const auto& block : model.blocks() )
    {
        if( block.id() == polyhedral_block_id )
        {
            continue;
        }
        const auto ids =
            block.mesh().vertex_attribute_manager().attribute_ids_matching_name(
                geode::StratigraphicModel::
                    STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME );
        geode::OpenGeodeGeosciencesImplicitException::test(
            ids && ids->size() == 1
                && ids->front() == model.stratigraphic_location_attribute_id(),
            "Old stratigraphic attribute should have been removed." );
    }
    test_copy_stratigraphic_values_with_non_tetrahedral_block(
        model, polyhedral_block_id );
    geode::StratigraphicModel moved_model{ std::move( model ) };
    geode::StratigraphicModelBuilder builder{ moved_model };
    builder.reinitialize_stratigraphic_query_trees();
    const auto box = moved_model.stratigraphic_bounding_box();
    geode::OpenGeodeGeosciencesImplicitException::test(
        box.min().value( 0 ) <= box.max().value( 0 ),
        "Stratigraphic bounding box should be valid." );
}

void test_empty_block()
{
    geode::Logger::info( "Testing empty block" );
    auto structural_model = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) );
    geode::StructuralModelBuilder structural_builder{ structural_model };
    const auto empty_block_id = structural_builder.add_block(
        geode::OpenGeodeTetrahedralSolid3D::impl_name_static() );
    const geode::StratigraphicModel model{ std::move( structural_model ) };
    const auto& empty_block = model.block( empty_block_id );
    const geode::StratigraphicPoint3D query{ geode::Point2D{ { 0, 0 } }, 0 };
    geode::OpenGeodeGeosciencesImplicitException::test(
        !model.stratigraphic_containing_polyhedron( empty_block, query ),
        "No polyhedron should contain a point in an empty block." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        !model.geometric_coordinates( empty_block, query ),
        "No geometric coordinates should be found in an empty block." );
    const auto box = model.stratigraphic_bounding_box();
    geode::OpenGeodeGeosciencesImplicitException::test(
        box.min().value( 0 ) <= box.max().value( 0 ),
        "Stratigraphic bounding box should ignore empty components." );
}

void test_reinitialize_after_modification( const geode::uuid& block1_id )
{
    geode::Logger::info( "Testing reinitialization after modification" );
    geode::StratigraphicModel model{ geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) ) };
    geode::StratigraphicModelBuilder builder{ model };
    builder.import_old_implicit_attribute_values_from_attribute_name(
        geode::ImplicitStructuralModel::IMPLICIT_ATTRIBUTE_NAME );
    builder.import_old_stratigraphic_attribute_values_from_attribute_name(
        geode::StratigraphicModel::STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME );
    std::vector< geode::uuid > to_remove;
    for( const auto& block : model.blocks() )
    {
        if( block.id() != block1_id )
        {
            to_remove.push_back( block.id() );
        }
    }
    for( const auto& block_id : to_remove )
    {
        builder.remove_block( model.block( block_id ) );
    }
    const auto& block1 = model.block( block1_id );
    builder.reinitialize_stratigraphic_query_trees();
    const auto strati_pt = model.stratigraphic_coordinates( block1, 59 );
    geode::OpenGeodeGeosciencesImplicitException::test(
        strati_pt.stratigraphic_coordinates().inexact_equal(
            geode::Point3D{ { -0.213112, -0.188148, 0.472047 } } ),
        "Wrong stratigraphic coordinates after reinitialization: ",
        strati_pt.string() );
    const auto box = model.stratigraphic_bounding_box();
    geode::OpenGeodeGeosciencesImplicitException::test(
        box.min().value( 2 ) <= box.max().value( 2 ),
        "Stratigraphic bounding box should be valid after block removal." );
}

void test_stratigraphic_model_from_coords()
{
    geode::Logger::info( "Testing stratigraphic model from coordinates" );
    geode::ImplicitStructuralModel implicit_model{ geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) ) };
    const auto model =
        geode::detail::stratigraphic_model_from_implicit_model_and_coords(
            std::move( implicit_model ), 2 );
    geode::index_t nb_checks{ 0 };
    for( const auto& block : model.blocks() )
    {
        for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
        {
            const auto& point = block.mesh().point( v );
            const auto& location = model.stratigraphic_coordinates( block, v )
                                       .stratigraphic_location();
            geode::OpenGeodeGeosciencesImplicitException::test(
                location.value( 0 ) == point.value( 0 )
                    && location.value( 1 ) == point.value( 1 ),
                "Wrong stratigraphic location from coordinates for vertex ", v,
                ": ", location.string(), " instead of ", point.string() );
            nb_checks++;
        }
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        nb_checks > 0, "No stratigraphic location checked." );
}

void test_invalid_stratigraphic_tetrahedra( geode::StratigraphicModel& model )
{
    geode::Logger::info( "Testing invalid stratigraphic tetrahedra" );
    geode::StratigraphicModelBuilder builder{ model };
    for( const auto& block : model.blocks() )
    {
        for( const auto v : geode::Range{ block.mesh().nb_vertices() } )
        {
            if( v % 7 != 0 )
            {
                continue;
            }
            auto location = model.stratigraphic_coordinates( block, v )
                                .stratigraphic_location();
            location.set_value( 0, -location.value( 0 ) );
            builder.set_stratigraphic_location( block, v, location );
        }
    }
    std::vector< geode::MeshElement > expected;
    for( const auto& block : model.blocks() )
    {
        const auto& mesh = block.mesh();
        for( const auto t : geode::Range{ mesh.nb_polyhedra() } )
        {
            const auto vertices = mesh.polyhedron_vertices( t );
            const geode::OwnerTetrahedron tetra{
                model.stratigraphic_coordinates( block, vertices[0] )
                    .stratigraphic_coordinates(),
                model.stratigraphic_coordinates( block, vertices[1] )
                    .stratigraphic_coordinates(),
                model.stratigraphic_coordinates( block, vertices[2] )
                    .stratigraphic_coordinates(),
                model.stratigraphic_coordinates( block, vertices[3] )
                    .stratigraphic_coordinates()
            };
            if( geode::tetrahedron_volume_sign( tetra )
                == geode::SIGN::negative )
            {
                expected.emplace_back( block.id(), t );
            }
        }
    }
    const auto invalid =
        geode::detail::invalid_stratigraphic_tetrahedra( model );
    geode::OpenGeodeGeosciencesImplicitException::test(
        !invalid.empty(), "Some stratigraphic tetrahedra should be invalid." );
    geode::OpenGeodeGeosciencesImplicitException::test( invalid == expected,
        "Wrong invalid stratigraphic tetrahedra: ", invalid.size(),
        " found instead of ", expected.size() );
    geode::StructuralModelBuilder structural_builder{ model };
    structural_builder.add_block(
        geode::OpenGeodePolyhedralSolid3D::impl_name_static() );
    geode::OpenGeodeGeosciencesImplicitException::test(
        geode::detail::invalid_stratigraphic_tetrahedra( model ) == expected,
        "Non tetrahedral blocks should be ignored." );
}

int main()
{
    try
    {
        geode::Logger::info( "Starting test" );
        geode::OpenGeodeGeosciencesImplicitLibrary::initialize();
        geode::Logger::set_level( geode::Logger::LEVEL::debug );
        geode::StratigraphicModel model{ geode::load_structural_model(
            absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) ) };
        geode::StratigraphicModelBuilder builder{ model };
        builder.import_old_implicit_attribute_values_from_attribute_name(
            geode::ImplicitStructuralModel::IMPLICIT_ATTRIBUTE_NAME );
        builder.import_old_stratigraphic_attribute_values_from_attribute_name(
            geode::StratigraphicModel::STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME );
        const geode::uuid block1_id{ "00000000-c271-42e7-8000-00002c3147ed" };
        add_horizons_stack_to_model( model, block1_id );
        test_model( model, block1_id );
        geode::Logger::info( "Testing copy" );
        test_copy( model, block1_id );
        DEBUG( "Testing save stratigraphic surfaces" );
        test_save_stratigraphic_surfaces( model );
        test_stratigraphic_surfaces_values( model );
        DEBUG( "Testing IO" );
        test_io( model, block1_id );
        test_invalid_stratigraphic_tetrahedra( model );
        test_move( model );
        test_non_tetrahedral_block();
        test_empty_block();
        test_reinitialize_after_modification( block1_id );
        test_stratigraphic_model_from_coords();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}