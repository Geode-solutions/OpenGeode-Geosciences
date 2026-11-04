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

#include <geode/geosciences/simulation/helpers/simulation_checker.hpp>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <absl/types/span.h>

#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>
#include <geode/geosciences/simulation/helpers/internal/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/internal/problem_definition_checker.hpp>
#include <geode/geosciences/simulation/helpers/internal/problem_properties_checker.hpp>
#include <geode/geosciences/simulation/helpers/internal/problem_regions_checker.hpp>
#include <geode/geosciences/simulation/helpers/internal/property_attribute_checker.hpp>
#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace
{
    using geode::PHYSICAL_PROPERTY_NAME;
    using geode::internal::component_location;
    using geode::internal::ComponentElements;
    using geode::internal::count_string;
    using geode::internal::for_each_component;
    using geode::internal::material_subject;

    std::vector< bool > block_polyhedron_unique_vertices(
        const geode::BRep& model )
    {
        std::vector< bool > on_block( model.nb_unique_vertices(), false );
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
            {
                for( const auto vertex :
                    geode::LRange{ mesh.nb_polyhedron_vertices( polyhedron ) } )
                {
                    const auto unique_vertex =
                        model.unique_vertex( { block.component_id(),
                            mesh.polyhedron_vertex(
                                { polyhedron, vertex } ) } );
                    if( unique_vertex != geode::NO_ID )
                    {
                        on_block[unique_vertex] = true;
                    }
                }
            }
        }
        return on_block;
    }

    // Block vertices, in increasing order, without unique vertex
    std::vector< geode::index_t > vertices_without_unique_vertex(
        const geode::BRep& model, const geode::Block3D& block )
    {
        const auto& mesh = block.mesh();
        std::vector< bool > is_polyhedron_vertex( mesh.nb_vertices(), false );
        for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto vertex : mesh.polyhedron_vertices( polyhedron ) )
            {
                is_polyhedron_vertex[vertex] = true;
            }
        }
        std::vector< geode::index_t > missing;
        for( const auto vertex : geode::Range{ mesh.nb_vertices() } )
        {
            if( is_polyhedron_vertex[vertex]
                && model.unique_vertex( { block.component_id(), vertex } )
                       == geode::NO_ID )
            {
                missing.push_back( vertex );
            }
        }
        return missing;
    }

    bool is_on_blocks( const geode::BRep& model,
        const geode::Surface3D& surface,
        geode::index_t vertex,
        const std::vector< bool >& on_block )
    {
        const auto unique_vertex =
            model.unique_vertex( { surface.component_id(), vertex } );
        return unique_vertex != geode::NO_ID && on_block[unique_vertex];
    }

    // Surface polygons with a vertex which is not a Block vertex
    std::vector< geode::index_t > polygons_outside_blocks(
        const geode::BRep& model,
        const geode::Surface3D& surface,
        const std::vector< bool >& on_block )
    {
        const auto& mesh = surface.mesh();
        std::vector< geode::index_t > polygons;
        for( const auto polygon : geode::Range{ mesh.nb_polygons() } )
        {
            const auto vertices = mesh.polygon_vertices( polygon );
            if( !absl::c_all_of( vertices, [&]( geode::index_t vertex ) {
                    return is_on_blocks( model, surface, vertex, on_block );
                } ) )
            {
                polygons.push_back( polygon );
            }
        }
        return polygons;
    }

    struct UniqueVerticesOutsideBlocks
    {
        // Unique vertices of no component mesh
        geode::index_t nb_isolated{ 0 };
        // Components in order of their first vertex, and their vertices
        std::vector< geode::uuid > components;
        absl::flat_hash_map< geode::uuid, std::vector< geode::index_t > >
            vertices;
    };

    UniqueVerticesOutsideBlocks unique_vertices_outside_blocks(
        const geode::BRep& model )
    {
        UniqueVerticesOutsideBlocks outside;
        for( const auto unique_vertex :
            geode::Range{ model.nb_unique_vertices() } )
        {
            if( model.has_component_mesh_vertices(
                    unique_vertex, geode::Block3D::component_type_static() ) )
            {
                continue;
            }
            const auto& vertices =
                model.component_mesh_vertices( unique_vertex );
            outside.nb_isolated += vertices.empty() ? 1 : 0;
            for( const auto& vertex : vertices )
            {
                auto [component_vertices, inserted] =
                    outside.vertices.try_emplace( vertex.component_id.id );
                if( inserted )
                {
                    outside.components.push_back( vertex.component_id.id );
                }
                component_vertices->second.push_back( vertex.vertex );
            }
        }
        return outside;
    }

    bool material_has_value(
        const geode::Material& material, PHYSICAL_PROPERTY_NAME property )
    {
        if( property == PHYSICAL_PROPERTY_NAME::porosity )
        {
            return material.porosity.has_value();
        }
        return material.permeability.has_value();
    }

    // Meshed Blocks of each material lacking the coverage property, in the
    // model and in the material
    std::vector< std::vector< const geode::Block3D* > >
        blocks_without_rock_property( const geode::PhysicalProblem& problem,
            const geode::BRep& model,
            const geode::PhysicalPropertyCoverage& coverage )
    {
        const auto materials = problem.materials();
        const auto block_materials =
            geode::internal::physical_problem_block_materials( problem, model );
        std::vector< std::vector< const geode::Block3D* > > uncovered_blocks(
            materials.size() );
        for( const auto& block : model.blocks() )
        {
            if( block.mesh().nb_polyhedra() == 0
                || coverage.covers( block.id() ) )
            {
                continue;
            }
            const auto material = block_materials.find( block.id() );
            if( material != block_materials.end()
                && !material_has_value(
                    materials[material->second], coverage.property ) )
            {
                uncovered_blocks[material->second].push_back( &block );
            }
        }
        return uncovered_blocks;
    }

    // Name between quotes, or the id if the model has no name
    std::string model_name( const geode::BRep& model )
    {
        if( model.name() && !model.name()->empty() )
        {
            return absl::StrCat( "\"", model.name().value(), "\"" );
        }
        return model.id().string();
    }
} // namespace

namespace geode
{
    SimulationChecker::~SimulationChecker() = default;

    SimulationReport SimulationChecker::check(
        const PhysicalProblem& problem, const BRep& model ) const
    {
        SimulationReport report;
        report.set_model_name( model_name( model ) );
        check_physical_problem_definition( problem, report );
        check_physical_problem_regions( problem, model, report );
        check_physical_problem_properties( problem, model, report );
        check_simulator( problem, model, report );
        return report;
    }

    void SimulationChecker::check_physical_problem_definition(
        const PhysicalProblem& problem, SimulationReport& report )
    {
        internal::check_physical_problem_definition( problem, report );
    }

    void SimulationChecker::check_physical_problem_regions(
        const PhysicalProblem& problem,
        const BRep& model,
        SimulationReport& report )
    {
        internal::check_physical_problem_regions( problem, model, report );
    }

    void SimulationChecker::check_physical_problem_properties(
        const PhysicalProblem& problem,
        const BRep& model,
        SimulationReport& report )
    {
        internal::check_physical_problem_properties( problem, model, report );
    }

    void SimulationChecker::check_blocks(
        const BRep& model, SimulationReport& report )
    {
        if( model.nb_blocks() == 0 )
        {
            report.add_error(
                SIMULATION_CHECK_STAGE::mesh, "Model", "has no Block" );
            return;
        }
        std::vector< SimulationIssueLocation > unmeshed;
        std::vector< SimulationIssueLocation > unidentified;
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            if( mesh.nb_polyhedra() == 0 )
            {
                unmeshed.push_back( component_location( block ) );
                continue;
            }
            auto missing = vertices_without_unique_vertex( model, block );
            if( !missing.empty() )
            {
                auto& location =
                    unidentified.emplace_back( component_location( block ) );
                location.element_type = SIMULATION_ELEMENT_TYPE::vertex;
                location.elements = std::move( missing );
                location.nb_elements = mesh.nb_vertices();
            }
        }
        const auto block_type = Block3D::component_type_static();
        if( !unmeshed.empty() )
        {
            SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::mesh, "Blocks",
                absl::StrCat( count_string( unmeshed.size(), block_type.get() ),
                    " without polyhedron" ) };
            issue.locations = std::move( unmeshed );
            issue.fix = "mesh them or remove them from the model";
            report.add_issue( std::move( issue ) );
        }
        if( !unidentified.empty() )
        {
            SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::mesh, "Blocks",
                absl::StrCat( "polyhedron vertices without unique vertex on ",
                    count_string( unidentified.size(), block_type.get() ) ) };
            issue.locations = std::move( unidentified );
            issue.fix = "compute the model unique vertices";
            report.add_issue( std::move( issue ) );
        }
    }

    void SimulationChecker::check_surfaces_on_blocks( const BRep& model,
        absl::Span< const uuid > surfaces,
        SimulationReport& report )
    {
        if( surfaces.empty() )
        {
            return;
        }
        const auto on_block = block_polyhedron_unique_vertices( model );
        std::vector< SimulationIssueLocation > outside;
        for( const auto& surface_id : surfaces )
        {
            const auto& surface = model.surface( surface_id );
            auto polygons = polygons_outside_blocks( model, surface, on_block );
            if( !polygons.empty() )
            {
                auto& location =
                    outside.emplace_back( component_location( surface ) );
                location.element_type = SIMULATION_ELEMENT_TYPE::polygon;
                location.elements = std::move( polygons );
                location.nb_elements = surface.mesh().nb_polygons();
            }
        }
        if( outside.empty() )
        {
            return;
        }
        SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
            SIMULATION_CHECK_STAGE::mesh, "Surfaces",
            absl::StrCat( "polygons with vertices which are not Block "
                          "polyhedron vertices on ",
                count_string( outside.size(),
                    Surface3D::component_type_static().get() ) ) };
        issue.locations = std::move( outside );
        issue.fix = "the Surface meshes must match the Block meshes";
        report.add_issue( std::move( issue ) );
    }

    void SimulationChecker::check_unique_vertices_in_blocks(
        const BRep& model, SimulationReport& report )
    {
        auto outside = unique_vertices_outside_blocks( model );
        if( outside.nb_isolated != 0 )
        {
            SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::mesh, "Unique vertices",
                absl::StrCat( outside.nb_isolated,
                    outside.nb_isolated > 1 ? " unique vertices are"
                                            : " unique vertex is",
                    " not a vertex of any component mesh" ) };
            issue.fix = "remove the isolated unique vertices";
            report.add_issue( std::move( issue ) );
        }
        if( outside.components.empty() )
        {
            return;
        }
        SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
            SIMULATION_CHECK_STAGE::mesh, "Unique vertices",
            absl::StrCat( "vertices of ",
                count_string( outside.components.size(), "component" ),
                " are not Block vertices" ) };
        issue.fix = "every unique vertex, including those of Corners, Lines "
                    "and Surfaces, must be a Block vertex";
        for( const auto& component_id : outside.components )
        {
            auto& location =
                issue.add_location( model.component( component_id ) );
            location.element_type = SIMULATION_ELEMENT_TYPE::vertex;
            location.elements =
                std::move( outside.vertices.at( component_id ) );
            absl::c_sort( location.elements );
        }
        report.add_issue( std::move( issue ) );
    }

    std::string_view physical_property_string( PHYSICAL_PROPERTY_NAME property )
    {
        switch( property )
        {
            case PHYSICAL_PROPERTY_NAME::porosity:
                return "porosity";
            case PHYSICAL_PROPERTY_NAME::permeability:
                return "permeability";
            case PHYSICAL_PROPERTY_NAME::bulk_modulus:
                return "bulk_modulus";
            case PHYSICAL_PROPERTY_NAME::shear_modulus:
                return "shear_modulus";
            case PHYSICAL_PROPERTY_NAME::density:
                return "density";
            case PHYSICAL_PROPERTY_NAME::biot_coefficient:
                return "biot_coefficient";
            case PHYSICAL_PROPERTY_NAME::grain_density:
                return "grain_density";
            case PHYSICAL_PROPERTY_NAME::thermal_expansion_coefficient:
                return "thermal_expansion_coefficient";
            case PHYSICAL_PROPERTY_NAME::initial_stress:
                return "initial_stress";
            case PHYSICAL_PROPERTY_NAME::initial_shear_stress:
                return "initial_shear_stress";
            case PHYSICAL_PROPERTY_NAME::boundary_pressure:
                return "boundary_pressure";
            case PHYSICAL_PROPERTY_NAME::boundary_temperature:
                return "boundary_temperature";
            case PHYSICAL_PROPERTY_NAME::boundary_oil_fraction:
                return "boundary_oil_fraction";
            case PHYSICAL_PROPERTY_NAME::boundary_gas_fraction:
                return "boundary_gas_fraction";
            case PHYSICAL_PROPERTY_NAME::boundary_water_fraction:
                return "boundary_water_fraction";
            case PHYSICAL_PROPERTY_NAME::boundary_displacement:
                return "boundary_displacement";
            case PHYSICAL_PROPERTY_NAME::boundary_traction:
                return "boundary_traction";
            case PHYSICAL_PROPERTY_NAME::boundary_normal_traction:
                return "boundary_normal_traction";
        }
        return "unknown";
    }

    ComponentType physical_property_component_type(
        PHYSICAL_PROPERTY_NAME property )
    {
        switch( property )
        {
            case PHYSICAL_PROPERTY_NAME::boundary_pressure:
            case PHYSICAL_PROPERTY_NAME::boundary_temperature:
            case PHYSICAL_PROPERTY_NAME::boundary_oil_fraction:
            case PHYSICAL_PROPERTY_NAME::boundary_gas_fraction:
            case PHYSICAL_PROPERTY_NAME::boundary_water_fraction:
            case PHYSICAL_PROPERTY_NAME::boundary_displacement:
            case PHYSICAL_PROPERTY_NAME::boundary_traction:
            case PHYSICAL_PROPERTY_NAME::boundary_normal_traction:
                return Surface3D::component_type_static();
            default:
                return Block3D::component_type_static();
        }
    }

    bool is_physical_property_read(
        const PhysicalProblem& problem, PHYSICAL_PROPERTY_NAME property )
    {
        return internal::property_reader( property ).reads( problem );
    }

    PhysicalPropertyRequirement::PhysicalPropertyRequirement(
        PHYSICAL_PROPERTY_NAME property_in )
        : property( property_in )
    {
        switch( property )
        {
            case PHYSICAL_PROPERTY_NAME::permeability:
                value_types = { PHYSICAL_PROPERTY_VALUE_TYPE::scalar,
                    PHYSICAL_PROPERTY_VALUE_TYPE::vector3 };
                return;
            case PHYSICAL_PROPERTY_NAME::initial_stress:
            case PHYSICAL_PROPERTY_NAME::initial_shear_stress:
            case PHYSICAL_PROPERTY_NAME::boundary_displacement:
            case PHYSICAL_PROPERTY_NAME::boundary_traction:
                value_types = { PHYSICAL_PROPERTY_VALUE_TYPE::vector3 };
                return;
            default:
                value_types = { PHYSICAL_PROPERTY_VALUE_TYPE::scalar };
        }
    }

    PhysicalPropertyCoverage::PhysicalPropertyCoverage(
        PHYSICAL_PROPERTY_NAME property_in )
        : property( property_in )
    {
    }

    bool PhysicalPropertyCoverage::carries( const uuid& component ) const
    {
        return components.contains( component );
    }

    bool PhysicalPropertyCoverage::covers( const uuid& component ) const
    {
        const auto carrier = components.find( component );
        return carrier != components.end() && carrier->second == 0;
    }

    bool PhysicalPropertyCoverage::is_carried() const
    {
        return !components.empty();
    }

    PhysicalPropertyCoverage SimulationChecker::check_physical_property(
        const BRep& model,
        const PhysicalPropertyRequirement& requirement,
        SimulationReport& report )
    {
        return internal::check_physical_property( model, requirement, report );
    }

    void SimulationChecker::check_rock_property_availability(
        const PhysicalProblem& problem,
        const BRep& model,
        const PhysicalPropertyCoverage& coverage,
        SimulationReport& report )
    {
        const auto materials = problem.materials();
        const auto uncovered_blocks =
            blocks_without_rock_property( problem, model, coverage );
        const auto name = physical_property_string( coverage.property );
        for( const auto material : Indices{ materials } )
        {
            const auto& blocks = uncovered_blocks[material];
            if( blocks.empty() )
            {
                continue;
            }
            SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::properties,
                material_subject( materials[material].name ),
                absl::StrCat( "no ", name, " on ",
                    count_string(
                        blocks.size(), Block3D::component_type_static().get() ),
                    coverage.is_registered
                        ? absl::StrCat( ": the model ", name,
                            " does not cover every polyhedron" )
                        : absl::StrCat( ": the model has no ", name,
                            " physical property on Blocks" ) ) };
            issue.fix = absl::StrCat( "give the material a ", name,
                " or a value to every polyhedron of the model ", name );
            for( const auto* block : blocks )
            {
                issue.add_location( *block );
            }
            report.add_issue( std::move( issue ) );
        }
    }

    void SimulationChecker::check_coinciding_physical_properties(
        const BRep& model,
        absl::Span< const PhysicalPropertyCoverage > coverages,
        SimulationReport& report )
    {
        if( coverages.size() < 2 )
        {
            return;
        }
        const auto type =
            physical_property_component_type( coverages.front().property );
        for_each_component( model, type,
            [&coverages, &report]( const ComponentElements& elements ) {
                std::vector< std::string_view > carried;
                std::vector< std::string_view > missing;
                for( const auto& coverage : coverages )
                {
                    const auto name =
                        physical_property_string( coverage.property );
                    if( coverage.carries( elements.component.id() ) )
                    {
                        carried.push_back( name );
                    }
                    else
                    {
                        missing.push_back( name );
                    }
                }
                if( carried.empty() || missing.empty() )
                {
                    return;
                }
                SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                    SIMULATION_CHECK_STAGE::properties,
                    absl::StrJoin( carried, ", " ),
                    absl::StrCat(
                        "given without ", absl::StrJoin( missing, ", " ) ) };
                issue.fix = "these properties must be given together";
                issue.add_location( elements.component );
                report.add_issue( std::move( issue ) );
            } );
    }

    void SimulationChecker::check_physical_property_presence( const BRep& model,
        const PhysicalPropertyCoverage& coverage,
        std::string_view consequence,
        SimulationReport& report )
    {
        if( coverage.is_carried() )
        {
            return;
        }
        const auto name = physical_property_string( coverage.property );
        if( !model.has_physical_property( coverage.property ) )
        {
            report.add_warning( SIMULATION_CHECK_STAGE::properties,
                std::string{ name }, "not in the model: ", consequence );
        }
        else if( coverage.is_registered )
        {
            report.add_warning( SIMULATION_CHECK_STAGE::properties,
                std::string{ name }, "no ",
                physical_property_component_type( coverage.property ).get(),
                " carries a value: ", consequence );
        }
    }

    void SimulationChecker::warn_ignored_physical_properties( const BRep& model,
        absl::Span< const PHYSICAL_PROPERTY_NAME > properties,
        SimulationReport& report )
    {
        for( const auto property : properties )
        {
            if( model.has_physical_property( property ) )
            {
                report.add_warning( SIMULATION_CHECK_STAGE::simulator,
                    std::string{ physical_property_string( property ) },
                    "ignored by ", report.simulator() );
            }
        }
    }
} // namespace geode
