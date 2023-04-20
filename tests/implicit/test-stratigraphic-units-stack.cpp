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
#include <geode/basic/logger.h>

#include <geode/geosciences/implicit/representation/builder/stratigraphic_units_stack_builder.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_input.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_output.h>

void test_stratigraphic_stack()
{
    geode::StratigraphicUnitsStack3D units_stack;
    geode::StratigraphicUnitsStackBuilder3D stack_builder{ units_stack };

    const auto& hor0 = stack_builder.add_horizon();
    const auto& hor1 = stack_builder.add_horizon();
    const auto& hor2 = stack_builder.add_horizon();
    const auto& unit0 = stack_builder.add_stratigraphic_unit();
    const auto& unit1 = stack_builder.add_stratigraphic_unit();
    OPENGEODE_EXCEPTION( units_stack.nb_horizons() == 3,
        "[TEST] Stratigraphic Units Stack should have 3 horizons." );
    OPENGEODE_EXCEPTION( units_stack.nb_stratigraphic_units() == 2,
        "[TEST] Stratigraphic Units Stack should have 2 Stratigraphic Units." );
    stack_builder.add_horizon_under(
        units_stack.horizon( hor0 ), units_stack.stratigraphic_unit( unit0 ) );
    stack_builder.add_horizon_above(
        units_stack.horizon( hor1 ), units_stack.stratigraphic_unit( unit0 ) );
    stack_builder.add_horizon_under(
        units_stack.horizon( hor1 ), units_stack.stratigraphic_unit( unit1 ) );
    stack_builder.add_horizon_above(
        units_stack.horizon( hor2 ), units_stack.stratigraphic_unit( unit1 ) );

    for( const auto& unit : units_stack.stratigraphic_units() )
    {
        const auto& unit_id = unit.id();
        OPENGEODE_EXCEPTION( units_stack.above( unit_id ),
            "[TEST] Unit should have a horizon above." );
        OPENGEODE_EXCEPTION( units_stack.under( unit_id ),
            "[TEST] Unit should have a horizon under." );
    }
    OPENGEODE_EXCEPTION(
        units_stack.above( units_stack.above( unit0 ).value() ).value()
            == unit1,
        "[TEST] Horizon 1 should be found from horizon 0." );

    const auto stack_path = absl::StrCat( "test_SUStack.",
        geode::StratigraphicUnitsStack3D::native_extension_static() );
    geode::save_stratigraphic_units_stack( units_stack, stack_path );
    auto reloaded_stack =
        geode::load_stratigraphic_units_stack< 3 >( stack_path );
}

int main()
{
    try
    {
        geode::Logger::info( "Starting test" );
        geode::GeosciencesImplicitLibrary::initialize();
        test_stratigraphic_stack();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}