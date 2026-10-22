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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>
#include <geode/geosciences/implicit/representation/builder/helpers/implicit_structural_model_stratigraphic_blocks_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>

namespace
{
    using CONTACT = geode::Horizon3D::CONTACT_TYPE;

    /*!
     * Stack from top to bottom: horizons[0], units[0], horizons[1], units[1],
     * ..., units[n-2], horizons[n-1].
     */
    struct Stack
    {
        std::vector< geode::uuid > horizons;
        std::vector< geode::uuid > units;
    };

    Stack create_stack( geode::ImplicitStructuralModel& model,
        absl::Span< const CONTACT > contact_types )
    {
        geode::ImplicitStructuralModelBuilder builder{ model };
        auto stack_builder = builder.horizons_stack_builder();
        Stack stack;
        for( const auto contact_type : contact_types )
        {
            const auto& horizon_id = builder.add_horizon( contact_type );
            stack_builder.add_horizon( horizon_id );
            stack.horizons.push_back( horizon_id );
        }
        for( const auto h : geode::Range{ 1, contact_types.size() } )
        {
            const auto unit_id = stack_builder.add_stratigraphic_unit();
            stack.units.push_back( unit_id );
            const auto& unit =
                model.horizons_stack().stratigraphic_unit( unit_id );
            stack_builder.set_horizon_above(
                model.horizons_stack().horizon( stack.horizons[h - 1] ), unit );
            stack_builder.set_horizon_under(
                model.horizons_stack().horizon( stack.horizons[h] ), unit );
        }
        stack_builder.compute_top_and_bottom_horizons();
        return stack;
    }

    const geode::uuid& add_block( geode::ImplicitStructuralModel& model )
    {
        geode::ImplicitStructuralModelBuilder builder{ model };
        return builder.add_block();
    }

    /*!
     * Adds a surface bounding the given blocks, optionally in a horizon
     */
    void add_boundary( geode::ImplicitStructuralModel& model,
        absl::Span< const geode::uuid > blocks,
        const std::optional< geode::uuid >& horizon )
    {
        geode::ImplicitStructuralModelBuilder builder{ model };
        const auto& surface = model.surface( builder.add_surface() );
        for( const auto& block : blocks )
        {
            builder.add_surface_block_boundary_relationship(
                surface, model.block( block ) );
        }
        if( horizon )
        {
            builder.add_surface_in_horizon(
                surface, model.horizon( horizon.value() ) );
        }
    }

    void assign( geode::ImplicitStructuralModel& model,
        const geode::uuid& block,
        const geode::uuid& unit )
    {
        geode::ImplicitStructuralModelBuilder builder{ model };
        if( !model.has_stratigraphic_unit( unit ) )
        {
            builder.add_stratigraphic_unit( unit );
        }
        builder.add_block_in_stratigraphic_unit(
            model.block( block ), model.stratigraphic_unit( unit ) );
    }

    std::optional< geode::uuid > block_unit(
        const geode::ImplicitStructuralModel& model, const geode::uuid& block )
    {
        for( const auto& collection : model.collections( block ) )
        {
            if( collection.type
                == geode::StratigraphicUnit3D::component_type_static() )
            {
                return collection.id;
            }
        }
        return std::nullopt;
    }

    void check_unit( const geode::ImplicitStructuralModel& model,
        const geode::uuid& block,
        const std::optional< geode::uuid >& expected_unit,
        std::string_view message )
    {
        const auto unit = block_unit( model, block );
        geode::OpenGeodeGeosciencesImplicitException::test(
            unit == expected_unit, "Wrong stratigraphic unit: ", message );
    }

    void test_top_horizon_without_unit_above()
    {
        geode::Logger::info( "Testing top horizon without unit above" );
        geode::ImplicitStructuralModel model;
        const auto stack = create_stack( model,
            { CONTACT::conformal, CONTACT::conformal, CONTACT::conformal } );
        const auto block = add_block( model );
        add_boundary( model, { block }, stack.horizons[0] );
        add_boundary( model, { block }, stack.horizons[1] );
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.horizons_stack().top_horizon() == stack.horizons[0]
                && model.horizons_stack().is_above(
                    stack.horizons[0], stack.units[0] ),
            "Wrong test stack" );
        const auto result =
            geode::complete_stratigraphic_unit_block_relationships( model );
        check_unit( model, block, stack.units[0], "block under top horizon" );
        geode::OpenGeodeGeosciencesImplicitException::test(
            result.assigned_blocks.size() == 1,
            "One block should be assigned" );
    }

    void test_horizon_not_in_stack()
    {
        geode::Logger::info( "Testing horizon not in stack" );
        geode::ImplicitStructuralModel model;
        const auto stack = create_stack( model,
            { CONTACT::conformal, CONTACT::conformal, CONTACT::conformal } );
        geode::ImplicitStructuralModelBuilder builder{ model };
        const auto other_horizon = builder.add_horizon();
        const auto block = add_block( model );
        const auto neighbor = add_block( model );
        add_boundary( model, { block, neighbor }, other_horizon );
        assign( model, neighbor, stack.units[1] );
        const auto result =
            geode::complete_stratigraphic_unit_block_relationships( model );
        check_unit( model, block, std::nullopt, "horizon not in stack" );
        geode::OpenGeodeGeosciencesImplicitException::test(
            result.unassigned_blocks.size() == 1,
            "One block should be unassigned" );
    }

    void test_empty_stratigraphic_units_cleanup()
    {
        geode::Logger::info( "Testing empty stratigraphic units cleanup" );
        geode::ImplicitStructuralModel model;
        const auto stack =
            create_stack( model, { CONTACT::conformal, CONTACT::conformal,
                                     CONTACT::conformal, CONTACT::conformal } );
        geode::ImplicitStructuralModelBuilder builder{ model };
        const auto user_unit = builder.add_stratigraphic_unit();
        builder.add_stratigraphic_unit( stack.units[2] );
        const auto block = add_block( model );
        add_boundary( model, { block }, stack.horizons[0] );
        add_boundary( model, { block }, stack.horizons[1] );
        geode::complete_stratigraphic_unit_block_relationships( model );
        check_unit( model, block, stack.units[0], "block in first unit" );
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.has_stratigraphic_unit( user_unit ),
            "Empty user stratigraphic unit should be kept" );
        geode::OpenGeodeGeosciencesImplicitException::test(
            model.has_stratigraphic_unit( stack.units[2] ),
            "Empty stratigraphic unit added by the user should be kept" );
        geode::OpenGeodeGeosciencesImplicitException::test(
            !model.has_stratigraphic_unit( stack.units[1] ),
            "Empty created stratigraphic unit should be removed" );
    }
} // namespace

int main()
{
    try
    {
        geode::Logger::info( "Starting test" );
        geode::OpenGeodeGeosciencesImplicitLibrary::initialize();
        test_top_horizon_without_unit_above();
        test_horizon_not_in_stack();
        test_empty_stratigraphic_units_cleanup();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
