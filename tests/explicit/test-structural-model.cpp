/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_output.hpp>

template < class ItemRange >
geode::index_t count_items( ItemRange items )
{
    geode::index_t count{ 0 };
    for( const auto& item : items )
    {
        geode_unused( item );
        count++;
    }
    return count;
}

void add_faults(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    const auto& fault0 = builder.add_fault();
    OPENGEODE_EXCEPTION( !model.fault( fault0 ).has_type(),
        "[Test] Addition of a Fault in StructuralModel is not correct" );

    const auto& fault1 =
        builder.add_fault( geode::Fault3D::FAULT_TYPE::reverse );
    builder.set_fault_name( fault1, "fault1" );
    OPENGEODE_EXCEPTION(
        model.fault( fault1 ).type() == geode::Fault3D::FAULT_TYPE::reverse,
        "[Test] Addition of a Fault in StructuralModel is not correct (wrong "
        "type)" );
    OPENGEODE_EXCEPTION( model.nb_faults() == 2,
        "[Test] Number of faults in StructuralModel should be 2" );
    OPENGEODE_EXCEPTION(
        model.fault( fault1 ).name() == "fault1", "[Test] Wrong Fault name" );

    for( auto& fault : model.faults() )
    {
        if( fault.name() != "unknown" )
        {
            builder.set_fault_name(
                fault.id(), absl::StrCat( "new_", fault.name() ) );
        }
        OPENGEODE_EXCEPTION( fault.component_type().get() == "Fault",
            "[Test] Wrong Fault component type" );
    }
    OPENGEODE_EXCEPTION( model.fault( fault1 ).name() == "new_fault1",
        "[Test] Wrong modified Fault name" );
}

void add_horizons(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    const auto& horizon0 = builder.add_horizon();
    OPENGEODE_EXCEPTION( !model.horizon( horizon0 ).has_type(),
        "[Test] Addition of a Horizon in StructuralModel is not correct" );

    const auto& horizon1 =
        builder.add_horizon( geode::Horizon3D::HORIZON_TYPE::conformal );
    OPENGEODE_EXCEPTION( model.horizon( horizon1 ).type()
                             == geode::Horizon3D::HORIZON_TYPE::conformal,
        "[Test] Addition of a Horizon in StructuralModel is not correct (wrong "
        "type)" );

    const auto& horizon2 =
        builder.add_horizon( geode::Horizon3D::HORIZON_TYPE::no_type );
    builder.set_horizon_name( horizon2, "horizon2" );
    OPENGEODE_EXCEPTION( !model.horizon( horizon2 ).has_type(),
        "[Test] Addition of a Horizon in StructuralModel is not correct (no "
        "type)" );
    OPENGEODE_EXCEPTION( model.nb_horizons() == 3,
        "[Test] Number of horizons in StructuralModel should be 3" );
    OPENGEODE_EXCEPTION( model.horizon( horizon2 ).name() == "horizon2",
        "[Test] Wrong Horizon name" );

    for( auto& horizon : model.horizons() )
    {
        if( horizon.name() != "unknown" )
        {
            builder.set_horizon_name(
                horizon.id(), absl::StrCat( "new_", horizon.name() ) );
        }
        OPENGEODE_EXCEPTION( horizon.component_type().get() == "Horizon",
            "[Test] Wrong Horizon component type" );
    }
    OPENGEODE_EXCEPTION( model.horizon( horizon2 ).name() == "new_horizon2",
        "[Test] Wrong modified Horizon name" );
}

void add_lines( geode::BRepBuilder& builder )
{
    for( const auto i : geode::Range{ 8 } )
    {
        geode_unused( i );
        const auto& uid = builder.add_line();
        geode_unused( uid );
    }
}

void add_surfaces( geode::BRepBuilder& builder )
{
    for( const auto i : geode::Range{ 8 } )
    {
        geode_unused( i );
        const auto& uid = builder.add_surface();
        geode_unused( uid );
    }
}

void do_checks( const geode::StructuralModel& model,
    absl::Span< const geode::uuid > surface_uuids,
    absl::Span< const geode::uuid > faults_uuids,
    absl::Span< const geode::uuid > horizons_uuids )
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

    for( const auto i : geode::Range{ 8 } )
    {
        if( i == 2 )
        {
            continue;
        }
        OPENGEODE_EXCEPTION( model.nb_collections( surface_uuids[i] ) == 1,
            "[Test] Number of collections in which surfaces_uuids[", i,
            "] is should be 1" );
    }

    OPENGEODE_EXCEPTION(
        count_items( model.horizon_items( model.horizon( horizons_uuids[2] ) ) )
            == 3,
        "[Test] Number of iterations on items in "
        "horizons_uuids[2] should be 3" );
    OPENGEODE_EXCEPTION(
        count_items( model.fault_items( model.fault( faults_uuids[1] ) ) ) == 2,
        "[Test] Number of iterations on items in "
        "faults_uuids[1] should be 2" );
}

void build_relations_between_geometry_and_geology(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    OPENGEODE_EXCEPTION(
        model.nb_lines() == 8, "[Test] Wrong number of lines" );
    OPENGEODE_EXCEPTION(
        model.nb_surfaces() == 8, "[Test] Wrong number of surfaces" );
    std::vector< geode::uuid > surfaces_uuids;
    surfaces_uuids.reserve( model.nb_surfaces() );
    for( const auto& surface : model.surfaces() )
    {
        surfaces_uuids.push_back( surface.id() );
        builder.add_line_surface_boundary_relationship(
            ( *model.lines().begin() ), surface );
    }
    std::vector< geode::uuid > faults_uuids;
    faults_uuids.reserve( model.nb_faults() );
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
        "[Test] Number of surfaces in reloaded model should be 8" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_horizons() == 3,
        "[Test] Number of horizons in reloaded model should be 3" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_faults() == 2,
        "[Test] Number of faults in reloaded model should be 2" );
}

void test_io( const geode::StructuralModel& model )
{
    const auto file_io = absl::StrCat( "test.", model.native_extension() );
    geode::save_structural_model( model, file_io );

    geode::StructuralModel reloaded_model =
        geode::load_structural_model( file_io );
    check_reloaded_model( reloaded_model );
}

void test_copy( const geode::StructuralModel& model )
{
    geode::StructuralModel copy;
    geode::StructuralModelBuilder copier( copy );
    const auto mapping = copier.copy( model );
    OPENGEODE_EXCEPTION( copy.nb_surfaces() == model.nb_surfaces(),
        "[Test] Number of surfaces in copied model should be 8" );
    OPENGEODE_EXCEPTION( copy.nb_horizons() == model.nb_horizons(),
        "[Test] Number of horizons in copied model should be 3" );
    OPENGEODE_EXCEPTION( copy.nb_faults() == model.nb_faults(),
        "[Test] Number of faults in copied model should be 2" );
    OPENGEODE_EXCEPTION( copy.nb_fault_blocks() == model.nb_fault_blocks(),
        "[Test] Number of fault blocks in copied model should be 0" );
    const auto& surfaces_mapping =
        mapping.at( geode::Surface3D::component_type_static() );
    const auto& lines_mapping =
        mapping.at( geode::Line3D::component_type_static() );
    for( const auto& surface : model.surfaces() )
    {
        const auto& surface_copy =
            copy.surface( surfaces_mapping.in2out( surface.id() ) );
        for( const auto& boundary : model.boundaries( surface ) )
        {
            const auto& boundary_copy =
                copy.line( lines_mapping.in2out( boundary.id() ) );
            OPENGEODE_EXCEPTION(
                copy.is_boundary( boundary_copy, surface_copy ),
                "[Test] Copied line ", boundary_copy.component_id().string(),
                " should be boundary of copied surface ",
                surface_copy.component_id().string() );
        }
    }
    const auto& faults_mapping =
        mapping.at( geode::Fault3D::component_type_static() );
    for( const auto& fault : model.faults() )
    {
        const auto& fault_copy =
            copy.fault( faults_mapping.in2out( fault.id() ) );
        for( const auto& item : model.fault_items( fault ) )
        {
            const auto& item_copy =
                copy.surface( surfaces_mapping.in2out( item.id() ) );
            OPENGEODE_EXCEPTION(
                copy.is_item( item_copy.id(), fault_copy.id() ),
                "[Test] Copied line ", item_copy.component_id().string(),
                " should be item of copied fault ",
                fault_copy.component_id().string() );
        }
    }
    const auto& horizons_mapping =
        mapping.at( geode::Horizon3D::component_type_static() );
    for( const auto& horizon : model.horizons() )
    {
        const auto& horizon_copy =
            copy.horizon( horizons_mapping.in2out( horizon.id() ) );
        for( const auto& item : model.horizon_items( horizon ) )
        {
            const auto& item_copy =
                copy.surface( surfaces_mapping.in2out( item.id() ) );
            OPENGEODE_EXCEPTION(
                copy.is_item( item_copy.id(), horizon_copy.id() ),
                "[Test] Copied line ", item_copy.component_id().string(),
                " should be item of copied horizon ",
                horizon_copy.component_id().string() );
        }
    }
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
                fault.id(), geode::Fault3D::FAULT_TYPE::strike_slip );
        }
        OPENGEODE_EXCEPTION(
            fault.has_type(), "[Test] All faults should have a type" );
    }
    for( const auto& horizon : model.horizons() )
    {
        if( !horizon.has_type() )
        {
            builder.set_horizon_type(
                horizon.id(), geode::Horizon3D::HORIZON_TYPE::non_conformal );
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
    const auto temp_id = builder.add_fault();
    builder.remove_fault( model.fault( temp_id ) );
    OPENGEODE_EXCEPTION( model.nb_horizons() == 0,
        "[Test] Number of horizons in modified model should be 0" );
    OPENGEODE_EXCEPTION( model.nb_faults() == 2,
        "[Test] Number of faults in modified model should be 2" );
}

geode::BRep build_brep()
{
    geode::BRep brep;
    geode::BRepBuilder brep_builder( brep );
    add_surfaces( brep_builder );
    add_lines( brep_builder );
    return brep;
}

int main()
{
    try
    {
        geode::GeosciencesExplicitLibrary::initialize();

        geode::StructuralModel model = build_brep();
        geode::StructuralModelBuilder builder( model );
        add_faults( model, builder );
        add_horizons( model, builder );
        build_relations_between_geometry_and_geology( model, builder );

        test_io( model );
        test_copy( model );
        modify_model( model, builder );

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
