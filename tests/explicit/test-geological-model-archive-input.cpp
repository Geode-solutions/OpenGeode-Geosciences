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

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/detail/input_files.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/section_input.hpp>

#include <geode/tests_config.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>
#include <geode/geosciences/explicit/representation/builder/cross_section_builder.hpp>
#include <geode/geosciences/explicit/representation/builder/helpers/structural_model_fault_blocks_builder.hpp>
#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/explicit/representation/core/cross_section.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <geode/geosciences/explicit/representation/io/cross_section_input.hpp>
#include <geode/geosciences/explicit/representation/io/cross_section_output.hpp>
#include <geode/geosciences/explicit/representation/io/geode/geode_cross_section_input.hpp>
#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_output.hpp>

namespace
{
    void check( bool condition, std::string_view where, std::string_view what )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            condition, "[", where, "] ", what );
    }

    template < typename Range >
    std::vector< geode::uuid > sorted_ids( const Range& range )
    {
        std::vector< geode::uuid > ids;
        for( const auto& item : range )
        {
            ids.push_back( item.id() );
        }
        absl::c_sort( ids );
        return ids;
    }

    template < typename Model >
    void check_same_geology(
        const Model& model, const Model& reference, std::string_view where )
    {
        check( model.nb_faults() == reference.nb_faults(), where,
            "wrong number of faults" );
        for( const auto& fault : reference.faults() )
        {
            check( model.has_fault( fault.id() ), where, "missing fault" );
            const auto& loaded = model.fault( fault.id() );
            check( loaded.name() == fault.name(), where, "wrong fault name" );
            check( loaded.has_type() == fault.has_type(), where,
                "wrong fault has_type" );
            check( !fault.has_type() || loaded.type() == fault.type(), where,
                "wrong fault type" );
            check( sorted_ids( model.fault_items( loaded ) )
                       == sorted_ids( reference.fault_items( fault ) ),
                where, "wrong fault items" );
        }
        check( model.nb_horizons() == reference.nb_horizons(), where,
            "wrong number of horizons" );
        for( const auto& horizon : reference.horizons() )
        {
            check(
                model.has_horizon( horizon.id() ), where, "missing horizon" );
            const auto& loaded = model.horizon( horizon.id() );
            check(
                loaded.name() == horizon.name(), where, "wrong horizon name" );
            check( loaded.contact_type() == horizon.contact_type(), where,
                "wrong horizon contact type" );
            check( sorted_ids( model.horizon_items( loaded ) )
                       == sorted_ids( reference.horizon_items( horizon ) ),
                where, "wrong horizon items" );
        }
        check( model.nb_fault_blocks() == reference.nb_fault_blocks(), where,
            "wrong number of fault blocks" );
        for( const auto& fault_block : reference.fault_blocks() )
        {
            check( model.has_fault_block( fault_block.id() ), where,
                "missing fault block" );
            const auto& loaded = model.fault_block( fault_block.id() );
            check( loaded.name() == fault_block.name(), where,
                "wrong fault block name" );
            check(
                sorted_ids( model.fault_block_items( loaded ) )
                    == sorted_ids( reference.fault_block_items( fault_block ) ),
                where, "wrong fault block items" );
        }
        check( model.nb_stratigraphic_units()
                   == reference.nb_stratigraphic_units(),
            where, "wrong number of stratigraphic units" );
        for( const auto& unit : reference.stratigraphic_units() )
        {
            check( model.has_stratigraphic_unit( unit.id() ), where,
                "missing stratigraphic unit" );
            const auto& loaded = model.stratigraphic_unit( unit.id() );
            check( loaded.name() == unit.name(), where,
                "wrong stratigraphic unit name" );
            check(
                sorted_ids( model.stratigraphic_unit_items( loaded ) )
                    == sorted_ids( reference.stratigraphic_unit_items( unit ) ),
                where, "wrong stratigraphic unit items" );
        }
    }

    template < typename Model, typename Component >
    void check_same_component( const Model& model,
        const Model& reference,
        const Component& component,
        const Component& reference_component,
        std::string_view where )
    {
        check( component.name() == reference_component.name(), where,
            "wrong component name" );
        const auto& mesh = component.mesh();
        const auto& reference_mesh = reference_component.mesh();
        check( mesh.nb_vertices() == reference_mesh.nb_vertices(), where,
            "wrong number of mesh vertices" );
        for( const auto vertex : geode::Range{ reference_mesh.nb_vertices() } )
        {
            check( mesh.point( vertex ) == reference_mesh.point( vertex ),
                where, "wrong mesh point" );
            check( model.unique_vertex( { component.component_id(), vertex } )
                       == reference.unique_vertex(
                           { reference_component.component_id(), vertex } ),
                where, "wrong unique vertex" );
        }
    }

    template < typename Model >
    void check_same_model_components(
        const Model& model, const Model& reference, std::string_view where )
    {
        check( model.id() == reference.id(), where, "wrong model id" );
        check( model.name() == reference.name(), where, "wrong model name" );
        check( model.nb_unique_vertices() == reference.nb_unique_vertices(),
            where, "wrong number of unique vertices" );
        check( model.nb_corners() == reference.nb_corners(), where,
            "wrong number of corners" );
        for( const auto& corner : reference.corners() )
        {
            check_same_component(
                model, reference, model.corner( corner.id() ), corner, where );
        }
        check( model.nb_lines() == reference.nb_lines(), where,
            "wrong number of lines" );
        for( const auto& line : reference.lines() )
        {
            check_same_component(
                model, reference, model.line( line.id() ), line, where );
            check( model.line( line.id() ).mesh().nb_edges()
                       == line.mesh().nb_edges(),
                where, "wrong number of line edges" );
        }
        check( model.nb_surfaces() == reference.nb_surfaces(), where,
            "wrong number of surfaces" );
        for( const auto& surface : reference.surfaces() )
        {
            check_same_component( model, reference,
                model.surface( surface.id() ), surface, where );
            check( model.surface( surface.id() ).mesh().nb_polygons()
                       == surface.mesh().nb_polygons(),
                where, "wrong number of surface polygons" );
        }
    }

    void check_same_structural_model( const geode::StructuralModel& model,
        const geode::StructuralModel& reference,
        std::string_view where )
    {
        check_same_model_components( model, reference, where );
        check( model.nb_blocks() == reference.nb_blocks(), where,
            "wrong number of blocks" );
        for( const auto& block : reference.blocks() )
        {
            check_same_component(
                model, reference, model.block( block.id() ), block, where );
            check( model.block( block.id() ).mesh().nb_polyhedra()
                       == block.mesh().nb_polyhedra(),
                where, "wrong number of block polyhedra" );
        }
        check_same_geology( model, reference, where );
    }

    void check_same_cross_section( const geode::CrossSection& model,
        const geode::CrossSection& reference,
        std::string_view where )
    {
        check_same_model_components( model, reference, where );
        check_same_geology( model, reference, where );
    }

    void set_environment(
        const char* name, const std::optional< std::string >& value )
    {
#ifdef _WIN32
        _putenv_s( name, value ? value->c_str() : "" );
#else
        if( value )
        {
            setenv( name, value->c_str(), 1 );
        }
        else
        {
            unsetenv( name );
        }
#endif
    }

    /*!
     * Call the function with the temporary directory of the system set to a
     * regular file (any use of it fails), then to an empty directory, which
     * must still be empty afterwards.
     */
    template < typename Function >
    void without_temporary_directory( const Function& function )
    {
        const std::vector< std::string > variables{ "TMPDIR", "TMP", "TEMP" };
        std::vector< std::optional< std::string > > old_values;
        for( const auto& variable : variables )
        {
            const auto* value = std::getenv( variable.c_str() );
            old_values.emplace_back(
                value ? std::optional< std::string >{ value } : std::nullopt );
        }
        const auto not_a_directory =
            std::filesystem::absolute( "geosciences_temporary_file" );
        {
            std::ofstream file{ not_a_directory };
            file << "not a directory";
        }
        const auto empty_directory =
            std::filesystem::absolute( "geosciences_empty_temporary" );
        std::filesystem::remove_all( empty_directory );
        std::filesystem::create_directories( empty_directory );
        for( const auto& temporary : { not_a_directory, empty_directory } )
        {
            for( const auto& variable : variables )
            {
                set_environment( variable.c_str(), temporary.u8string() );
            }
            try
            {
                function();
            }
            catch( ... )
            {
                for( const auto i : geode::Indices{ variables } )
                {
                    set_environment( variables[i].c_str(), old_values[i] );
                }
                throw;
            }
        }
        for( const auto i : geode::Indices{ variables } )
        {
            set_environment( variables[i].c_str(), old_values[i] );
        }
        geode::OpenGeodeGeosciencesExplicitException::test(
            std::filesystem::is_empty( empty_directory ),
            "Loading a model created files in the temporary directory" );
        std::filesystem::remove( not_a_directory );
        std::filesystem::remove_all( empty_directory );
    }

    geode::StructuralModel structural_model_to_save()
    {
        const auto data_file =
            absl::StrCat( geode::DATA_PATH, "modelA2.og_strm" );
        auto model = geode::load_one(
            geode::load_structural_model( data_file ), data_file );
        geode::build_structural_model_fault_blocks( model );
        geode::StructuralModelBuilder builder{ model };
        builder.set_name( "archive round trip" );
        const auto& unit_id = builder.add_stratigraphic_unit();
        const auto& unit = model.stratigraphic_unit( unit_id );
        builder.set_stratigraphic_unit_name( unit, "unit" );
        builder.add_block_in_stratigraphic_unit(
            *model.blocks().begin(), unit );
        geode::index_t count{ 0 };
        for( const auto& fault : model.faults() )
        {
            builder.set_fault_name( fault, absl::StrCat( "fault", count++ ) );
        }
        for( const auto& horizon : model.horizons() )
        {
            builder.set_horizon_contact_type(
                horizon, geode::Horizon3D::CONTACT_TYPE::erosion );
        }
        geode::OpenGeodeGeosciencesExplicitException::test(
            model.nb_faults() > 0 && model.nb_horizons() > 0
                && model.nb_fault_blocks() > 0
                && model.nb_stratigraphic_units() > 0,
            "The model to save should have every geological component" );
        return model;
    }

    void test_structural_model()
    {
        const auto model = structural_model_to_save();
        const auto filename = "archive_round_trip.og_strm";
        geode::save_structural_model( model, filename );
        {
            const geode::UnzipFile unzip{ filename, geode::uuid{}.string() };
            unzip.extract_all();
            geode::StructuralModel from_directory{
                geode::BITSERY::constructor
            };
            geode::detail::load_structural_model_files( from_directory,
                geode::detail::DirectoryInputFiles{ unzip.directory() } );
            check_same_structural_model(
                from_directory, model, "StructuralModel from directory" );
        }
        without_temporary_directory( [&model, &filename] {
            const auto loaded = geode::load_one(
                geode::load_structural_model( filename ), filename );
            check_same_structural_model(
                loaded, model, "StructuralModel from archive" );
        } );
    }

    void test_cross_section()
    {
        const auto data_file =
            absl::StrCat( geode::DATA_PATH, "test_section.og_sctn" );
        geode::CrossSection model{ geode::load_one(
            geode::load_section( data_file ), data_file ) };
        geode::CrossSectionBuilder builder{ model };
        const auto& fault_id =
            builder.add_fault( geode::Fault2D::FAULT_TYPE::normal );
        const auto& fault = model.fault( fault_id );
        builder.set_fault_name( fault, "fault" );
        auto line = model.lines().begin();
        builder.add_line_in_fault( *line, fault );
        const auto& horizon_id = builder.add_horizon();
        ++line;
        builder.add_line_in_horizon( *line, model.horizon( horizon_id ) );
        const auto& fault_block_id = builder.add_fault_block();
        builder.add_surface_in_fault_block(
            *model.surfaces().begin(), model.fault_block( fault_block_id ) );
        const auto& unit_id = builder.add_stratigraphic_unit();
        builder.add_surface_in_stratigraphic_unit(
            *model.surfaces().begin(), model.stratigraphic_unit( unit_id ) );
        const auto filename = "archive_round_trip.og_xsctn";
        geode::save_cross_section( model, filename );
        {
            const geode::UnzipFile unzip{ filename, geode::uuid{}.string() };
            unzip.extract_all();
            geode::CrossSection from_directory{ geode::BITSERY::constructor };
            geode::detail::load_cross_section_files( from_directory,
                geode::detail::DirectoryInputFiles{ unzip.directory() } );
            check_same_cross_section(
                from_directory, model, "CrossSection from directory" );
        }
        without_temporary_directory( [&model, &filename] {
            const auto loaded = geode::load_one(
                geode::load_cross_section( filename ), filename );
            check_same_cross_section(
                loaded, model, "CrossSection from archive" );
        } );
    }
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesExplicitLibrary::initialize();
        test_structural_model();
        test_cross_section();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
