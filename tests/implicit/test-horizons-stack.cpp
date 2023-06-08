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

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.h>
#include <geode/geosciences/implicit/representation/core/horizons_stack.h>
#include <geode/geosciences/implicit/representation/io/horizons_stack_input.h>
#include <geode/geosciences/implicit/representation/io/horizons_stack_output.h>

void test_horizons_stack()
{
    geode::HorizonsStack3D horizons_stack;
    geode::HorizonsStackBuilder3D stack_builder{ horizons_stack };

    const auto& hor0 = stack_builder.add_horizon();
    const auto& hor1 = stack_builder.add_horizon();
    const geode::uuid hor2{};
    stack_builder.add_horizon( hor2 );
    const auto& unit0 = stack_builder.add_stratigraphic_unit();
    const geode::uuid unit1{};
    stack_builder.add_stratigraphic_unit( unit1 );
    OPENGEODE_EXCEPTION( horizons_stack.nb_horizons() == 3,
        "[Test] Stratigraphic Units Stack should have 3 horizons." );
    OPENGEODE_EXCEPTION( horizons_stack.nb_stratigraphic_units() == 2,
        "[Test] Stratigraphic Units Stack should have 2 Stratigraphic Units." );
    stack_builder.add_horizon_under( horizons_stack.horizon( hor0 ),
        horizons_stack.stratigraphic_unit( unit0 ) );
    stack_builder.add_horizon_above( horizons_stack.horizon( hor1 ),
        horizons_stack.stratigraphic_unit( unit0 ) );
    stack_builder.add_horizon_under( horizons_stack.horizon( hor1 ),
        horizons_stack.stratigraphic_unit( unit1 ) );
    stack_builder.add_horizon_above( horizons_stack.horizon( hor2 ),
        horizons_stack.stratigraphic_unit( unit1 ) );

    for( const auto& unit : horizons_stack.stratigraphic_units() )
    {
        const auto& unit_id = unit.id();
        OPENGEODE_EXCEPTION( horizons_stack.above( unit_id ),
            "[Test] Unit should have a horizon above." );
        OPENGEODE_EXCEPTION( horizons_stack.under( unit_id ),
            "[Test] Unit should have a horizon under." );
    }
    OPENGEODE_EXCEPTION(
        horizons_stack.above( horizons_stack.above( unit0 ).value() ).value()
            == unit1,
        "[Test] Unit 1 should be found from unit 0." );
    const auto insertion_info = stack_builder.add_horizon_in_stratigraphic_unit(
        horizons_stack.stratigraphic_unit( unit1 ) );
    OPENGEODE_EXCEPTION( horizons_stack.above( hor1 ).value()
                             == insertion_info.strati_unit_under_id,
        "[Test] New bottom unit should be found above horizon 1." );
    OPENGEODE_EXCEPTION(
        horizons_stack.above( horizons_stack.above( hor1 ).value() ).value()
            == insertion_info.new_horizon_id,
        "[Test] New horizon should be found from horizon 1." );

    const auto stack_path = absl::StrCat( "test_HorizonStack.",
        geode::HorizonsStack3D::native_extension_static() );
    geode::save_horizons_stack( horizons_stack, stack_path );
    auto reloaded_stack = geode::load_horizons_stack< 3 >( stack_path );
}

int main()
{
    try
    {
        geode::Logger::info( "Starting test" );
        geode::GeosciencesImplicitLibrary::initialize();
        test_horizons_stack();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}