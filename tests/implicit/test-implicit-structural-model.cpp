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

#include <geode/geometry/point.hpp>

#include <geode/basic/variable_attribute.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

#include <geode/model/mixin/core/block.hpp>

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.hpp>
#include <geode/geosciences/implicit/representation/core/detail/helpers.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>

namespace
{
    struct StackIds
    {
        std::array< geode::uuid, 3 > horizons;
        std::array< geode::uuid, 4 > units;
    };

    geode::uuid add_tetrahedral_block( geode::StructuralModelBuilder& builder )
    {
        const auto& block_id =
            builder.add_block( geode::MeshFactory::default_impl(
                geode::TetrahedralSolid3D::type_name_static() ) );
        return block_id;
    }

    void fill_tetrahedral_block( geode::StructuralModelBuilder& builder,
        const geode::Block3D& block,
        double shift )
    {
        auto mesh_builder =
            builder.block_mesh_builder< geode::TetrahedralSolid3D >( block );
        const auto offset =
            mesh_builder->create_point( geode::Point3D{ { shift, 0, 0 } } );
        mesh_builder->create_point( geode::Point3D{ { shift + 1, 0, 0 } } );
        mesh_builder->create_point( geode::Point3D{ { shift, 1, 0 } } );
        mesh_builder->create_point( geode::Point3D{ { shift, 0, 1 } } );
        mesh_builder->create_point( geode::Point3D{ { shift + 1, 1, 1 } } );
        mesh_builder->create_tetrahedron(
            { offset, offset + 1, offset + 2, offset + 3 } );
        mesh_builder->create_tetrahedron(
            { offset + 1, offset + 2, offset + 3, offset + 4 } );
        mesh_builder->compute_polyhedron_adjacencies();
    }

    /*!
     * Model with one tetrahedral block: implicit value is z.
     */
    geode::ImplicitStructuralModel create_model()
    {
        geode::StructuralModel structural_model;
        geode::StructuralModelBuilder builder{ structural_model };
        const auto block_id = add_tetrahedral_block( builder );
        fill_tetrahedral_block(
            builder, structural_model.block( block_id ), 0 );
        geode::ImplicitStructuralModel model{ std::move( structural_model ) };
        geode::ImplicitStructuralModelBuilder implicit_builder{ model };
        for( const auto& block : model.blocks() )
        {
            for( const auto vertex :
                geode::Range{ block.mesh().nb_vertices() } )
            {
                implicit_builder.set_implicit_value(
                    block, vertex, block.mesh().point( vertex ).value( 2 ) );
            }
        }
        return model;
    }

    /*!
     * Stack from bottom to top: u0, h0, u1, h1, u2, h2, u3
     */
    StackIds add_horizons_stack( geode::ImplicitStructuralModel& model,
        const std::array< double, 3 >& isovalues )
    {
        geode::ImplicitStructuralModelBuilder builder{ model };
        auto stack_builder = builder.horizons_stack_builder();
        const auto& stack = model.horizons_stack();
        StackIds ids;
        for( auto& horizon : ids.horizons )
        {
            horizon = stack_builder.add_horizon();
        }
        for( auto& unit : ids.units )
        {
            unit = stack_builder.add_stratigraphic_unit();
        }
        for( const auto h : geode::LRange{ 3 } )
        {
            const auto& horizon = stack.horizon( ids.horizons[h] );
            stack_builder.set_horizon_above(
                horizon, stack.stratigraphic_unit( ids.units[h] ) );
            stack_builder.set_horizon_under(
                horizon, stack.stratigraphic_unit( ids.units[h + 1] ) );
            builder.set_horizon_implicit_value( horizon, isovalues[h] );
        }
        stack_builder.compute_top_and_bottom_horizons();
        return ids;
    }

    geode::index_t nb_implicit_attributes(
        const geode::AttributeManager& manager )
    {
        const auto ids = manager.attribute_ids_matching_name(
            geode::ImplicitStructuralModel::IMPLICIT_ATTRIBUTE_NAME );
        return ids ? static_cast< geode::index_t >( ids->size() ) : 0;
    }

    void check_unit( const geode::ImplicitStructuralModel& model,
        double value,
        const geode::uuid& expected )
    {
        const auto unit = model.containing_stratigraphic_unit( value );
        geode::OpenGeodeGeosciencesImplicitException::test( unit.has_value(),
            "[containing_stratigraphic_unit] No unit found for value ", value );
        geode::OpenGeodeGeosciencesImplicitException::test(
            unit.value() == expected,
            "[containing_stratigraphic_unit] Wrong unit for value ", value );
    }
} // namespace

void test_containing_stratigraphic_unit()
{
    geode::Logger::info( "Testing containing stratigraphic unit" );
    {
        auto model = create_model();
        const auto ids = add_horizons_stack( model, { 0., 1., 2. } );
        check_unit( model, -1., ids.units[0] );
        check_unit( model, 0., ids.units[1] );
        check_unit( model, 0.5, ids.units[1] );
        check_unit( model, 1., ids.units[2] );
        check_unit( model, 1.5, ids.units[2] );
        check_unit( model, 2., ids.units[3] );
        check_unit( model, 3., ids.units[3] );
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.implicit_value_is_above_horizon(
                1.5, model.horizons_stack().horizon( ids.horizons[1] ) ),
            "[implicit_value_is_above_horizon] 1.5 should be above "
            "horizon 1" );
    }
    {
        auto model = create_model();
        const auto ids = add_horizons_stack( model, { 2., 1., 0. } );
        check_unit( model, 3., ids.units[0] );
        check_unit( model, 2., ids.units[0] );
        check_unit( model, 1.5, ids.units[1] );
        check_unit( model, 1., ids.units[1] );
        check_unit( model, 0.5, ids.units[2] );
        check_unit( model, 0., ids.units[2] );
        check_unit( model, -1., ids.units[3] );
        geode::OpenGeodeGeosciencesImplicitException::test(
            !model.implicit_value_is_above_horizon(
                1.5, model.horizons_stack().horizon( ids.horizons[1] ) ),
            "[implicit_value_is_above_horizon] 1.5 should be under "
            "horizon 1" );
    }
}

void test_containing_stratigraphic_unit_after_horizon_removal()
{
    geode::Logger::info(
        "Testing containing stratigraphic unit after horizon removal" );
    auto model = create_model();
    const auto ids = add_horizons_stack( model, { 0., 1., 2. } );
    geode::ImplicitStructuralModelBuilder builder{ model };
    builder.horizons_stack_builder().remove_horizon(
        model.horizons_stack().horizon( ids.horizons[2] ) );
    check_unit( model, -1., ids.units[0] );
    check_unit( model, 0.5, ids.units[1] );
    check_unit( model, 1.5, ids.units[2] );
    check_unit( model, 3., ids.units[2] );
}

void test_conversion_and_move()
{
    geode::Logger::info( "Testing conversion with non-tetrahedral block "
                         "and move" );
    geode::StructuralModel structural_model;
    geode::StructuralModelBuilder builder{ structural_model };
    const auto polyhedral_id = builder.add_block();
    const auto tetrahedral_id = add_tetrahedral_block( builder );
    fill_tetrahedral_block(
        builder, structural_model.block( tetrahedral_id ), 0 );
    geode::ImplicitStructuralModel model{ std::move( structural_model ) };
    geode::ImplicitStructuralModelBuilder implicit_builder{ model };
    const auto& block = model.block( tetrahedral_id );
    for( const auto vertex : geode::Range{ block.mesh().nb_vertices() } )
    {
        implicit_builder.set_implicit_value(
            block, vertex, block.mesh().point( vertex ).value( 2 ) );
    }
    geode::ImplicitStructuralModel moved{ std::move( model ) };
    const geode::Point3D query{ { 0.1, 0.1, 0.5 } };
    const auto value =
        moved.implicit_value( moved.block( tetrahedral_id ), query );
    geode::OpenGeodeGeosciencesImplicitException::test(
        value.has_value() && std::fabs( value.value() - 0.5 ) < 1e-10,
        "[conversion] Wrong implicit value after move" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        moved.block( polyhedral_id ).mesh().nb_vertices() == 0,
        "[conversion] Polyhedral block should be kept" );
}

void test_empty_mesh_query()
{
    geode::Logger::info( "Testing query on empty mesh" );
    geode::StructuralModel base_model;
    geode::StructuralModelBuilder builder{ base_model };
    const auto block_id = add_tetrahedral_block( builder );
    geode::ImplicitStructuralModel model{ std::move( base_model ) };
    const geode::Point3D query{ { 0.1, 0.1, 0.5 } };
    geode::OpenGeodeGeosciencesImplicitException::test(
        !model.containing_polyhedron( model.block( block_id ), query ),
        "[empty mesh] No element should contain the query" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        !model.implicit_value( model.block( block_id ), query ),
        "[empty mesh] No implicit value should be found" );
}

void test_reinitialize_after_mesh_edition()
{
    geode::Logger::info( "Testing query after mesh edition" );
    auto model = create_model();
    const auto& block = *model.blocks().begin();
    const geode::Point3D query{ { 0.1, 0.1, 0.5 } };
    geode::OpenGeodeGeosciencesImplicitException::test(
        model.containing_polyhedron( block, query ).has_value(),
        "[mesh edition] Element should be found before edition" );
    geode::ImplicitStructuralModelBuilder builder{ model };
    fill_tetrahedral_block( builder, block, 5 );
    builder.reinitialize_implicit_query_trees();
    const geode::Point3D new_query{ { 5.1, 0.1, 0.5 } };
    geode::OpenGeodeGeosciencesImplicitException::test(
        model.containing_polyhedron( block, new_query ).has_value(),
        "[mesh edition] Element should be found after edition" );
}

void test_from_scalar_field()
{
    geode::Logger::info( "Testing creation from scalar field" );
    geode::StructuralModel base_model;
    geode::StructuralModelBuilder builder{ base_model };
    const auto block_id = add_tetrahedral_block( builder );
    fill_tetrahedral_block( builder, base_model.block( block_id ), 0 );
    const auto& mesh = base_model.block( block_id ).mesh();
    geode::AttributeValues< double > values;
    values.default_value = 0;
    values.no_value = 0;
    const auto attribute_id =
        mesh.vertex_attribute_manager()
            .create_attribute< geode::VariableAttribute, double >(
                "scalar", values, geode::AttributeProperties{} );
    auto attribute =
        mesh.vertex_attribute_manager()
            .find_attribute< geode::VariableAttribute, double >( attribute_id );
    for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
    {
        attribute->set_value( vertex, mesh.point( vertex ).value( 2 ) + 1 );
    }
    const auto model =
        geode::detail::implicit_model_from_structural_model_scalar_field(
            std::move( base_model ), attribute_id );
    const auto& block = model.block( block_id );
    for( const auto vertex : geode::Range{ block.mesh().nb_vertices() } )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.implicit_value( block, vertex )
                == block.mesh().point( vertex ).value( 2 ) + 1,
            "[scalar field] Wrong implicit value" );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        nb_implicit_attributes( block.mesh().vertex_attribute_manager() ) == 1,
        "[scalar field] There should be only one implicit attribute" );
}

void test_rescale_constant_field()
{
    geode::Logger::info( "Testing rescale of a constant implicit field" );
    auto model = create_model();
    {
        geode::ImplicitStructuralModelBuilder builder{ model };
        for( const auto& block : model.blocks() )
        {
            for( const auto vertex :
                geode::Range{ block.mesh().nb_vertices() } )
            {
                builder.set_implicit_value( block, vertex, 1 );
            }
        }
    }
    geode::StratigraphicModel stratigraphic{ std::move( model ) };
    geode::detail::rescale_implicit_value_to_bbox_scale( stratigraphic );
    for( const auto& block : stratigraphic.blocks() )
    {
        for( const auto vertex : geode::Range{ block.mesh().nb_vertices() } )
        {
            geode::OpenGeodeGeosciencesImplicitException::test(
                stratigraphic.implicit_value( block, vertex ) == 1,
                "[rescale] Constant implicit field should not be rescaled" );
        }
    }
}

void test_rescale_with_unsupported_mesh()
{
    geode::Logger::info( "Testing rescale with unsupported mesh type" );
    geode::StructuralModel base_model;
    geode::StructuralModelBuilder builder{ base_model };
    const auto unsupported_id = builder.add_block();
    builder.block_mesh_builder( base_model.block( unsupported_id ) )
        ->create_point( geode::Point3D{ { 0, 0, 0 } } );
    const auto block_id = add_tetrahedral_block( builder );
    fill_tetrahedral_block( builder, base_model.block( block_id ), 0 );
    geode::ImplicitStructuralModel model{ std::move( base_model ) };
    geode::ImplicitStructuralModelBuilder implicit_builder{ model };
    const auto& block = model.block( block_id );
    for( const auto vertex : geode::Range{ block.mesh().nb_vertices() } )
    {
        implicit_builder.set_implicit_value( block, vertex, 1 );
    }
    geode::detail::rescale_implicit_value( model, 2 );
    for( const auto vertex : geode::Range{ block.mesh().nb_vertices() } )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.implicit_value( block, vertex ) == 2,
            "[rescale] Implicit values should be rescaled" );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        model.block( unsupported_id ).mesh().nb_vertices() == 1,
        "[rescale] Unsupported mesh should be kept" );
}

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesImplicitLibrary::initialize();
        test_containing_stratigraphic_unit();
        test_containing_stratigraphic_unit_after_horizon_removal();
        test_conversion_and_move();
        test_empty_mesh_query();
        test_reinitialize_after_mesh_edition();
        test_from_scalar_field();
        test_rescale_constant_field();
        test_rescale_with_unsupported_mesh();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
