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

#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/builder/structural_model_builder.h>
#include <geode/geosciences/core/fault.h>
#include <geode/geosciences/core/horizon.h>
#include <geode/geosciences/core/structural_model.h>
#include <geode/geosciences/io/structural_model_input.h>
#include <geode/geosciences/io/structural_model_output.h>

void add_faults(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    const auto& fault0 = builder.add_fault();
    OPENGEODE_EXCEPTION( !model.fault( fault0 ).has_type(),
        "Addition of a Fault in StructuralModel is not correct" );

    const auto& fault1 =
        builder.add_fault( geode::Fault3D::FAULT_TYPE::REVERSE );
    OPENGEODE_EXCEPTION(
        model.fault( fault1 ).type() == geode::Fault3D::FAULT_TYPE::REVERSE,
        "Addition of a Fault in StructuralModel is not correct (wrong type)" );
    OPENGEODE_EXCEPTION( model.nb_faults() == 2,
        "Number of faults in StructuralModel should be 2" );
}

void add_horizons(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    const auto& horizon0 = builder.add_horizon();
    OPENGEODE_EXCEPTION( !model.horizon( horizon0 ).has_type(),
        "Addition of a Horizon in StructuralModel is not correct" );

    const auto& horizon1 =
        builder.add_horizon( geode::Horizon3D::HORIZON_TYPE::CONFORMAL );
    OPENGEODE_EXCEPTION( model.horizon( horizon1 ).type()
                             == geode::Horizon3D::HORIZON_TYPE::CONFORMAL,
        "Addition of a Horizon in StructuralModel is not correct (wrong "
        "type)" );

    const auto& horizon2 =
        builder.add_horizon( geode::Horizon3D::HORIZON_TYPE::NO_TYPE );
    OPENGEODE_EXCEPTION( !model.horizon( horizon2 ).has_type(),
        "Addition of a Horizon in StructuralModel is not correct (no type)" );
    OPENGEODE_EXCEPTION( model.nb_horizons() == 3,
        "Number of horizons in StructuralModel should be 3" );
}

void add_surfaces( geode::StructuralModelBuilder& builder )
{
    for( auto i : geode::Range( 8 ) )
    {
        geode_unused( i );
        builder.add_surface();
    }
}

void do_checks( const geode::StructuralModel& model,
    const std::vector< geode::uuid >& surface_uuids,
    const std::vector< geode::uuid >& faults_uuids,
    const std::vector< geode::uuid >& horizons_uuids )
{
    OPENGEODE_EXCEPTION( model.relationships().nb_items( faults_uuids[0] ) == 3,
        "Number of items in fault_uuids[0] should be 3" );
    OPENGEODE_EXCEPTION( model.relationships().nb_items( faults_uuids[1] ) == 2,
        "Number of items in fault_uuids[1] should be 2" );

    OPENGEODE_EXCEPTION(
        model.relationships().nb_items( horizons_uuids[0] ) == 1,
        "Number of items in horizons_uuids[0] should be 1" );
    OPENGEODE_EXCEPTION(
        model.relationships().nb_items( horizons_uuids[1] ) == 0,
        "Number of items in horizons_uuids[1] should be 0" );
    OPENGEODE_EXCEPTION(
        model.relationships().nb_items( horizons_uuids[2] ) == 3,
        "Number of items in horizons_uuids[2] should be 3" );

    for( auto i : geode::Range( 8 ) )
    {
        if( i == 2 )
        {
            continue;
        }
        OPENGEODE_EXCEPTION(
            model.relationships().nb_collections( surface_uuids[i] ) == 1,
            "Number of collections in which surfaces_uuids["
                + std::to_string( i ) + "] is should be 1" );
    }
}

void build_relations_between_geometry_and_geology(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    std::vector< geode::uuid > surfaces_uuids;
    surfaces_uuids.reserve( model.nb_surfaces() );
    for( const auto& surface : model.surfaces() )
    {
        surfaces_uuids.push_back( surface.id() );
    }
    std::vector< geode::uuid > faults_uuids;
    surfaces_uuids.reserve( model.nb_faults() );
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

    builder.add_surface_in_fault(
        model.surface( surfaces_uuids[0] ), model.fault( faults_uuids[0] ) );
    builder.add_surface_in_fault(
        model.surface( surfaces_uuids[1] ), model.fault( faults_uuids[0] ) );
    builder.add_surface_in_fault(
        model.surface( surfaces_uuids[2] ), model.fault( faults_uuids[0] ) );
    builder.add_surface_in_fault(
        model.surface( surfaces_uuids[2] ), model.fault( faults_uuids[1] ) );
    builder.add_surface_in_fault(
        model.surface( surfaces_uuids[3] ), model.fault( faults_uuids[1] ) );

    builder.add_surface_in_horizon( model.surface( surfaces_uuids[4] ),
        model.horizon( horizons_uuids[0] ) );
    builder.add_surface_in_horizon( model.surface( surfaces_uuids[5] ),
        model.horizon( horizons_uuids[2] ) );
    builder.add_surface_in_horizon( model.surface( surfaces_uuids[6] ),
        model.horizon( horizons_uuids[2] ) );
    builder.add_surface_in_horizon( model.surface( surfaces_uuids[7] ),
        model.horizon( horizons_uuids[2] ) );

    do_checks( model, surfaces_uuids, faults_uuids, horizons_uuids );
}

void check_reloaded_model( const geode::StructuralModel& reloaded_model )
{
    OPENGEODE_EXCEPTION( reloaded_model.nb_surfaces() == 8,
        "Number of surfaces in reloaded model should be 8" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_horizons() == 3,
        "Number of horizons in reloaded model should be 3" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_faults() == 2,
        "Number of faults in reloaded model should be 2" );
}

void test_io( const geode::StructuralModel& model )
{
    std::string file_io{ "test." + model.native_extension() };
    save_structural_model( model, file_io );

    geode::StructuralModel reloaded_model;
    load_structural_model( reloaded_model, file_io );
    check_reloaded_model( reloaded_model );
}

void modify_model(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    // Modify faults and horizons
    for( const auto& fault : model.faults() )
    {
        if( !fault.has_type() )
        {
            builder.set_fault_type(
                fault.id(), geode::Fault3D::FAULT_TYPE::STRIKE_SLIP );
        }
        OPENGEODE_EXCEPTION(
            fault.has_type(), "All faults should have a type" );
    }
    for( const auto& horizon : model.horizons() )
    {
        if( !horizon.has_type() )
        {
            builder.set_horizon_type(
                horizon.id(), geode::Horizon3D::HORIZON_TYPE::NON_CONFORMAL );
        }
        OPENGEODE_EXCEPTION(
            horizon.has_type(), "All horizons should have a type" );
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
        "Number of horizons in modified model should be 0" );
    OPENGEODE_EXCEPTION( model.nb_faults() == 2,
        "Number of faults in modified model should be 2" );
}

int main()
{
    using namespace geode;

    try
    {
        StructuralModel model;
        StructuralModelBuilder builder( model );
        add_faults( model, builder );
        add_horizons( model, builder );
        add_surfaces( builder );
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
