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

#include <filesystem>
#include <fstream>
#include <iterator>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/logger_client.hpp>
#include <geode/basic/logger_manager.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/explicit/representation/core/detail/clone.hpp>
#include <geode/geosciences/explicit/representation/core/detail/helpers.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_output.hpp>

#include <geode/tests_config.hpp>

namespace
{
    geode::index_t nb_warnings{ 0 };

    class WarningCounter : public geode::LoggerClient
    {
    public:
        void trace( const std::string& /*unused*/ ) override {}
        void debug( const std::string& /*unused*/ ) override {}
        void info( const std::string& /*unused*/ ) override {}
        void warning( const std::string& /*unused*/ ) override
        {
            nb_warnings++;
        }
        void error( const std::string& /*unused*/ ) override {}
        void critical( const std::string& /*unused*/ ) override {}
    };
} // namespace

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
    geode::OpenGeodeGeosciencesExplicitException::test(
        !model.fault( fault0 ).has_type(),
        "Addition of a Fault in StructuralModel is not correct" );

    const auto& fault1 =
        builder.add_fault( geode::Fault3D::FAULT_TYPE::reverse );
    builder.set_fault_name( model.fault( fault1 ), "fault1" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.fault( fault1 ).type() == geode::Fault3D::FAULT_TYPE::reverse,
        "Addition of a Fault in StructuralModel is not correct (wrong "
        "type)" );
    geode::OpenGeodeGeosciencesExplicitException::test( model.nb_faults() == 2,
        "Number of faults in StructuralModel should be 2" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.fault( fault1 ).name() == "fault1", "Wrong Fault name" );

    for( auto& fault : model.faults() )
    {
        if( const auto name = fault.name() )
        {
            builder.set_fault_name(
                fault, absl::StrCat( "new_", name.value() ) );
        }
        geode::OpenGeodeGeosciencesExplicitException::test(
            fault.component_type().get() == "Fault",
            "Wrong Fault component type" );
    }
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.fault( fault1 ).name() == "new_fault1",
        "Wrong modified Fault name" );
}

void add_horizons(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    const auto& horizon0 = builder.add_horizon();
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.horizon( horizon0 ).contact_type()
            == geode::Horizon3D::CONTACT_TYPE::conformal,
        "Addition of a Horizon in StructuralModel is not correct" );

    const auto& horizon1 =
        builder.add_horizon( geode::Horizon3D::CONTACT_TYPE::conformal );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.horizon( horizon1 ).contact_type()
            == geode::Horizon3D::CONTACT_TYPE::conformal,
        "Addition of a Horizon in StructuralModel is not correct (wrong "
        "type)" );

    const auto& horizon2 =
        builder.add_horizon( geode::Horizon3D::CONTACT_TYPE::erosion );
    builder.set_horizon_name( model.horizon( horizon2 ), "horizon2" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.horizon( horizon2 ).contact_type()
            == geode::Horizon3D::CONTACT_TYPE::erosion,
        "Addition of a Horizon in StructuralModel is not correct (wrong "
        "type)" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_horizons() == 3,
        "Number of horizons in StructuralModel should be 3" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.horizon( horizon2 ).name() == "horizon2", "Wrong Horizon name" );

    for( auto& horizon : model.horizons() )
    {
        if( const auto name = horizon.name() )
        {
            builder.set_horizon_name(
                horizon, absl::StrCat( "new_", name.value() ) );
        }
        geode::OpenGeodeGeosciencesExplicitException::test(
            horizon.component_type().get() == "Horizon",
            "Wrong Horizon component type" );
    }
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.horizon( horizon2 ).name() == "new_horizon2",
        "Wrong modified Horizon name" );
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
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_items( faults_uuids[0] ) == 3,
        "Number of items in fault_uuids[0] should be 3" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_items( faults_uuids[1] ) == 2,
        "Number of items in fault_uuids[1] should be 2" );

    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_items( horizons_uuids[0] ) == 1,
        "Number of items in horizons_uuids[0] should be 1" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_items( horizons_uuids[1] ) == 0,
        "Number of items in horizons_uuids[1] should be 0" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_items( horizons_uuids[2] ) == 3,
        "Number of items in horizons_uuids[2] should be 3" );

    for( const auto i : geode::Range{ 8 } )
    {
        if( i == 2 )
        {
            continue;
        }
        geode::OpenGeodeGeosciencesExplicitException::test(
            model.nb_collections( surface_uuids[i] ) == 1,
            "Number of collections in which surfaces_uuids[", i,
            "] is should be 1" );
    }

    geode::OpenGeodeGeosciencesExplicitException::test(
        count_items( model.horizon_items( model.horizon( horizons_uuids[2] ) ) )
            == 3,
        "Number of iterations on items in "
        "horizons_uuids[2] should be 3" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        count_items( model.fault_items( model.fault( faults_uuids[1] ) ) ) == 2,
        "Number of iterations on items in "
        "faults_uuids[1] should be 2" );
}

void build_relations_between_geometry_and_geology(
    geode::StructuralModel& model, geode::StructuralModelBuilder& builder )
{
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_lines() == 8, "Wrong number of lines" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_surfaces() == 8, "Wrong number of surfaces" );
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
    geode::OpenGeodeGeosciencesExplicitException::test(
        reloaded_model.nb_surfaces() == 8,
        "Number of surfaces in reloaded model should be 8" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        reloaded_model.nb_horizons() == 3,
        "Number of horizons in reloaded model should be 3" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        reloaded_model.nb_faults() == 2,
        "Number of faults in reloaded model should be 2" );
}

void check_physical_properties( const geode::StructuralModel& model,
    const geode::StructuralModel& reference,
    std::string_view context )
{
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.has_physical_property( geode::PHYSICAL_PROPERTY_NAME::porosity ),
        context, " Porosity physical property should exist" );
    const auto& info =
        model.physical_property_info( geode::PHYSICAL_PROPERTY_NAME::porosity );
    const auto& reference_info = reference.physical_property_info(
        geode::PHYSICAL_PROPERTY_NAME::porosity );
    geode::OpenGeodeGeosciencesExplicitException::test(
        info.component_type == reference_info.component_type
            && info.attribute_id == reference_info.attribute_id,
        context, " Wrong porosity physical property info" );
}

void test_io( const geode::StructuralModel& model )
{
    const auto file_io = absl::StrCat( "test.", model.native_extension() );
    geode::save_structural_model( model, file_io );

    geode::StructuralModel reloaded_model =
        geode::load_structural_model( file_io );
    check_reloaded_model( reloaded_model );
    check_physical_properties( reloaded_model, model, "[Test] reload" );
}

void test_copy( const geode::StructuralModel& model )
{
    geode::StructuralModel copy;
    geode::StructuralModelBuilder copier( copy );
    const auto mapping = copier.copy( model );
    check_physical_properties( copy, model, "[Test] copy" );
    check_physical_properties( model.clone(), model, "[Test] clone" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        copy.nb_surfaces() == model.nb_surfaces(),
        "Number of surfaces in copied model should be 8" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        copy.nb_horizons() == model.nb_horizons(),
        "Number of horizons in copied model should be 3" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        copy.nb_faults() == model.nb_faults(),
        "Number of faults in copied model should be 2" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        copy.nb_fault_blocks() == model.nb_fault_blocks(),
        "Number of fault blocks in copied model should be 0" );
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
            geode::OpenGeodeGeosciencesExplicitException::test(
                copy.is_boundary( boundary_copy, surface_copy ), "Copied line ",
                boundary_copy.component_id().string(),
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
            geode::OpenGeodeGeosciencesExplicitException::test(
                copy.is_item( item_copy.id(), fault_copy.id() ), "Copied line ",
                item_copy.component_id().string(),
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
            geode::OpenGeodeGeosciencesExplicitException::test(
                copy.is_item( item_copy.id(), horizon_copy.id() ),
                "Copied line ", item_copy.component_id().string(),
                " should be item of copied horizon ",
                horizon_copy.component_id().string() );
        }
    }
}

void test_transfer_with_stale_mapping( const geode::StructuralModel& model )
{
    geode::ModelGenericMapping mappings;
    auto& surfaces_mapping =
        mappings[geode::Surface3D::component_type_static()];
    for( const auto& surface : model.surfaces() )
    {
        surfaces_mapping.map( surface.id(), geode::uuid{} );
    }
    bool thrown{ false };
    try
    {
        geode::StructuralModel transferred{ model, geode::BRep{}, mappings };
    }
    catch( const std::exception& /*unused*/ )
    {
        thrown = true;
    }
    geode::OpenGeodeGeosciencesExplicitException::test( thrown,
        "Transfer with a stale mapping should throw a catchable exception" );
}

void test_item_ranges_with_mistyped_items()
{
    geode::StructuralModel model;
    geode::StructuralModelBuilder builder{ model };
    const auto& surface = model.surface( builder.add_surface() );
    const auto& block = model.block( builder.add_block() );
    const auto& line = model.line( builder.add_line() );
    const auto& horizon = model.horizon( builder.add_horizon() );
    const auto& fault = model.fault( builder.add_fault() );
    const auto& fault_block = model.fault_block( builder.add_fault_block() );
    const auto& unit =
        model.stratigraphic_unit( builder.add_stratigraphic_unit() );
    builder.add_item_in_collection(
        line.component_id(), horizon.component_id() );
    builder.add_surface_in_horizon( surface, horizon );
    builder.add_item_in_collection( line.component_id(), fault.component_id() );
    builder.add_surface_in_fault( surface, fault );
    builder.add_item_in_collection(
        surface.component_id(), fault_block.component_id() );
    builder.add_block_in_fault_block( block, fault_block );
    builder.add_item_in_collection( line.component_id(), unit.component_id() );
    builder.add_block_in_stratigraphic_unit( block, unit );
    builder.add_item_in_collection(
        surface.component_id(), unit.component_id() );

    geode::OpenGeodeGeosciencesExplicitException::test(
        count_items( model.horizon_items( horizon ) ) == 1,
        "Horizon item range should skip items which are not Surfaces" );
    for( const auto& item : model.horizon_items( horizon ) )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            item.id() == surface.id(), "Wrong Horizon item" );
    }
    geode::OpenGeodeGeosciencesExplicitException::test(
        count_items( model.fault_items( fault ) ) == 1,
        "Fault item range should skip items which are not Surfaces" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        count_items( model.fault_block_items( fault_block ) ) == 1,
        "FaultBlock item range should skip items which are not Blocks" );
    for( const auto& item : model.fault_block_items( fault_block ) )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            item.id() == block.id(), "Wrong FaultBlock item" );
    }
    geode::OpenGeodeGeosciencesExplicitException::test(
        count_items( model.stratigraphic_unit_items( unit ) ) == 1,
        "StratigraphicUnit item range should skip items which are not "
        "Blocks" );
}

void test_transfer_with_many_to_one_mapping(
    const geode::StructuralModel& model )
{
    geode::BRep new_model;
    geode::BRepBuilder new_builder{ new_model };
    const auto merged_id = new_builder.add_surface();
    geode::ModelGenericMapping mappings;
    auto& surfaces_mapping =
        mappings[geode::Surface3D::component_type_static()];
    for( const auto& surface : model.surfaces() )
    {
        surfaces_mapping.map( surface.id(), merged_id );
    }
    const auto nb_warnings_before = nb_warnings;
    geode::StructuralModel transferred{ model, std::move( new_model ),
        mappings };
    geode::OpenGeodeGeosciencesExplicitException::test(
        nb_warnings == nb_warnings_before,
        "Transfer with a many-to-one mapping should not emit warnings" );
    for( const auto& horizon : model.horizons() )
    {
        const auto& new_horizon = transferred.horizon( horizon.id() );
        const auto expected = model.nb_items( horizon.id() ) == 0 ? 0 : 1;
        geode::OpenGeodeGeosciencesExplicitException::test(
            transferred.nb_items( new_horizon.id() ) == expected,
            "Wrong number of items in transferred Horizon" );
    }
    for( const auto& fault : model.faults() )
    {
        const auto& new_fault = transferred.fault( fault.id() );
        const auto expected = model.nb_items( fault.id() ) == 0 ? 0 : 1;
        geode::OpenGeodeGeosciencesExplicitException::test(
            transferred.nb_items( new_fault.id() ) == expected,
            "Wrong number of items in transferred Fault" );
        if( expected == 1 )
        {
            geode::OpenGeodeGeosciencesExplicitException::test(
                transferred.is_item( merged_id, new_fault.id() ),
                "Merged surface should be in transferred Fault" );
        }
    }
}

std::vector< char > read_bytes( const std::string& filename )
{
    std::ifstream file{ filename, std::ifstream::binary };
    return { std::istreambuf_iterator< char >{ file },
        std::istreambuf_iterator< char >{} };
}

void write_bytes(
    const std::string& filename, const std::vector< char >& bytes )
{
    std::ofstream file{ filename, std::ofstream::binary };
    file.write( bytes.data(), static_cast< std::streamsize >( bytes.size() ) );
}

size_t single_difference(
    const std::vector< char >& lhs, const std::vector< char >& rhs )
{
    geode::OpenGeodeGeosciencesExplicitException::test(
        lhs.size() == rhs.size(), "Files should have the same size" );
    std::vector< size_t > differences;
    for( const auto i : geode::Range{ lhs.size() } )
    {
        if( lhs[i] != rhs[i] )
        {
            differences.push_back( i );
        }
    }
    geode::OpenGeodeGeosciencesExplicitException::test(
        differences.size() == 1, "Files should differ by a single byte" );
    return differences.front();
}

geode::StructuralModel load_geology( const std::string& directory )
{
    geode::StructuralModel model;
    geode::StructuralModelBuilder builder{ model };
    builder.load_faults( directory );
    builder.load_horizons( directory );
    return model;
}

void test_load_invalid_geological_types()
{
    const std::string root{ "invalid_geological_types" };
    for( const auto& directory :
        { "/ref", "/modified", "/corrupted", "/legacy", "/legacy_corrupted" } )
    {
        std::filesystem::create_directories( absl::StrCat( root, directory ) );
    }
    geode::StructuralModel model;
    geode::StructuralModelBuilder builder{ model };
    const auto& fault =
        model.fault( builder.add_fault( geode::Fault3D::FAULT_TYPE::normal ) );
    const auto& horizon = model.horizon(
        builder.add_horizon( geode::Horizon3D::CONTACT_TYPE::conformal ) );
    model.save_faults( absl::StrCat( root, "/ref" ) );
    model.save_horizons( absl::StrCat( root, "/ref" ) );
    builder.set_fault_type( fault, geode::Fault3D::FAULT_TYPE::reverse );
    builder.set_horizon_contact_type(
        horizon, geode::Horizon3D::CONTACT_TYPE::erosion );
    model.save_faults( absl::StrCat( root, "/modified" ) );
    model.save_horizons( absl::StrCat( root, "/modified" ) );

    const auto faults = read_bytes( absl::StrCat( root, "/ref/faults" ) );
    const auto horizons = read_bytes( absl::StrCat( root, "/ref/horizons" ) );
    const auto fault_type_byte = single_difference(
        faults, read_bytes( absl::StrCat( root, "/modified/faults" ) ) );
    const auto contact_type_byte = single_difference(
        horizons, read_bytes( absl::StrCat( root, "/modified/horizons" ) ) );
    geode::OpenGeodeGeosciencesExplicitException::test(
        horizons[contact_type_byte - 1] == 2,
        "Horizon contact type should be serialized in version 2" );

    auto corrupted_faults = faults;
    corrupted_faults[fault_type_byte] = 0x7F;
    write_bytes( absl::StrCat( root, "/corrupted/faults" ), corrupted_faults );
    auto corrupted_horizons = horizons;
    corrupted_horizons[contact_type_byte] = 0x7F;
    write_bytes(
        absl::StrCat( root, "/corrupted/horizons" ), corrupted_horizons );
    auto nb_warnings_before = nb_warnings;
    const auto corrupted = load_geology( absl::StrCat( root, "/corrupted" ) );
    geode::OpenGeodeGeosciencesExplicitException::test(
        corrupted.fault( fault.id() ).type()
            == geode::Fault3D::FAULT_TYPE::no_type,
        "Invalid loaded Fault type should be repaired" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        corrupted.horizon( horizon.id() ).contact_type()
            == geode::Horizon3D::CONTACT_TYPE::conformal,
        "Invalid loaded Horizon contact type should be repaired" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        nb_warnings == nb_warnings_before + 2,
        "Invalid loaded Fault type and Horizon contact type should be "
        "reported" );

    write_bytes( absl::StrCat( root, "/legacy/faults" ), faults );
    write_bytes( absl::StrCat( root, "/legacy_corrupted/faults" ), faults );
    auto legacy_horizons = horizons;
    legacy_horizons[contact_type_byte - 1] = 1;
    legacy_horizons[contact_type_byte] = 2; // legacy non_conformal
    write_bytes( absl::StrCat( root, "/legacy/horizons" ), legacy_horizons );
    nb_warnings_before = nb_warnings;
    const auto legacy = load_geology( absl::StrCat( root, "/legacy" ) );
    geode::OpenGeodeGeosciencesExplicitException::test(
        legacy.horizon( horizon.id() ).contact_type()
            == geode::Horizon3D::CONTACT_TYPE::erosion,
        "Legacy non conformal Horizon should be loaded as erosion" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        nb_warnings == nb_warnings_before,
        "Valid legacy Horizon should be loaded without warning" );

    legacy_horizons[contact_type_byte] = 0x7F;
    write_bytes(
        absl::StrCat( root, "/legacy_corrupted/horizons" ), legacy_horizons );
    nb_warnings_before = nb_warnings;
    const auto legacy_corrupted =
        load_geology( absl::StrCat( root, "/legacy_corrupted" ) );
    geode::OpenGeodeGeosciencesExplicitException::test(
        legacy_corrupted.horizon( horizon.id() ).contact_type()
            == geode::Horizon3D::CONTACT_TYPE::conformal,
        "Invalid legacy Horizon contact type should be repaired" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        nb_warnings == nb_warnings_before + 1,
        "Invalid legacy Horizon contact type should be reported" );
}

void test_geology_clone_mapping_with_existing_mappings(
    const geode::StructuralModel& model )
{
    geode::ModelCopyMapping mappings;
    const auto& first_fault = *model.faults().begin();
    const geode::uuid custom_fault_id;
    mappings[geode::Fault3D::component_type_static()].map(
        first_fault.id(), custom_fault_id );
    mappings[geode::FaultBlock3D::component_type_static()];
    mappings[geode::Horizon3D::component_type_static()];
    mappings[geode::StratigraphicUnit3D::component_type_static()];
    geode::detail::add_geology_clone_mapping( mappings, model );
    const auto& faults_mapping =
        mappings.at( geode::Fault3D::component_type_static() );
    geode::OpenGeodeGeosciencesExplicitException::test(
        faults_mapping.in2out( first_fault.id() ) == custom_fault_id,
        "Existing Fault clone mapping should be kept" );
    for( const auto& fault : model.faults() )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            faults_mapping.has_mapping_input( fault.id() ),
            "All Faults should be in the clone mapping" );
    }
    for( const auto& horizon : model.horizons() )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            mappings.at( geode::Horizon3D::component_type_static() )
                    .in2out( horizon.id() )
                == horizon.id(),
            "All Horizons should be in the clone mapping" );
    }
}

void test_geological_components_mappings_with_existing_mappings(
    const geode::StructuralModel& model )
{
    geode::ModelCopyMapping geological_mappings;
    geode::detail::add_geology_clone_mapping( geological_mappings, model );
    geode::ModelGenericMapping component_mappings;
    const auto& first_horizon = *model.horizons().begin();
    const geode::uuid other_horizon_id;
    component_mappings[geode::Horizon3D::component_type_static()].map(
        first_horizon.id(), other_horizon_id );
    geode::detail::add_geological_components_mappings< 3 >(
        geological_mappings, component_mappings );
    const auto& horizons_mapping =
        component_mappings.at( geode::Horizon3D::component_type_static() );
    geode::OpenGeodeGeosciencesExplicitException::test(
        horizons_mapping.in2out( first_horizon.id() ).size() == 2,
        "Existing Horizon component mapping should be merged" );
    for( const auto& horizon : model.horizons() )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            absl::c_contains(
                horizons_mapping.in2out( horizon.id() ), horizon.id() ),
            "All Horizons should be in the component mapping" );
    }
    for( const auto& fault : model.faults() )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            component_mappings.at( geode::Fault3D::component_type_static() )
                    .in2out( fault.id() )
                    .size()
                == 1,
            "All Faults should be in the component mapping" );
    }
}

void test_geological_components_activity()
{
    geode::StructuralModel model;
    geode::StructuralModelBuilder builder{ model };
    const auto& fault = model.fault( builder.add_fault() );
    const auto& horizon = model.horizon( builder.add_horizon() );
    const auto& fault_block = model.fault_block( builder.add_fault_block() );
    const auto& unit =
        model.stratigraphic_unit( builder.add_stratigraphic_unit() );
    builder.set_fault_active( fault, false );
    builder.set_horizon_active( horizon, false );
    builder.set_fault_block_active( fault_block, false );
    builder.set_stratigraphic_unit_active( unit, false );
    geode::OpenGeodeGeosciencesExplicitException::test(
        !fault.is_active() && !horizon.is_active() && !fault_block.is_active()
            && !unit.is_active(),
        "Geological components should be inactive" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_active_faults() == 0 && model.nb_active_horizons() == 0
            && model.nb_active_fault_blocks() == 0
            && model.nb_active_stratigraphic_units() == 0,
        "Inactive geological components should not be in active ranges" );
    geode::StructuralModel copy;
    geode::StructuralModelBuilder copier{ copy };
    const auto mappings = copier.copy( model );
    geode::OpenGeodeGeosciencesExplicitException::test(
        copy.nb_active_faults() == 0 && copy.nb_active_horizons() == 0
            && copy.nb_active_fault_blocks() == 0
            && copy.nb_active_stratigraphic_units() == 0,
        "Copied geological components should keep their activity" );
    const auto clone = model.clone();
    geode::OpenGeodeGeosciencesExplicitException::test(
        !clone.fault( fault.id() ).is_active()
            && !clone.horizon( horizon.id() ).is_active()
            && !clone.fault_block( fault_block.id() ).is_active()
            && !clone.stratigraphic_unit( unit.id() ).is_active(),
        "Cloned geological components should keep their activity" );
    builder.set_fault_active( fault, true );
    geode::OpenGeodeGeosciencesExplicitException::test(
        fault.is_active(), "Fault should be active again" );
}

void test_copy_in_non_empty_model( const geode::StructuralModel& model )
{
    geode::StructuralModel target;
    geode::StructuralModelBuilder target_builder{ target };
    const auto& uid = target_builder.add_fault();
    geode_unused( uid );
    bool thrown{ false };
    try
    {
        const auto mappings = target_builder.copy( model );
    }
    catch( const geode::OpenGeodeException& /*unused*/ )
    {
        thrown = true;
    }
    geode::OpenGeodeGeosciencesExplicitException::test(
        thrown, "Copy in a non-empty StructuralModel should throw" );
    geode::OpenGeodeGeosciencesExplicitException::test( target.nb_faults() == 1,
        "Non-empty StructuralModel should not be modified" );
}

void test_load_without_fault_blocks_and_stratigraphic_units()
{
    nb_warnings = 0;
    const auto model = geode::load_structural_model( absl::StrCat(
        geode::DATA_PATH,
        "structural_model_without_fault_blocks_and_units.og_strm" ) );
    geode::OpenGeodeGeosciencesExplicitException::test( nb_warnings == 0,
        "Loading a StructuralModel saved before FaultBlocks and "
        "StratigraphicUnits existed should not warn" );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_blocks() != 0 && model.nb_fault_blocks() == 0
            && model.nb_stratigraphic_units() == 0,
        "Loaded legacy StructuralModel has wrong geological components" );
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
                fault, geode::Fault3D::FAULT_TYPE::strike_slip );
        }
        geode::OpenGeodeGeosciencesExplicitException::test(
            fault.has_type(), "All faults should have a type" );
    }
    for( const auto& horizon : model.horizons() )
    {
        if( horizon.contact_type()
            == geode::Horizon3D::CONTACT_TYPE::conformal )
        {
            builder.set_horizon_contact_type(
                horizon, geode::Horizon3D::CONTACT_TYPE::erosion );
        }
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
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_horizons() == 0,
        "Number of horizons in modified model should be 0" );
    geode::OpenGeodeGeosciencesExplicitException::test( model.nb_faults() == 2,
        "Number of faults in modified model should be 2" );
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
        geode::OpenGeodeGeosciencesExplicitLibrary::initialize();
        geode::LoggerManager::register_client(
            std::make_unique< WarningCounter >() );

        geode::StructuralModel model{ build_brep() };
        geode::StructuralModelBuilder builder( model );
        add_faults( model, builder );
        add_horizons( model, builder );
        build_relations_between_geometry_and_geology( model, builder );
        const geode::uuid porosity_attribute_id;
        geode::AttributeValues< double > porosity_values;
        porosity_values.default_value = 0.2;
        porosity_values.no_value = -1.;
        builder.create_blocks_attribute< geode::VariableAttribute, double >(
            "porosity", porosity_attribute_id, porosity_values, {} );
        builder.set_physical_property( geode::PHYSICAL_PROPERTY_NAME::porosity,
            geode::Block3D::component_type_static(), porosity_attribute_id );

        test_io( model );
        test_copy( model );
        test_copy_in_non_empty_model( model );
        test_transfer_with_stale_mapping( model );
        test_transfer_with_many_to_one_mapping( model );
        test_item_ranges_with_mistyped_items();
        test_load_invalid_geological_types();
        test_geology_clone_mapping_with_existing_mappings( model );
        test_geological_components_mappings_with_existing_mappings( model );
        test_geological_components_activity();
        test_load_without_fault_blocks_and_stratigraphic_units();
        modify_model( model, builder );

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
