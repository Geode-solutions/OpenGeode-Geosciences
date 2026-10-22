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
#include <geode/basic/logger.hpp>

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/core/detail/helpers.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/io/horizons_stack_input.hpp>
#include <geode/geosciences/implicit/representation/io/horizons_stack_output.hpp>

void test_horizons_stack()
{
    geode::HorizonsStack3D horizons_stack;
    geode::HorizonsStackBuilder3D stack_builder{ horizons_stack };

    geode::index_t counter{ 0 };
    geode::Logger::info(
        "There should be a warning on empty iteration after this" );
    for( const auto& horizon : horizons_stack.bottom_to_top_horizons() )
    {
        geode::Logger::debug( "Found horizon ", horizon.id().string() );
        counter++;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        counter == 0, "Range should not have iterated on any horizon." );

    const auto& hor0 = stack_builder.add_horizon();
    const auto& hor1 = stack_builder.add_horizon();
    const geode::uuid hor2{};
    stack_builder.add_horizon( hor2 );
    const auto& unit0 = stack_builder.add_stratigraphic_unit();
    const geode::uuid unit1{};
    stack_builder.add_stratigraphic_unit( unit1 );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.nb_horizons() == 3,
        "Horizons Stack should have 3 horizons." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.nb_stratigraphic_units() == 2,
        "Horizons Stack should have 2 Stratigraphic Units." );
    stack_builder.set_horizon_under( horizons_stack.horizon( hor0 ),
        horizons_stack.stratigraphic_unit( unit0 ) );
    stack_builder.set_horizon_above( horizons_stack.horizon( hor1 ),
        horizons_stack.stratigraphic_unit( unit0 ) );
    stack_builder.set_horizon_under( horizons_stack.horizon( hor1 ),
        horizons_stack.stratigraphic_unit( unit1 ) );
    stack_builder.set_horizon_above( horizons_stack.horizon( hor2 ),
        horizons_stack.stratigraphic_unit( unit1 ) );

    for( const auto& unit : horizons_stack.stratigraphic_units() )
    {
        const auto& unit_id = unit.id();
        geode::OpenGeodeGeosciencesImplicitException::test(
            horizons_stack.above( unit_id ).has_value(),
            "Unit should have a horizon above." );
        geode::OpenGeodeGeosciencesImplicitException::test(
            horizons_stack.under( unit_id ).has_value(),
            "Unit should have a horizon under." );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.above( horizons_stack.above( unit0 ).value() ).value()
            == unit1,
        "Unit 1 should be found from unit 0." );
    const auto insertion_info = stack_builder.add_horizon_in_stratigraphic_unit(
        horizons_stack.stratigraphic_unit( unit1 ) );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.above( hor1 ).value()
            == insertion_info.strati_unit_under_id,
        "New bottom unit should be found above horizon 1." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.above( horizons_stack.above( hor1 ).value() ).value()
            == insertion_info.new_horizon_id,
        "New horizon should be found from horizon 1." );

    geode::detail::repair_horizon_stack_if_possible(
        horizons_stack, stack_builder );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.nb_horizons() == 4,
        "Horizons Stack should have 4 horizons after repair." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.nb_stratigraphic_units() == 5,
        "Horizons Stack should have 5 Stratigraphic Units after "
        "repair." );
    const auto bottom_horizon = horizons_stack.bottom_horizon();
    geode::OpenGeodeGeosciencesImplicitException::test(
        bottom_horizon.has_value(), "There should be a bottom horizon" );
    const auto bottom_unit_id = horizons_stack.under( bottom_horizon.value() );
    geode::OpenGeodeGeosciencesImplicitException::test(
        bottom_unit_id.has_value(),
        "There should be a stratigraphic unit under the "
        "bottom horizon" );
    for( const auto& horizon : horizons_stack.horizons() )
    {
        const auto& horizon_id = horizon.id();
        geode::OpenGeodeGeosciencesImplicitException::test(
            horizons_stack.above( horizon_id ).has_value(),
            "Horizon should have a unit above." );
        geode::OpenGeodeGeosciencesImplicitException::test(
            horizons_stack.under( horizon_id ).has_value(),
            "Horizon should have a unit under." );
        geode::OpenGeodeGeosciencesImplicitException::test(
            horizons_stack.is_above( horizon_id, bottom_unit_id.value() ),
            "Horizon should be above the bottom unit." );
    }

    const auto stack_path = absl::StrCat( "test_HorizonStack.",
        geode::HorizonsStack3D::native_extension_static() );
    geode::save_horizons_stack( horizons_stack, stack_path );
    auto reloaded_stack = geode::load_horizons_stack< 3 >( stack_path );
    auto reloaded_stack_v0 = geode::load_horizons_stack< 3 >(
        absl::StrCat( geode::DATA_PATH, "test_HorizonStack_v0.og_hst3d" ) );
    geode::index_t nb_v0_horizons{ 0 };
    for( const auto& horizon : reloaded_stack_v0.bottom_to_top_horizons() )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            reloaded_stack_v0.above( horizon.id() ).has_value()
                && reloaded_stack_v0.under( horizon.id() ).has_value(),
            "Legacy stack horizon should have units above and under" );
        nb_v0_horizons++;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        nb_v0_horizons == reloaded_stack_v0.nb_horizons(),
        "Should iterate on all horizons of the legacy stack" );
}

void test_horizons_stack( const geode::HorizonsStack2D& horizons_stack )
{
    std::array< std::string, 4 > horizons_list{ "h1", "h2", "h3", "h4" };
    std::array< std::string, 3 > units_list{ "su1", "su2", "su3" };
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.nb_horizons() == 4,
        "Created Horizons Stack should have 4 horizons." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.nb_stratigraphic_units() == 5,
        "Created Horizons Stack should have 5 Stratigraphic Units." );
    const auto bot_horizon = horizons_stack.bottom_horizon().value();
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack.horizon( bot_horizon ).name() == horizons_list[0],
        "Wrong name for bottom horizon." );
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons_stack
                .stratigraphic_unit(
                    horizons_stack.above( bot_horizon ).value() )
                .name()
            == units_list[0],
        "Wrong name for unit above bottom horizon." );
    const auto bot_horizon_id =
        geode::detail::horizon_id_from_name( horizons_stack, horizons_list[0] );
    geode::OpenGeodeGeosciencesImplicitException::test(
        bot_horizon_id && bot_horizon_id.value() == bot_horizon,
        "Should have found bottom horizon from first horizon name" );
    const auto top_horizon_id =
        geode::detail::horizon_id_from_name( horizons_stack, horizons_list[3] );
    geode::OpenGeodeGeosciencesImplicitException::test(
        top_horizon_id
            && top_horizon_id.value() == horizons_stack.top_horizon(),
        "Should have found top horizon from last horizon name" );
    geode::index_t counter{ 0 };
    for( const auto& horizon : horizons_stack.bottom_to_top_horizons() )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            horizon.name() == horizons_list[counter],
            "Should have found horizon ", horizons_list[counter],
            " as horizon number ", counter, ", not ", horizon.name().value() );
        counter++;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        counter == 4, "Bottom to top Range did not pass through all horizons" );
    for( const auto& horizon : horizons_stack.top_to_bottom_horizons() )
    {
        counter--;
        geode::OpenGeodeGeosciencesImplicitException::test(
            horizon.name() == horizons_list[counter],
            "Should have found horizon ", horizons_list[counter],
            " as horizon number ", counter, ", not ", horizon.name().value() );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        counter == 0, "Top to bottom Range did not pass through all horizons" );
    for( const auto& s_unit : horizons_stack.bottom_to_top_units() )
    {
        if( counter != 0 && counter < 4 )
        {
            geode::OpenGeodeGeosciencesImplicitException::test(
                s_unit.name() == units_list[counter - 1],
                "Should have found stratigraphic unit ",
                units_list[counter - 1], " as unit number ", counter, ", not ",
                s_unit.name().value() );
        }
        counter++;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        counter == 5, "Range did not pass through all stratigraphic units" );
    for( const auto& s_unit : horizons_stack.top_to_bottom_units() )
    {
        counter--;
        if( counter != 0 && counter < 4 )
        {
            geode::OpenGeodeGeosciencesImplicitException::test(
                s_unit.name() == units_list[counter - 1],
                "Should have found stratigraphic unit ",
                units_list[counter - 1], " as unit number ", counter, ", not ",
                s_unit.name().value() );
        }
    }
    geode::OpenGeodeGeosciencesImplicitException::test( counter == 0,
        "Top to bottom Range did not "
        "pass through all stratigraphic units" );
}

void test_create_horizons_stack_top_to_bottom()
{
    std::array< std::string, 4 > horizons_list{ "h4", "h3", "h2", "h1" };
    std::array< std::string, 3 > units_list{ "su3", "su2", "su1" };
    const auto horizons_stack =
        geode::detail::horizons_stack_from_top_to_bottom_names< 2 >(
            horizons_list, units_list );
    test_horizons_stack( horizons_stack );
}

void test_create_horizons_stack_bottom_to_top()
{
    std::array< std::string, 4 > horizons_list{ "h1", "h2", "h3", "h4" };
    std::array< std::string, 3 > units_list{ "su1", "su2", "su3" };
    const auto horizons_stack =
        geode::detail::horizons_stack_from_bottom_to_top_names< 2 >(
            horizons_list, units_list );
    test_horizons_stack( horizons_stack );
}

void test_cycle_rejection()
{
    geode::HorizonsStack3D stack;
    geode::HorizonsStackBuilder3D builder{ stack };
    const auto& h0 = builder.add_horizon();
    const auto& h1 = builder.add_horizon();
    const auto& u0 = builder.add_stratigraphic_unit();
    const auto& u1 = builder.add_stratigraphic_unit();
    builder.set_horizon_above(
        stack.horizon( h0 ), stack.stratigraphic_unit( u0 ) );
    builder.set_horizon_under(
        stack.horizon( h1 ), stack.stratigraphic_unit( u0 ) );
    builder.set_horizon_above(
        stack.horizon( h1 ), stack.stratigraphic_unit( u1 ) );
    bool cycle_rejected{ false };
    try
    {
        builder.set_horizon_under(
            stack.horizon( h0 ), stack.stratigraphic_unit( u1 ) );
    }
    catch( const geode::OpenGeodeException& /*unused*/ )
    {
        cycle_rejected = true;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        cycle_rejected, "Closing a cycle of above relations should throw" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        !stack.above( h0 ).has_value(),
        "Rejected relation should not have been added" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        !stack.is_above( h0, h0 ), "Horizon should not be above itself" );
    builder.compute_top_and_bottom_horizons();
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.top_horizon() == h0 && stack.bottom_horizon() == h1,
        "Wrong top and bottom horizons after cycle rejection" );
}

void test_load_cyclic_stack()
{
    // File saved with a cycle h0 > u0 > h1 > u1 > h0 in above relations
    const auto stack = geode::load_horizons_stack< 3 >(
        absl::StrCat( geode::DATA_PATH, "test_HorizonStack_cycle.og_hst3d" ) );
    for( const auto& horizon : stack.horizons() )
    {
        geode::OpenGeodeGeosciencesImplicitException::test(
            !stack.is_above( horizon.id(), horizon.id() ),
            "Cycle should have been removed on load" );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.top_horizon().has_value() && stack.bottom_horizon().has_value()
            && stack.top_horizon() != stack.bottom_horizon(),
        "Top and bottom horizons should be computed after cycle removal" );
    geode::index_t counter{ 0 };
    for( const auto& horizon : stack.top_to_bottom_horizons() )
    {
        geode::Logger::debug( "Found horizon ", horizon.id().string() );
        counter++;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        counter == 2, "Should iterate on 2 horizons after cycle removal" );
}

void test_reverse_relation()
{
    geode::HorizonsStack3D stack;
    geode::HorizonsStackBuilder3D builder{ stack };
    const auto& horizon = builder.add_horizon();
    const auto& unit = builder.add_stratigraphic_unit();
    builder.set_horizon_above(
        stack.horizon( horizon ), stack.stratigraphic_unit( unit ) );
    builder.set_horizon_under(
        stack.horizon( horizon ), stack.stratigraphic_unit( unit ) );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.above( horizon ) == unit && !stack.under( horizon ),
        "Unit should now be above the horizon" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.under( unit ) == horizon && !stack.above( unit ),
        "Horizon should now be under the unit" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.is_directly_above( unit, horizon )
            && !stack.is_directly_above( horizon, unit ),
        "Relation direction should have been reversed" );
}

void test_replace_relation()
{
    geode::HorizonsStack3D stack;
    geode::HorizonsStackBuilder3D builder{ stack };
    const auto& h0 = builder.add_horizon();
    const auto& h1 = builder.add_horizon();
    const auto& u0 = builder.add_stratigraphic_unit();
    const auto& u1 = builder.add_stratigraphic_unit();
    builder.set_horizon_above(
        stack.horizon( h0 ), stack.stratigraphic_unit( u0 ) );
    builder.set_horizon_above(
        stack.horizon( h1 ), stack.stratigraphic_unit( u0 ) );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.above( u0 ) == h1 && !stack.under( h0 ),
        "Horizon above unit should have been replaced" );
    builder.set_horizon_under(
        stack.horizon( h0 ), stack.stratigraphic_unit( u0 ) );
    builder.set_horizon_under(
        stack.horizon( h0 ), stack.stratigraphic_unit( u1 ) );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.above( h0 ) == u1 && !stack.under( u0 ),
        "Unit above horizon should have been replaced" );

    geode::HorizonsStack3D other_stack;
    geode::HorizonsStackBuilder3D other_builder{ other_stack };
    const auto& other_horizon = other_builder.add_horizon();
    bool throws{ false };
    try
    {
        builder.set_horizon_above( other_stack.horizon( other_horizon ),
            stack.stratigraphic_unit( u1 ) );
    }
    catch( const geode::OpenGeodeException& /*unused*/ )
    {
        throws = true;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        throws, "Should not relate a horizon from another stack" );
}

geode::index_t count_horizons( const geode::HorizonsStack2D& stack )
{
    geode::index_t counter{ 0 };
    for( const auto& horizon : stack.bottom_to_top_horizons() )
    {
        geode::Logger::debug( "Found horizon ", horizon.id().string() );
        counter++;
    }
    return counter;
}

geode::index_t count_units( const geode::HorizonsStack2D& stack )
{
    geode::index_t counter{ 0 };
    for( const auto& unit : stack.top_to_bottom_units() )
    {
        geode::Logger::debug( "Found unit ", unit.id().string() );
        counter++;
    }
    return counter;
}

void test_update_top_and_bottom_horizons()
{
    std::array< std::string, 3 > horizons_list{ "h1", "h2", "h3" };
    std::array< std::string, 2 > units_list{ "su1", "su2" };
    auto stack = geode::detail::horizons_stack_from_bottom_to_top_names< 2 >(
        horizons_list, units_list );
    geode::HorizonsStackBuilder2D builder{ stack };
    const auto top_unit = stack.above( stack.top_horizon().value() ).value();
    const auto info = builder.add_horizon_in_stratigraphic_unit(
        stack.stratigraphic_unit( top_unit ) );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.top_horizon() == info.new_horizon_id,
        "Inserted horizon should be the new top horizon" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_horizons( stack ) == 4, "Should iterate on 4 horizons" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_units( stack ) == 5, "Should iterate on 5 units" );

    const auto bottom_horizon = stack.bottom_horizon().value();
    const auto new_bottom =
        stack.above( stack.above( bottom_horizon ).value() );
    builder.remove_horizon( stack.horizon( bottom_horizon ) );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.bottom_horizon() == new_bottom,
        "Bottom horizon should have been updated after removal" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_horizons( stack ) == 3, "Should iterate on 3 horizons" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_units( stack ) == 4, "Should iterate on 4 units" );

    const auto top_horizon = stack.top_horizon().value();
    const auto new_top = stack.under( stack.under( top_horizon ).value() );
    builder.remove_horizon( stack.horizon( top_horizon ) );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.top_horizon() == new_top,
        "Top horizon should have been updated after removal" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_horizons( stack ) == 2, "Should iterate on 2 horizons" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_units( stack ) == 3, "Should iterate on 3 units" );
}

void test_incomplete_stack()
{
    geode::HorizonsStack2D stack;
    geode::HorizonsStackBuilder2D builder{ stack };
    const auto& horizon = builder.add_horizon();
    const auto& unit = builder.add_stratigraphic_unit();
    builder.set_horizon_above(
        stack.horizon( horizon ), stack.stratigraphic_unit( unit ) );
    builder.compute_top_and_bottom_horizons();
    geode::Logger::info( stack.string() );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_horizons( stack ) == 1, "Should iterate on 1 horizon" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_units( stack ) == 1, "Should iterate on 1 unit" );

    std::array< std::string, 3 > horizons_list{ "h1", "h2", "h3" };
    std::array< std::string, 2 > units_list{ "su1", "su2" };
    auto named_stack =
        geode::detail::horizons_stack_from_bottom_to_top_names< 2 >(
            horizons_list, units_list );
    geode::HorizonsStackBuilder2D named_builder{ named_stack };
    const auto bottom = named_stack.bottom_horizon().value();
    const auto unit_above_bottom = named_stack.above( bottom ).value();
    // Creates a gap in the stack without updating top and bottom horizons
    named_builder.remove_above_relation( unit_above_bottom, bottom );
    geode::Logger::info( named_stack.string() );
    geode::OpenGeodeGeosciencesImplicitException::test(
        count_horizons( named_stack ) == 1,
        "Should stop horizon iteration at the gap" );
    geode::index_t counter{ 0 };
    for( const auto& unit_in_stack : named_stack.bottom_to_top_units() )
    {
        geode::Logger::debug( "Found unit ", unit_in_stack.id().string() );
        counter++;
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        counter == 1, "Should stop unit iteration at the gap" );
}

void test_create_empty_horizons_stack()
{
    const auto bottom_to_top =
        geode::detail::horizons_stack_from_bottom_to_top_names< 3 >( {}, {} );
    geode::OpenGeodeGeosciencesImplicitException::test(
        bottom_to_top.nb_horizons() == 0
            && bottom_to_top.nb_stratigraphic_units() == 0,
        "Stack created from empty bottom to top lists should be empty" );
    const auto top_to_bottom =
        geode::detail::horizons_stack_from_top_to_bottom_names< 3 >( {}, {} );
    geode::OpenGeodeGeosciencesImplicitException::test(
        top_to_bottom.nb_horizons() == 0
            && top_to_bottom.nb_stratigraphic_units() == 0,
        "Stack created from empty top to bottom lists should be empty" );
}

void test_repair_stack_without_horizon()
{
    geode::HorizonsStack3D stack;
    geode::HorizonsStackBuilder3D builder{ stack };
    geode::detail::repair_horizon_stack_if_possible( stack, builder );
    builder.add_stratigraphic_unit();
    geode::detail::repair_horizon_stack_if_possible( stack, builder );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.nb_horizons() == 0 && stack.nb_stratigraphic_units() == 1,
        "Repair of a stack without horizon should not change it" );
}

void test_large_stack_relations()
{
    constexpr geode::index_t nb_horizons{ 200 };
    std::vector< std::string > horizons_names;
    std::vector< std::string > units_names;
    for( const auto index : geode::Range{ nb_horizons } )
    {
        horizons_names.push_back( absl::StrCat( "h", index ) );
        units_names.push_back( absl::StrCat( "su", index ) );
    }
    units_names.pop_back();
    auto stack = geode::detail::horizons_stack_from_bottom_to_top_names< 3 >(
        horizons_names, units_names );
    std::vector< geode::uuid > horizons;
    for( const auto& horizon : stack.bottom_to_top_horizons() )
    {
        horizons.push_back( horizon.id() );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        horizons.size() == nb_horizons, "Wrong number of horizons" );
    for( const auto index : geode::Range{ 1, nb_horizons } )
    {
        const auto unit = stack.above( horizons[index - 1] ).value();
        geode::OpenGeodeGeosciencesImplicitException::test(
            stack.is_directly_above( unit, horizons[index - 1] )
                && stack.is_directly_above( horizons[index], unit )
                && !stack.is_directly_above( unit, horizons[index] )
                && !stack.is_directly_above(
                    horizons[index], horizons[index - 1] ),
            "Wrong direct relations in large stack" );
    }
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.is_above( horizons.back(), horizons.front() )
            && !stack.is_above( horizons.front(), horizons.back() ),
        "Wrong indirect relations in large stack" );
    geode::HorizonsStackBuilder3D builder{ stack };
    const auto middle = nb_horizons / 2;
    const auto unit_above = stack.above( horizons[middle] ).value();
    const auto unit_under = stack.under( horizons[middle] ).value();
    // Removal works whatever the order of the given components
    builder.remove_above_relation( unit_above, horizons[middle] );
    builder.remove_above_relation( unit_under, horizons[middle] );
    geode::OpenGeodeGeosciencesImplicitException::test(
        !stack.above( horizons[middle] ) && !stack.under( horizons[middle] )
            && !stack.under( unit_above ) && !stack.above( unit_under ),
        "Relations should have been removed in large stack" );
    geode::OpenGeodeGeosciencesImplicitException::test(
        stack.is_above( horizons.back(), horizons[middle + 1] )
            && !stack.is_above( horizons.back(), horizons.front() ),
        "Wrong indirect relations after removal in large stack" );
}

int main()
{
    try
    {
        geode::Logger::info( "Starting test" );
        geode::OpenGeodeGeosciencesImplicitLibrary::initialize();
        test_horizons_stack();
        test_create_horizons_stack_bottom_to_top();
        test_create_horizons_stack_top_to_bottom();
        test_cycle_rejection();
        test_load_cyclic_stack();
        test_reverse_relation();
        test_replace_relation();
        test_update_top_and_bottom_horizons();
        test_incomplete_stack();
        test_create_empty_horizons_stack();
        test_repair_stack_without_horizon();
        test_large_stack_relations();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}