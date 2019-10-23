/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>

#include <geode/model/mixin/core/line.h>

#include <geode/geosciences/mixin/core/fault.h>
#include <geode/geosciences/mixin/core/horizon.h>
#include <geode/geosciences/representation/builder/cross_section_builder.h>
#include <geode/geosciences/representation/core/cross_section.h>
#include <geode/geosciences/representation/io/cross_section_input.h>
#include <geode/geosciences/representation/io/cross_section_output.h>

void add_faults(
    geode::CrossSection& model, geode::CrossSectionBuilder& builder )
{
    const auto& fault0 = builder.add_fault();
    OPENGEODE_EXCEPTION( !model.fault( fault0 ).has_type(),
        "[Test] Addition of a Fault in CrossSection is not correct" );

    const auto& fault1 =
        builder.add_fault( geode::Fault2D::FAULT_TYPE::REVERSE );
    builder.set_fault_name( fault1, "fault1" );
    OPENGEODE_EXCEPTION(
        model.fault( fault1 ).type() == geode::Fault2D::FAULT_TYPE::REVERSE,
        "[Test] Addition of a Fault in CrossSection is not correct (wrong "
        "type)" );
    OPENGEODE_EXCEPTION( model.nb_faults() == 2,
        "[Test] Number of faults in CrossSection should be 2" );
    OPENGEODE_EXCEPTION(
        model.fault( fault1 ).name() == "fault1", "[Test] Wrong Fault name" );
}

void add_horizons(
    geode::CrossSection& model, geode::CrossSectionBuilder& builder )
{
    const auto& horizon0 = builder.add_horizon();
    OPENGEODE_EXCEPTION( !model.horizon( horizon0 ).has_type(),
        "[Test] Addition of a Horizon in CrossSection is not correct" );

    const auto& horizon1 =
        builder.add_horizon( geode::Horizon2D::HORIZON_TYPE::CONFORMAL );
    OPENGEODE_EXCEPTION( model.horizon( horizon1 ).type()
                             == geode::Horizon2D::HORIZON_TYPE::CONFORMAL,
        "[Test] Addition of a Horizon in CrossSection is not correct (wrong "
        "type)" );

    const auto& horizon2 =
        builder.add_horizon( geode::Horizon2D::HORIZON_TYPE::NO_TYPE );
    builder.set_horizon_name( horizon2, "horizon2" );
    OPENGEODE_EXCEPTION( !model.horizon( horizon2 ).has_type(),
        "[Test] Addition of a Horizon in CrossSection is not correct (no "
        "type)" );
    OPENGEODE_EXCEPTION( model.nb_horizons() == 3,
        "[Test] Number of horizons in CrossSection should be 3" );
    OPENGEODE_EXCEPTION( model.horizon( horizon2 ).name() == "horizon2",
        "[Test] Wrong Horizon name" );
}

void add_lines( geode::CrossSectionBuilder& builder )
{
    for( auto i : geode::Range{ 8 } )
    {
        geode_unused( i );
        builder.add_line();
    }
}

void do_checks( const geode::CrossSection& model,
    const std::vector< geode::uuid >& line_uuids,
    const std::vector< geode::uuid >& faults_uuids,
    const std::vector< geode::uuid >& horizons_uuids )
{
    OPENGEODE_EXCEPTION( model.nb_items( faults_uuids[0] ) == 3,
        "[Test] Number of items in fault_uuids[0] should be 3" );
    OPENGEODE_EXCEPTION( model.nb_items( faults_uuids[1] ) == 2,
        "[Test] Number of items in fault_uuids[1] should be 2" );

    OPENGEODE_EXCEPTION( model.nb_items( horizons_uuids[0] ) == 1,
        "[Test] Number of items in horizons_uuids[0] should be 1" );
    OPENGEODE_EXCEPTION( model.nb_items( horizons_uuids[1] ) == 0,
        "[Test] Number of items in horizons_uuids[1] should be 0" );
    OPENGEODE_EXCEPTION( model.nb_items( horizons_uuids[2] ) == 3,
        "[Test] Number of items in horizons_uuids[2] should be 3" );

    for( auto i : geode::Range{ 8 } )
    {
        if( i == 2 )
        {
            continue;
        }
        OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[i] ) == 1,
            "[Test] Number of collections in which line_uuids["
                + std::to_string( i ) + "] is should be 1" );
    }
}

void build_relations_between_geometry_and_geology(
    geode::CrossSection& model, geode::CrossSectionBuilder& builder )
{
    std::vector< geode::uuid > lines_uuids;
    lines_uuids.reserve( model.nb_lines() );
    for( const auto& line : model.lines() )
    {
        lines_uuids.push_back( line.id() );
    }
    std::vector< geode::uuid > faults_uuids;
    lines_uuids.reserve( model.nb_faults() );
    for( const auto& fault : model.faults() )
    {
        faults_uuids.push_back( fault.id() );
    }
    std::vector< geode::uuid > horizons_uuids;
    horizons_uuids.reserve( model.nb_horizons() );
    for( const auto& horizon : model.horizons() )
    {
        horizons_uuids.push_back( horizon.id() );
    }

    builder.add_line_in_fault(
        model.line( lines_uuids[0] ), model.fault( faults_uuids[0] ) );
    builder.add_line_in_fault(
        model.line( lines_uuids[1] ), model.fault( faults_uuids[0] ) );
    builder.add_line_in_fault(
        model.line( lines_uuids[2] ), model.fault( faults_uuids[0] ) );
    builder.add_line_in_fault(
        model.line( lines_uuids[2] ), model.fault( faults_uuids[1] ) );
    builder.add_line_in_fault(
        model.line( lines_uuids[3] ), model.fault( faults_uuids[1] ) );

    builder.add_line_in_horizon( model.line( lines_uuids[4] ),
        model.horizon( horizons_uuids[0] ) );
    builder.add_line_in_horizon( model.line( lines_uuids[5] ),
        model.horizon( horizons_uuids[2] ) );
    builder.add_line_in_horizon( model.line( lines_uuids[6] ),
        model.horizon( horizons_uuids[2] ) );
    builder.add_line_in_horizon( model.line( lines_uuids[7] ),
        model.horizon( horizons_uuids[2] ) );

    do_checks( model, lines_uuids, faults_uuids, horizons_uuids );
}

void check_reloaded_model( const geode::CrossSection& reloaded_model )
{
    OPENGEODE_EXCEPTION( reloaded_model.nb_lines() == 8,
        "[Test] Number of lines in reloaded model should be 8" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_horizons() == 3,
        "[Test] Number of horizons in reloaded model should be 3" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_faults() == 2,
        "[Test] Number of faults in reloaded model should be 2" );
}

void test_io( const geode::CrossSection& model )
{
    std::string file_io{ "test." + model.native_extension() };
    save_cross_section( model, file_io );

    geode::CrossSection reloaded_model;
    load_cross_section( reloaded_model, file_io );
    check_reloaded_model( reloaded_model );
}

void modify_model(
    geode::CrossSection& model, geode::CrossSectionBuilder& builder )
{
    // Modify faults and horizons
    for( const auto& fault : model.faults() )
    {
        if( !fault.has_type() )
        {
            builder.set_fault_type(
                fault.id(), geode::Fault2D::FAULT_TYPE::STRIKE_SLIP );
        }
        OPENGEODE_EXCEPTION(
            fault.has_type(), "[Test] All faults should have a type" );
    }
    for( const auto& horizon : model.horizons() )
    {
        if( !horizon.has_type() )
        {
            builder.set_horizon_type(
                horizon.id(), geode::Horizon2D::HORIZON_TYPE::NON_CONFORMAL );
        }
        OPENGEODE_EXCEPTION(
            horizon.has_type(), "[Test] All horizons should have a type" );
    }

    // Remove all horizons
    std::vector< geode::uuid > horizons_ids;
    horizons_ids.reserve( model.nb_horizons() );
    for( const auto& horizon : model.horizons() )
    {
        horizons_ids.emplace_back( horizon.id() );
    }
    for( const auto& horizon_id : horizons_ids )
    {
        builder.remove_horizon( model.horizon( horizon_id ) );
    }

    // Add a new fault and remove it
    auto temp_id = builder.add_fault();
    builder.remove_fault( model.fault( temp_id ) );
    OPENGEODE_EXCEPTION( model.nb_horizons() == 0,
        "[Test] Number of horizons in modified model should be 0" );
    OPENGEODE_EXCEPTION( model.nb_faults() == 2,
        "[Test] Number of faults in modified model should be 2" );
}

int main()
{
    using namespace geode;

    try
    {
        CrossSection model;
        CrossSectionBuilder builder( model );
        add_faults( model, builder );
        add_horizons( model, builder );
        add_lines( builder );
        build_relations_between_geometry_and_geology( model, builder );

        test_io( model );
        modify_model( model, builder );

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
