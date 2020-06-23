/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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
#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/mixin/core/fault.h>
#include <geode/geosciences/mixin/core/fault_block.h>
#include <geode/geosciences/mixin/core/horizon.h>
#include <geode/geosciences/mixin/core/stratigraphic_unit.h>
#include <geode/geosciences/representation/builder/cross_section_builder.h>
#include <geode/geosciences/representation/core/cross_section.h>
#include <geode/geosciences/representation/io/cross_section_input.h>
#include <geode/geosciences/representation/io/cross_section_output.h>

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

void add_fault_blocks(
    geode::CrossSection& model, geode::CrossSectionBuilder& builder )
{
    const auto& fault_block0 = builder.add_fault_block();
    const auto& fault_block1 = builder.add_fault_block();
    builder.add_fault_block();
    builder.set_fault_block_name( fault_block1, "hanging_wall" );
    OPENGEODE_EXCEPTION( model.nb_fault_blocks() == 3,
        "[Test] Number of fault blocks in CrossSection should be 3" );
    builder.remove_fault_block( model.fault_block( fault_block0 ) );
    OPENGEODE_EXCEPTION(
        model.fault_block( fault_block1 ).name() == "hanging_wall",
        "[Test] Wrong FaultBlock name" );
    OPENGEODE_EXCEPTION( model.nb_fault_blocks() == 2,
        "[Test] Number of fault blocks in CrossSection should be 2" );
}

void add_stratigraphic_units(
    geode::CrossSection& model, geode::CrossSectionBuilder& builder )
{
    const auto& stratigraphic_unit0 = builder.add_stratigraphic_unit();
    const auto& stratigraphic_unit1 = builder.add_stratigraphic_unit();
    builder.add_stratigraphic_unit();
    builder.set_stratigraphic_unit_name( stratigraphic_unit1, "cretaceous" );
    OPENGEODE_EXCEPTION( model.nb_stratigraphic_units() == 3,
        "[Test] Number of stratigraphic_units in CrossSection should be 3" );
    builder.remove_stratigraphic_unit(
        model.stratigraphic_unit( stratigraphic_unit0 ) );
    OPENGEODE_EXCEPTION(
        model.stratigraphic_unit( stratigraphic_unit1 ).name() == "cretaceous",
        "[Test] Wrong StratigraphicUnit name" );
    OPENGEODE_EXCEPTION( model.nb_stratigraphic_units() == 2,
        "[Test] Number of stratigraphic_units in CrossSection should be 2" );
}

void add_lines( geode::CrossSectionBuilder& builder )
{
    for( const auto i : geode::Range{ 8 } )
    {
        geode_unused( i );
        builder.add_line();
    }
}

void add_surfaces( geode::CrossSectionBuilder& builder )
{
    for( const auto i : geode::Range{ 4 } )
    {
        geode_unused( i );
        builder.add_surface();
    }
}

void do_checks( const geode::CrossSection& model,
    absl::Span< const geode::uuid > line_uuids,
    absl::Span< const geode::uuid > surface_uuids,
    absl::Span< const geode::uuid > faults_uuids,
    absl::Span< const geode::uuid > horizons_uuids,
    absl::Span< const geode::uuid > fault_blocks_uuids,
    absl::Span< const geode::uuid > stratigraphic_units_uuids )
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

    OPENGEODE_EXCEPTION( model.nb_items( fault_blocks_uuids[0] ) == 2,
        "[Test] Number of items in fault_blocks_uuids[0] should be 2" );
    OPENGEODE_EXCEPTION( model.nb_items( fault_blocks_uuids[1] ) == 2,
        "[Test] Number of items in fault_blocks_uuids[1] should be 2" );

    OPENGEODE_EXCEPTION( model.nb_items( stratigraphic_units_uuids[0] ) == 2,
        "[Test] Number of items in stratigraphic_units_uuids[0] should be 2" );
    OPENGEODE_EXCEPTION( model.nb_items( stratigraphic_units_uuids[1] ) == 2,
        "[Test] Number of items in stratigraphic_units_uuids[1] should be 2" );

    for( const auto i : geode::Range{ 8 } )
    {
        if( i == 2 )
        {
            continue;
        }
        OPENGEODE_EXCEPTION( model.nb_collections( line_uuids[i] ) == 1,
            "[Test] Number of collections in which line_uuids[", i,
            "] is should be 1" );
    }

    for( const auto i : geode::Range{ 4 } )
    {
        OPENGEODE_EXCEPTION( model.nb_collections( surface_uuids[i] ) == 2,
            "[Test] Number of collections in which surface_uuids[", i,
            "] is should be 2 (one StratigraphicUnit and one FaultBlock" );
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
    OPENGEODE_EXCEPTION( count_items( model.fault_block_items(
                             model.fault_block( fault_blocks_uuids[0] ) ) )
                             == 2,
        "[Test] Number of iterations on items in "
        "fault_blocks_uuids[0] should be 2" );
    OPENGEODE_EXCEPTION(
        count_items( model.stratigraphic_unit_items(
            model.stratigraphic_unit( stratigraphic_units_uuids[0] ) ) )
            == 2,
        "[Test] Number of iterations on items in "
        "stratigraphic_units_uuids[0] should be 2" );
}

void build_relations_between_geometry_and_geology(
    geode::CrossSection& model, geode::CrossSectionBuilder& builder )
{
    absl::FixedArray< geode::uuid > lines_uuids( model.nb_lines() );
    geode::index_t l{ 0 };
    for( const auto& line : model.lines() )
    {
        lines_uuids[l++] = line.id();
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

    builder.add_line_in_horizon(
        model.line( lines_uuids[4] ), model.horizon( horizons_uuids[0] ) );
    builder.add_line_in_horizon(
        model.line( lines_uuids[5] ), model.horizon( horizons_uuids[2] ) );
    builder.add_line_in_horizon(
        model.line( lines_uuids[6] ), model.horizon( horizons_uuids[2] ) );
    builder.add_line_in_horizon(
        model.line( lines_uuids[7] ), model.horizon( horizons_uuids[2] ) );

    absl::FixedArray< geode::uuid > surfaces_uuids( model.nb_surfaces() );
    geode::index_t s{ 0 };
    for( const auto& surface : model.surfaces() )
    {
        surfaces_uuids[s++] = surface.id();
    }

    std::vector< geode::uuid > fault_blocks_uuids;
    fault_blocks_uuids.reserve( model.nb_fault_blocks() );
    for( const auto& fault_block : model.fault_blocks() )
    {
        fault_blocks_uuids.push_back( fault_block.id() );
    }

    std::vector< geode::uuid > stratigraphic_units_uuids;
    stratigraphic_units_uuids.reserve( model.nb_stratigraphic_units() );
    for( const auto& stratigraphic_unit : model.stratigraphic_units() )
    {
        stratigraphic_units_uuids.push_back( stratigraphic_unit.id() );
    }

    builder.add_surface_in_fault_block( model.surface( surfaces_uuids[0] ),
        model.fault_block( fault_blocks_uuids[0] ) );
    builder.add_surface_in_fault_block( model.surface( surfaces_uuids[1] ),
        model.fault_block( fault_blocks_uuids[0] ) );
    builder.add_surface_in_fault_block( model.surface( surfaces_uuids[2] ),
        model.fault_block( fault_blocks_uuids[1] ) );
    builder.add_surface_in_fault_block( model.surface( surfaces_uuids[3] ),
        model.fault_block( fault_blocks_uuids[1] ) );

    builder.add_surface_in_stratigraphic_unit(
        model.surface( surfaces_uuids[0] ),
        model.stratigraphic_unit( stratigraphic_units_uuids[0] ) );
    builder.add_surface_in_stratigraphic_unit(
        model.surface( surfaces_uuids[1] ),
        model.stratigraphic_unit( stratigraphic_units_uuids[1] ) );
    builder.add_surface_in_stratigraphic_unit(
        model.surface( surfaces_uuids[2] ),
        model.stratigraphic_unit( stratigraphic_units_uuids[0] ) );
    builder.add_surface_in_stratigraphic_unit(
        model.surface( surfaces_uuids[3] ),
        model.stratigraphic_unit( stratigraphic_units_uuids[1] ) );

    do_checks( model, lines_uuids, surfaces_uuids, faults_uuids, horizons_uuids,
        fault_blocks_uuids, stratigraphic_units_uuids );
}

void check_reloaded_model( const geode::CrossSection& reloaded_model )
{
    OPENGEODE_EXCEPTION( reloaded_model.nb_lines() == 8,
        "[Test] Number of lines in reloaded model should be 8" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_horizons() == 3,
        "[Test] Number of horizons in reloaded model should be 3" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_faults() == 2,
        "[Test] Number of faults in reloaded model should be 2" );
    OPENGEODE_EXCEPTION( reloaded_model.nb_fault_blocks() == 2,
        "[Test] Number of fault blocks in reloaded model should be 2" );
}

void test_io( const geode::CrossSection& model )
{
    const auto file_io = absl::StrCat( "test.", model.native_extension() );
    geode::save_cross_section( model, file_io );

    geode::CrossSection reloaded_model = geode::load_cross_section( file_io );
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
    const auto temp_id = builder.add_fault();
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
        add_fault_blocks( model, builder );
        add_stratigraphic_units( model, builder );
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
