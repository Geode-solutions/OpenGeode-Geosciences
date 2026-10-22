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
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.hpp>
#include <geode/geosciences/implicit/representation/core/detail/helpers.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_cross_section.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.hpp>

namespace
{
    struct StackIds
    {
        std::array< geode::uuid, 3 > horizons;
        std::array< geode::uuid, 4 > units;
    };

    geode::uuid add_triangulated_surface( geode::CrossSectionBuilder& builder )
    {
        const auto& surface_id =
            builder.add_surface( geode::MeshFactory::default_impl(
                geode::TriangulatedSurface2D::type_name_static() ) );
        return surface_id;
    }

    void fill_triangulated_surface( geode::CrossSectionBuilder& builder,
        const geode::Surface2D& surface,
        double shift )
    {
        auto mesh_builder =
            builder.surface_mesh_builder< geode::TriangulatedSurface2D >(
                surface );
        const auto offset =
            mesh_builder->create_point( geode::Point2D{ { shift, 0 } } );
        mesh_builder->create_point( geode::Point2D{ { shift + 1, 0 } } );
        mesh_builder->create_point( geode::Point2D{ { shift, 1 } } );
        mesh_builder->create_point( geode::Point2D{ { shift + 1, 1 } } );
        mesh_builder->create_triangle( { offset, offset + 1, offset + 2 } );
        mesh_builder->create_triangle( { offset + 1, offset + 3, offset + 2 } );
        mesh_builder->compute_polygon_adjacencies();
    }

    /*!
     * Model with one triangulated surface: implicit value is y.
     */
    geode::ImplicitCrossSection create_model()
    {
        geode::CrossSection cross_section;
        geode::CrossSectionBuilder builder{ cross_section };
        const auto surface_id = add_triangulated_surface( builder );
        fill_triangulated_surface(
            builder, cross_section.surface( surface_id ), 0 );
        geode::ImplicitCrossSection model{ std::move( cross_section ) };
        geode::ImplicitCrossSectionBuilder implicit_builder{ model };
        for( const auto& surface : model.surfaces() )
        {
            for( const auto vertex :
                geode::Range{ surface.mesh().nb_vertices() } )
            {
                implicit_builder.set_implicit_value( surface, vertex,
                    surface.mesh().point( vertex ).value( 1 ) );
            }
        }
        return model;
    }

    /*!
     * Stack from bottom to top: u0, h0, u1, h1, u2, h2, u3
     */
    StackIds add_horizons_stack( geode::ImplicitCrossSection& model,
        const std::array< double, 3 >& isovalues )
    {
        geode::ImplicitCrossSectionBuilder builder{ model };
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
            geode::ImplicitCrossSection::IMPLICIT_ATTRIBUTE_NAME );
        return ids ? static_cast< geode::index_t >( ids->size() ) : 0;
    }

    void check_unit( const geode::ImplicitCrossSection& model,
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
    geode::ImplicitCrossSectionBuilder builder{ model };
    builder.horizons_stack_builder().remove_horizon(
        model.horizons_stack().horizon( ids.horizons[2] ) );
    check_unit( model, -1., ids.units[0] );
    check_unit( model, 0.5, ids.units[1] );
    check_unit( model, 1.5, ids.units[2] );
    check_unit( model, 3., ids.units[2] );
}

void test_conversion_and_move()
{
    geode::Logger::info( "Testing conversion with non-triangulated surface "
                         "and move" );
    geode::CrossSection cross_section;
    geode::CrossSectionBuilder builder{ cross_section };
    const auto polygonal_id = builder.add_surface();
    const auto triangulated_id = add_triangulated_surface( builder );
    fill_triangulated_surface(
        builder, cross_section.surface( triangulated_id ), 0 );
    geode::ImplicitCrossSection model{ std::move( cross_section ) };
    geode::ImplicitCrossSectionBuilder implicit_builder{ model };
    const auto& surface = model.surface( triangulated_id );
    for( const auto vertex : geode::Range{ surface.mesh().nb_vertices() } )
    {
        implicit_builder.set_implicit_value(
            surface, vertex, surface.mesh().point( vertex ).value( 1 ) );
    }
    geode::ImplicitCrossSection moved{ std::move( model ) };
    const geode::Point2D query{ { 0.25, 0.5 } };
    const auto value =
        moved.implicit_value( moved.surface( triangulated_id ), query );
    geode::OpenGeodeGeosciencesImplicitException::test(
        value.has_value() && std::fabs( value.value() - 0.5 ) < 1e-10,
        "[conversion] Wrong implicit value after move" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        moved.surface( polygonal_id ).mesh().nb_vertices() == 0,
        "[conversion] Polygonal surface should be kept" );
}

void test_empty_mesh_query()
{
    geode::Logger::info( "Testing query on empty mesh" );
    geode::CrossSection base_model;
    geode::CrossSectionBuilder builder{ base_model };
    const auto surface_id = add_triangulated_surface( builder );
    geode::ImplicitCrossSection model{ std::move( base_model ) };
    const geode::Point2D query{ { 0.25, 0.5 } };
    geode::OpenGeodeGeosciencesImplicitException::test(
        !model.containing_polygon( model.surface( surface_id ), query ),
        "[empty mesh] No element should contain the query" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        !model.implicit_value( model.surface( surface_id ), query ),
        "[empty mesh] No implicit value should be found" );
}

void test_reinitialize_after_mesh_edition()
{
    geode::Logger::info( "Testing query after mesh edition" );
    auto model = create_model();
    const auto& surface = *model.surfaces().begin();
    const geode::Point2D query{ { 0.25, 0.5 } };
    geode::OpenGeodeGeosciencesImplicitException::test(
        model.containing_polygon( surface, query ).has_value(),
        "[mesh edition] Element should be found before edition" );
    geode::ImplicitCrossSectionBuilder builder{ model };
    fill_triangulated_surface( builder, surface, 5 );
    builder.reinitialize_implicit_query_trees();
    const geode::Point2D new_query{ { 5.25, 0.5 } };
    geode::OpenGeodeGeosciencesImplicitException::test(
        model.containing_polygon( surface, new_query ).has_value(),
        "[mesh edition] Element should be found after edition" );
}

void test_from_scalar_field()
{
    geode::Logger::info( "Testing creation from scalar field" );
    geode::CrossSection base_model;
    geode::CrossSectionBuilder builder{ base_model };
    const auto surface_id = add_triangulated_surface( builder );
    fill_triangulated_surface( builder, base_model.surface( surface_id ), 0 );
    const auto& mesh = base_model.surface( surface_id ).mesh();
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
        attribute->set_value( vertex, mesh.point( vertex ).value( 1 ) + 1 );
    }
    const auto model =
        geode::detail::implicit_section_from_cross_section_scalar_field(
            std::move( base_model ), attribute_id );
    const auto& surface = model.surface( surface_id );
    for( const auto vertex : geode::Range{ surface.mesh().nb_vertices() } )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.implicit_value( surface, vertex )
                == surface.mesh().point( vertex ).value( 1 ) + 1,
            "[scalar field] Wrong implicit value" );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        nb_implicit_attributes( surface.mesh().vertex_attribute_manager() )
            == 1,
        "[scalar field] There should be only one implicit attribute" );
}

void test_rescale_constant_field()
{
    geode::Logger::info( "Testing rescale of a constant implicit field" );
    auto model = create_model();
    {
        geode::ImplicitCrossSectionBuilder builder{ model };
        for( const auto& surface : model.surfaces() )
        {
            for( const auto vertex :
                geode::Range{ surface.mesh().nb_vertices() } )
            {
                builder.set_implicit_value( surface, vertex, 1 );
            }
        }
    }
    geode::StratigraphicSection stratigraphic{ std::move( model ) };
    geode::detail::rescale_implicit_value_to_bbox_scale( stratigraphic );
    for( const auto& surface : stratigraphic.surfaces() )
    {
        for( const auto vertex : geode::Range{ surface.mesh().nb_vertices() } )
        {
            geode::OpenGeodeGeosciencesImplicitException::test(
                stratigraphic.implicit_value( surface, vertex ) == 1,
                "[rescale] Constant implicit field should not be rescaled" );
        }
    }
}

void test_rescale_with_unsupported_mesh()
{
    geode::Logger::info( "Testing rescale with unsupported mesh type" );
    geode::CrossSection base_model;
    geode::CrossSectionBuilder builder{ base_model };
    const auto unsupported_id = builder.add_surface();
    builder.surface_mesh_builder( base_model.surface( unsupported_id ) )
        ->create_point( geode::Point2D{ { 0, 0 } } );
    const auto surface_id = add_triangulated_surface( builder );
    fill_triangulated_surface( builder, base_model.surface( surface_id ), 0 );
    geode::ImplicitCrossSection model{ std::move( base_model ) };
    geode::ImplicitCrossSectionBuilder implicit_builder{ model };
    const auto& surface = model.surface( surface_id );
    for( const auto vertex : geode::Range{ surface.mesh().nb_vertices() } )
    {
        implicit_builder.set_implicit_value( surface, vertex, 1 );
    }
    geode::detail::rescale_implicit_value( model, 2 );
    for( const auto vertex : geode::Range{ surface.mesh().nb_vertices() } )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.implicit_value( surface, vertex ) == 2,
            "[rescale] Implicit values should be rescaled" );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        model.surface( unsupported_id ).mesh().nb_vertices() == 1,
        "[rescale] Unsupported mesh should be kept" );
}

void test_copy_with_unsupported_mesh()
{
    geode::Logger::info( "Testing copy with unsupported mesh type" );
    geode::CrossSection base_model;
    geode::CrossSectionBuilder builder{ base_model };
    const auto unsupported_id = builder.add_surface();
    builder.surface_mesh_builder( base_model.surface( unsupported_id ) )
        ->create_point( geode::Point2D{ { 0, 0 } } );
    const auto surface_id = add_triangulated_surface( builder );
    fill_triangulated_surface( builder, base_model.surface( surface_id ), 0 );
    geode::ImplicitCrossSection model{ std::move( base_model ) };
    geode::ImplicitCrossSectionBuilder implicit_builder{ model };
    const auto& surface = model.surface( surface_id );
    for( const auto vertex : geode::Range{ surface.mesh().nb_vertices() } )
    {
        implicit_builder.set_implicit_value( surface, vertex, vertex + 1 );
    }
    geode::ImplicitCrossSection copy;
    geode::ImplicitCrossSectionBuilder copy_builder{ copy };
    const auto mappings = copy_builder.copy( model );
    const auto& copy_surface =
        copy.surface( mappings.at( geode::Surface2D::component_type_static() )
                .in2out( surface_id ) );
    for( const auto vertex : geode::Range{ copy_surface.mesh().nb_vertices() } )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            copy.implicit_value( copy_surface, vertex ) == vertex + 1,
            "[copy] Wrong implicit value in copy" );
    }
}

void test_copy_horizons_isovalues()
{
    geode::Logger::info( "Testing copy of horizons isovalues" );
    auto model = create_model();
    const auto ids = add_horizons_stack( model, { 0., 1., 2. } );
    {
        geode::ImplicitCrossSectionBuilder builder{ model };
        const auto model_only_horizon = builder.add_horizon();
        geode::OpenGeodeGeosciencesImplicitException::test(
            !model.horizons_stack().has_horizon( model_only_horizon ),
            "[copy isovalues] Horizon should not be in the stack" );
    }
    geode::ImplicitCrossSection copy;
    geode::ImplicitCrossSectionBuilder copy_builder{ copy };
    const auto mappings = copy_builder.copy( model );
    const auto& horizon_mapping =
        mappings.at( geode::Horizon2D::component_type_static() );
    for( const auto h : geode::LRange{ 3 } )
    {
        const auto& copy_horizon = copy.horizons_stack().horizon(
            horizon_mapping.in2out( ids.horizons[h] ) );
        const auto isovalue = copy.horizon_implicit_value( copy_horizon );
        geode::OpenGeodeGeosciencesImplicitException::test(
            isovalue.has_value() && isovalue.value() == h,
            "[copy isovalues] Wrong horizon isovalue in copy" );
    }
    check_unit( copy, 0.5,
        mappings.at( geode::StratigraphicUnit2D::component_type_static() )
            .in2out( ids.units[1] ) );
}

void test_copy_single_implicit_attribute()
{
    geode::Logger::info( "Testing copy keeps a single implicit attribute" );
    const auto model = create_model();
    geode::ImplicitCrossSection copy;
    geode::ImplicitCrossSectionBuilder copy_builder{ copy };
    const auto mappings = copy_builder.copy( model );
    for( const auto& surface : model.surfaces() )
    {
        const auto& copy_surface = copy.surface(
            mappings.at( geode::Surface2D::component_type_static() )
                .in2out( surface.id() ) );
        geode::OpenGeodeGeosciencesImplicitException::test(
            nb_implicit_attributes(
                copy_surface.mesh().vertex_attribute_manager() )
                == 1,
            "[copy] There should be only one implicit attribute" );
        for( const auto vertex : geode::Range{ surface.mesh().nb_vertices() } )
        {
            geode::OpenGeodeGeosciencesImplicitException::test(
                copy.implicit_value( copy_surface, vertex )
                    == surface.mesh().point( vertex ).value( 1 ),
                "[copy] Wrong implicit value in copy" );
        }
    }
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
        test_copy_with_unsupported_mesh();
        test_copy_horizons_isovalues();
        test_copy_single_implicit_attribute();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
