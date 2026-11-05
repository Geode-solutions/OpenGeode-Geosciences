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
#include <absl/types/span.h>

#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>
#include <geode/geosciences/simulation/helpers/internal/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace
{
    using geode::PHYSICAL_PROPERTY_NAME;
    using geode::internal::component_location;
    using geode::internal::count_string;
    using geode::internal::material_subject;

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
            const absl::flat_hash_map< geode::uuid, geode::index_t >&
                block_materials,
            const geode::PhysicalPropertyCoverage& coverage )
    {
        const auto materials = problem.materials();
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

    void check_material_rock_property( const geode::PhysicalProblem& problem,
        const geode::BRep& model,
        const absl::flat_hash_map< geode::uuid, geode::index_t >&
            block_materials,
        const geode::PhysicalPropertyCoverage& coverage,
        geode::SimulationReport& report )
    {
        geode::OpenGeodeGeosciencesSimulationException::check_exception(
            coverage.property == PHYSICAL_PROPERTY_NAME::porosity
                || coverage.property == PHYSICAL_PROPERTY_NAME::permeability,
            nullptr, geode::OpenGeodeException::TYPE::internal,
            "[SimulationChecker::check_rock_property_availability] Materials "
            "only give porosity and permeability, not ",
            geode::physical_property_string( coverage.property ) );
        const auto materials = problem.materials();
        const auto uncovered_blocks = blocks_without_rock_property(
            problem, model, block_materials, coverage );
        const auto name = geode::physical_property_string( coverage.property );
        for( const auto material : geode::Indices{ materials } )
        {
            const auto& blocks = uncovered_blocks[material];
            if( blocks.empty() )
            {
                continue;
            }
            geode::SimulationIssue issue{
                geode::SIMULATION_ISSUE_SEVERITY::error,
                geode::SIMULATION_CHECK_STAGE::properties,
                material_subject( materials[material].name ),
                absl::StrCat( "no ", name, " on ",
                    count_string( blocks.size(),
                        geode::Block3D::component_type_static().get() ),
                    coverage.is_registered
                        ? absl::StrCat( ": the model ", name,
                            " does not cover every polyhedron" )
                        : absl::StrCat( ": the model has no ", name,
                            " physical property on Blocks" ) )
            };
            issue.fix = absl::StrCat( "give the material a ", name,
                " or a value to every polyhedron of the model ", name );
            for( const auto* block : blocks )
            {
                issue.add_location( *block );
            }
            report.add_issue( std::move( issue ) );
        }
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
    SimulationChecker::SimulationChecker(
        const PhysicalProblem& problem, const BRep& model )
        : problem_( problem ), model_( model )
    {
    }

    SimulationChecker::~SimulationChecker() = default;

    SimulationReport SimulationChecker::check()
    {
        report_ = SimulationReport{};
        report_.set_model_name( model_name( model() ) );
        check_physical_problem_definition();
        check_physical_problem_regions();
        check_physical_problem_properties();
        check_simulator();
        return std::move( report_ );
    }

    void SimulationChecker::check_blocks()
    {
        if( model().nb_blocks() == 0 )
        {
            report_.add_error(
                SIMULATION_CHECK_STAGE::mesh, "Model", "has no Block" );
            return;
        }
        std::vector< SimulationIssueLocation > unmeshed;
        std::vector< SimulationIssueLocation > unidentified;
        for( const auto& block : model().blocks() )
        {
            const auto& mesh = block.mesh();
            if( mesh.nb_polyhedra() == 0 )
            {
                unmeshed.push_back( component_location( block ) );
                continue;
            }
            auto missing = vertices_without_unique_vertex( model(), block );
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
            report_.add_issue( std::move( issue ) );
        }
        if( !unidentified.empty() )
        {
            SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::mesh, "Blocks",
                absl::StrCat( "polyhedron vertices without unique vertex on ",
                    count_string( unidentified.size(), block_type.get() ) ) };
            issue.locations = std::move( unidentified );
            issue.fix = "compute the model unique vertices";
            report_.add_issue( std::move( issue ) );
        }
    }

    void SimulationChecker::check_unique_vertices_in_blocks()
    {
        auto outside = unique_vertices_outside_blocks( model() );
        if( outside.nb_isolated != 0 )
        {
            SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::mesh, "Unique vertices",
                absl::StrCat( outside.nb_isolated,
                    outside.nb_isolated > 1 ? " unique vertices are"
                                            : " unique vertex is",
                    " not a vertex of any component mesh" ) };
            issue.fix = "remove the isolated unique vertices";
            report_.add_issue( std::move( issue ) );
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
                issue.add_location( model().component( component_id ) );
            location.element_type = SIMULATION_ELEMENT_TYPE::vertex;
            location.elements =
                std::move( outside.vertices.at( component_id ) );
            absl::c_sort( location.elements );
        }
        report_.add_issue( std::move( issue ) );
    }

    std::string_view physical_property_string( PHYSICAL_PROPERTY_NAME property )
    {
        return internal::physical_property_description( property ).name;
    }

    ComponentType physical_property_component_type(
        PHYSICAL_PROPERTY_NAME property )
    {
        if( internal::physical_property_description( property ).element_type
            == SIMULATION_ELEMENT_TYPE::polygon )
        {
            return Surface3D::component_type_static();
        }
        return Block3D::component_type_static();
    }

    bool is_physical_property_read(
        const PhysicalProblem& problem, PHYSICAL_PROPERTY_NAME property )
    {
        return internal::reads(
            internal::physical_property_description( property ).reader,
            problem );
    }

    PhysicalPropertyRequirement::PhysicalPropertyRequirement(
        PHYSICAL_PROPERTY_NAME property_in )
        : property( property_in )
    {
        switch(
            internal::physical_property_description( property ).value_types )
        {
            case internal::PROPERTY_VALUE_TYPES::scalar:
                value_types = { PHYSICAL_PROPERTY_VALUE_TYPE::scalar };
                return;
            case internal::PROPERTY_VALUE_TYPES::vector3:
                value_types = { PHYSICAL_PROPERTY_VALUE_TYPE::vector3 };
                return;
            case internal::PROPERTY_VALUE_TYPES::scalar_or_vector3:
                value_types = { PHYSICAL_PROPERTY_VALUE_TYPE::scalar,
                    PHYSICAL_PROPERTY_VALUE_TYPE::vector3 };
                return;
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

    void SimulationChecker::check_rock_property_availability(
        absl::Span< const PhysicalPropertyCoverage > coverages )
    {
        const auto block_materials =
            internal::physical_problem_block_materials( problem(), model() );
        for( const auto& coverage : coverages )
        {
            check_material_rock_property(
                problem(), model(), block_materials, coverage, report_ );
        }
    }

    void SimulationChecker::check_physical_property_presence(
        const PhysicalPropertyCoverage& coverage, std::string_view consequence )
    {
        if( coverage.is_carried() )
        {
            return;
        }
        const auto name = physical_property_string( coverage.property );
        if( !model().has_physical_property( coverage.property ) )
        {
            report_.add_warning( SIMULATION_CHECK_STAGE::properties,
                std::string{ name }, "not in the model: ", consequence );
        }
        else if( coverage.is_registered )
        {
            report_.add_warning( SIMULATION_CHECK_STAGE::properties,
                std::string{ name }, "no ",
                physical_property_component_type( coverage.property ).get(),
                " carries a value: ", consequence );
        }
    }

    void SimulationChecker::warn_ignored_physical_properties(
        absl::Span< const PHYSICAL_PROPERTY_NAME > properties )
    {
        for( const auto property : properties )
        {
            if( model().has_physical_property( property ) )
            {
                report_.add_warning( SIMULATION_CHECK_STAGE::simulator,
                    std::string{ physical_property_string( property ) },
                    "ignored by ", report_.simulator() );
            }
        }
    }
} // namespace geode
