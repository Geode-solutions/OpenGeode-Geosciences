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

#include <array>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

#include <absl/strings/match.h>
#include <absl/strings/str_join.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/builder/physical_problem_builder.hpp>
#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/helpers/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/physical_problem_simulation.hpp>
#include <geode/geosciences/simulation/helpers/simulation_model_checks.hpp>

namespace
{
    using Exception = geode::OpenGeodeGeosciencesSimulationException;
    using geode::PHYSICAL_PROPERTY_NAME;

    class DummyChecker final : public geode::SimulationChecker
    {
    public:
        [[nodiscard]] geode::SimulationReport check(
            const geode::PhysicalProblem& problem,
            const geode::BRep& model ) const final
        {
            geode::SimulationReport report;
            geode::check_physical_problem_consistency( problem, model, report );
            return report;
        }
    };

    class ThrowingChecker final : public geode::SimulationChecker
    {
    public:
        [[nodiscard]] geode::SimulationReport check(
            const geode::PhysicalProblem& /*unused*/,
            const geode::BRep& /*unused*/ ) const final
        {
            throw std::runtime_error{ "broken checker" };
        }
    };

    bool contains(
        absl::Span< const std::string > issues, std::string_view keyword )
    {
        return std::any_of(
            issues.begin(), issues.end(), [keyword]( const auto& issue ) {
                return absl::StrContains( issue, keyword );
            } );
    }

    void check_error( const geode::SimulationReport& report,
        std::string_view keyword,
        std::string_view context )
    {
        Exception::test( contains( report.errors, keyword ), "[Test] ", context,
            ": no error containing \"", keyword, "\" in: ", report.string() );
    }

    void check_warning( const geode::SimulationReport& report,
        std::string_view keyword,
        std::string_view context )
    {
        Exception::test( contains( report.warnings, keyword ), "[Test] ",
            context, ": no warning containing \"", keyword,
            "\" in: ", report.string() );
    }

    void check_no_error(
        const geode::SimulationReport& report, std::string_view context )
    {
        Exception::test( report.is_simulable(), "[Test] ", context,
            ": unexpected errors: ", report.string() );
    }

    // One tetrahedron Block and one triangle Surface on its first facet
    struct TetModel
    {
        TetModel()
        {
            geode::BRepBuilder builder{ model };
            block_id = builder.add_block( geode::MeshFactory::default_impl(
                geode::TetrahedralSolid3D::type_name_static() ) );
            const auto& block = model.block( block_id );
            auto block_builder =
                builder.block_mesh_builder< geode::TetrahedralSolid3D >(
                    block );
            block_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
            block_builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
            block_builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
            block_builder->create_point( geode::Point3D{ { 0, 0, 1 } } );
            block_builder->create_tetrahedron( { 0, 1, 2, 3 } );
            block_builder->compute_polyhedron_adjacencies();
            surface_id = builder.add_surface( geode::MeshFactory::default_impl(
                geode::TriangulatedSurface3D::type_name_static() ) );
            const auto& surface = model.surface( surface_id );
            auto surface_builder =
                builder.surface_mesh_builder< geode::TriangulatedSurface3D >(
                    surface );
            surface_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
            surface_builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
            surface_builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
            surface_builder->create_triangle( { 0, 1, 2 } );
            builder.add_surface_block_boundary_relationship( surface, block );
            static_cast< void >( builder.create_unique_vertices( 4 ) );
            for( const auto vertex : geode::Range{ 4 } )
            {
                builder.set_unique_vertex(
                    { block.component_id(), vertex }, vertex );
            }
            for( const auto vertex : geode::Range{ 3 } )
            {
                builder.set_unique_vertex(
                    { surface.component_id(), vertex }, vertex );
            }
        }

        geode::BRep model;
        geode::uuid block_id;
        geode::uuid surface_id;
    };

    geode::PhysicalProblem water_problem()
    {
        geode::PhysicalProblem problem;
        geode::PhysicalProblemBuilder builder{ problem };
        auto fluid = std::make_shared< geode::SinglePhaseFluid >(
            std::vector< geode::PHASE >{ geode::PHASE::water },
            std::vector< geode::FluidComponent >{ { "water", 0.018 } }, 1000.,
            1e-3, 1e-9, 0., 0. );
        auto flow = std::make_shared< geode::SinglePhaseFlow >();
        flow->set_fluid( fluid );
        builder.set_flow( flow );
        geode::Material material;
        material.name = "rock";
        material.porosity = 0.2;
        material.permeability = std::array< double, 3 >{ 1e-13, 1e-13, 1e-13 };
        builder.add_material( material );
        builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{}, 1e7 ) );
        builder.set_schedule( { 0., 10., 1., std::nullopt } );
        return problem;
    }

    void test_report_string()
    {
        geode::SimulationReport report;
        Exception::test(
            report.is_simulable() && report.string() == "No simulation issue",
            "[Test] Empty report" );
        report.add_warning( "first ", 1 );
        Exception::test( report.is_simulable()
                             && report.string() == "1 warning:\n - first 1",
            "[Test] Warning only report: ", report.string() );
        report.add_errors( { "a", "b" } );
        Exception::test( !report.is_simulable()
                             && report.string()
                                    == "2 errors:\n - a\n - b\n1 warning:\n - "
                                       "first 1",
            "[Test] Full report: ", report.string() );
    }

    void test_dispatch()
    {
        geode::SimulationCheckerFactory::register_creator< DummyChecker >(
            "dummy" );
        geode::SimulationCheckerFactory::register_creator< ThrowingChecker >(
            "throwing" );
        const TetModel tet;
        auto problem = water_problem();
        Exception::test(
            geode::is_physical_problem_simulable( problem, tet.model, "Dummy" ),
            "[Test] Valid problem should be simulable: ",
            geode::check_physical_problem_simulation(
                problem, tet.model, "dummy" )
                .string() );
        geode::PhysicalProblemBuilder{ problem }.set_schedule(
            { 10., 0., 1., std::nullopt } );
        check_error( geode::check_physical_problem_simulation(
                         problem, tet.model, "dummy" ),
            "Schedule end time", "generic issue through the factory" );
        const auto unknown = geode::check_physical_problem_simulation(
            problem, tet.model, "unknown" );
        check_error( unknown, "Unknown simulator", "unknown simulator" );
        check_error( unknown, "dummy, throwing", "registered simulators" );
        Exception::test( !geode::is_physical_problem_simulable(
                             problem, tet.model, "unknown" ),
            "[Test] Unknown simulator is not simulable" );
        check_error( geode::check_physical_problem_simulation(
                         problem, tet.model, "throwing" ),
            "broken checker", "throwing checker" );
    }

    void test_mesh_checks()
    {
        TetModel tet;
        geode::SimulationReport report;
        geode::check_simulation_blocks( tet.model, report );
        geode::check_simulation_surfaces_on_blocks(
            tet.model, { &tet.surface_id, 1 }, report );
        geode::check_simulation_unique_vertices_in_blocks( tet.model, report );
        check_no_error( report, "valid mesh" );

        geode::BRepBuilder builder{ tet.model };
        builder.unset_unique_vertex(
            { tet.model.block( tet.block_id ).component_id(), 3 }, 3 );
        static_cast< void >( builder.add_block() );
        const auto& surface = tet.model.surface( tet.surface_id );
        builder.surface_mesh_builder< geode::TriangulatedSurface3D >( surface )
            ->create_point( geode::Point3D{ { 5, 5, 5 } } );
        builder.surface_mesh_builder< geode::TriangulatedSurface3D >( surface )
            ->create_triangle( { 0, 1, 3 } );
        builder.set_unique_vertex( { surface.component_id(), 3 },
            builder.create_unique_vertices( 1 ) );
        geode::SimulationReport broken;
        geode::check_simulation_blocks( tet.model, broken );
        geode::check_simulation_surfaces_on_blocks(
            tet.model, { &tet.surface_id, 1 }, broken );
        geode::check_simulation_unique_vertices_in_blocks( tet.model, broken );
        check_error( broken, "has no polyhedron", "empty Block" );
        check_error( broken, "1 polyhedron vertices without unique vertex",
            "Block vertex without unique vertex" );
        check_error( broken, "are not Block polyhedron vertices",
            "Surface vertex outside Blocks" );
        check_error( broken, "2 unique vertices are not Block vertices",
            "unique vertices outside Blocks" );

        geode::BRep empty;
        geode::SimulationReport no_block;
        geode::check_simulation_blocks( empty, no_block );
        check_error( no_block, "no Block", "model without Block" );
    }

    template < typename Value >
    geode::uuid add_attribute( geode::AttributeManager& manager,
        Value value,
        std::optional< Value > no_value = std::nullopt )
    {
        geode::AttributeValues< Value > values;
        values.default_value = value;
        values.no_value = no_value;
        return manager.create_attribute< geode::VariableAttribute, Value >(
            "property", std::move( values ), {} );
    }

    geode::AttributeManager& polyhedra( const TetModel& tet )
    {
        return tet.model.block( tet.block_id )
            .mesh()
            .polyhedron_attribute_manager();
    }

    geode::AttributeManager& polygons( const TetModel& tet )
    {
        return tet.model.surface( tet.surface_id )
            .mesh()
            .polygon_attribute_manager();
    }

    void register_property( TetModel& tet,
        PHYSICAL_PROPERTY_NAME property,
        const geode::ComponentType& type,
        const geode::uuid& attribute_id )
    {
        geode::BRepBuilder{ tet.model }.set_physical_property(
            property, type, attribute_id );
    }

    geode::SimulationReport check_property( const TetModel& tet,
        const geode::PhysicalPropertyRequirement& requirement )
    {
        geode::SimulationReport report;
        static_cast< void >(
            geode::check_physical_property( tet.model, requirement, report ) );
        return report;
    }

    void test_rock_property_checks()
    {
        const auto block_type = geode::Block3D::component_type_static();
        const geode::PhysicalPropertyRequirement porosity{
            PHYSICAL_PROPERTY_NAME::porosity
        };
        TetModel tet;
        geode::SimulationReport report;
        const auto absent =
            geode::check_physical_property( tet.model, porosity, report );
        Exception::test( report.errors.empty() && !absent.is_registered
                             && !absent.is_carried(),
            "[Test] Unregistered property is not an issue" );

        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( polyhedra( tet ), 0.2 ) );
        geode::SimulationReport valid;
        const auto coverage =
            geode::check_physical_property( tet.model, porosity, valid );
        check_no_error( valid, "valid porosity" );
        Exception::test( coverage.is_registered
                             && coverage.covers( tet.block_id )
                             && coverage.value_bounds
                             && coverage.value_bounds->at( 1 ) == 0.2,
            "[Test] Wrong porosity coverage" );

        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( polyhedra( tet ), 1.5 ) );
        check_error( check_property( tet, porosity ), "values outside (0, 1]",
            "porosity range" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( polyhedra( tet ),
                std::numeric_limits< double >::quiet_NaN() ) );
        check_error(
            check_property( tet, porosity ), "values outside", "NaN porosity" );

        const geode::PhysicalPropertyRequirement permeability{
            PHYSICAL_PROPERTY_NAME::permeability
        };
        register_property( tet, PHYSICAL_PROPERTY_NAME::permeability,
            block_type,
            add_attribute( polyhedra( tet ),
                std::array< double, 3 >{ 1e-13, 1e-13, 0. } ) );
        check_no_error( check_property( tet, permeability ),
            "zero permeability (barrier)" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::permeability,
            block_type,
            add_attribute( polyhedra( tet ),
                std::array< double, 3 >{ 1e-13, -1e-13, 1e-13 } ) );
        check_error( check_property( tet, permeability ),
            "values outside [0, +inf[", "negative permeability" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity,
            geode::Surface3D::component_type_static(),
            add_attribute( polygons( tet ), 0.2 ) );
        check_error( check_property( tet, porosity ),
            "is registered on Surface", "porosity on Surfaces" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( tet.model.block( tet.block_id )
                               .mesh()
                               .vertex_attribute_manager(),
                0.2 ) );
        check_error( check_property( tet, porosity ), "on vertices",
            "porosity on vertices" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( polyhedra( tet ), 0.2f ) );
        check_error( check_property( tet, porosity ), "must store double",
            "float porosity" );

        const auto permeability_id = add_attribute(
            polyhedra( tet ), std::array< double, 3 >{ 1e-13, 1e-13, 1e-13 } );
        register_property( tet, PHYSICAL_PROPERTY_NAME::permeability,
            block_type, permeability_id );
        check_no_error( check_property( tet,
                            geode::PhysicalPropertyRequirement{
                                PHYSICAL_PROPERTY_NAME::permeability } ),
            "vector permeability" );
        geode::PhysicalPropertyRequirement scalar_permeability{
            PHYSICAL_PROPERTY_NAME::permeability
        };
        scalar_permeability.value_types = {
            geode::PHYSICAL_PROPERTY_VALUE_TYPE::scalar
        };
        check_error( check_property( tet, scalar_permeability ),
            "must store double values", "restricted value types" );
    }

    // Adds a Block meshed with one tetrahedron away from the TetModel one
    [[nodiscard]] geode::uuid add_meshed_block( TetModel& tet )
    {
        geode::BRepBuilder builder{ tet.model };
        const auto block_id =
            builder.add_block( geode::MeshFactory::default_impl(
                geode::TetrahedralSolid3D::type_name_static() ) );
        auto block_builder =
            builder.block_mesh_builder< geode::TetrahedralSolid3D >(
                tet.model.block( block_id ) );
        for( const auto vertex : geode::Range{ 4 } )
        {
            block_builder->create_point(
                geode::Point3D{ { 2. + vertex, 0, 0 } } );
        }
        block_builder->create_tetrahedron( { 0, 1, 2, 3 } );
        return block_id;
    }

    template < typename Value >
    void add_attribute_with_id( geode::AttributeManager& manager,
        const geode::uuid& attribute_id,
        std::string_view name,
        Value value,
        geode::AttributeProperties properties = {} )
    {
        geode::AttributeValues< Value > values;
        values.default_value = value;
        manager.create_attribute< geode::VariableAttribute, Value >(
            name, attribute_id, std::move( values ), std::move( properties ) );
    }

    geode::AttributeManager& polyhedra(
        const geode::BRep& model, const geode::uuid& block_id )
    {
        return model.block( block_id ).mesh().polyhedron_attribute_manager();
    }

    void test_merged_property_issues()
    {
        const auto block_type = geode::Block3D::component_type_static();
        geode::PhysicalPropertyRequirement merged_porosity{
            PHYSICAL_PROPERTY_NAME::porosity
        };
        merged_porosity.merged = true;

        TetModel not_transferable;
        geode::AttributeProperties properties;
        properties.transferable = false;
        const geode::uuid not_transferable_id;
        add_attribute_with_id( polyhedra( not_transferable ),
            not_transferable_id, "porosity", 0.2, properties );
        register_property( not_transferable, PHYSICAL_PROPERTY_NAME::porosity,
            block_type, not_transferable_id );
        check_error( check_property( not_transferable, merged_porosity ),
            "is not transferable", "merged attribute not transferable" );
        const geode::PhysicalPropertyRequirement split_porosity{
            PHYSICAL_PROPERTY_NAME::porosity
        };
        check_no_error( check_property( not_transferable, split_porosity ),
            "non transferable attribute is fine when not merged" );

        TetModel unnamed;
        const geode::uuid unnamed_id;
        add_attribute_with_id( polyhedra( unnamed ), unnamed_id, "", 0.2 );
        register_property(
            unnamed, PHYSICAL_PROPERTY_NAME::porosity, block_type, unnamed_id );
        check_error( check_property( unnamed, merged_porosity ), "has no name",
            "merged attribute without name" );

        TetModel mixed;
        const auto other_block = add_meshed_block( mixed );
        const geode::uuid permeability_id;
        add_attribute_with_id(
            polyhedra( mixed ), permeability_id, "permeability", 1e-13 );
        add_attribute_with_id( polyhedra( mixed.model, other_block ),
            permeability_id, "permeability",
            std::array< double, 3 >{ 1e-13, 1e-13, 1e-14 } );
        register_property( mixed, PHYSICAL_PROPERTY_NAME::permeability,
            block_type, permeability_id );
        const geode::PhysicalPropertyRequirement split_permeability{
            PHYSICAL_PROPERTY_NAME::permeability
        };
        check_no_error( check_property( mixed, split_permeability ),
            "per-Block value types are fine when not merged" );
        auto merged_permeability = split_permeability;
        merged_permeability.merged = true;
        const auto mixed_report = check_property( mixed, merged_permeability );
        check_error( mixed_report, "has another value type than on",
            "merged attribute with several value types" );
        Exception::test( mixed_report.errors.size() == 1,
            "[Test] Only the second Block should be reported: ",
            mixed_report.string() );
    }

    void test_merged_and_missing_values()
    {
        const auto block_type = geode::Block3D::component_type_static();
        TetModel tet;
        static_cast< void >( geode::BRepBuilder{ tet.model }.add_block() );
        const auto missing_id = add_attribute< double >( polyhedra( tet ),
            std::numeric_limits< double >::quiet_NaN(),
            std::numeric_limits< double >::quiet_NaN() );
        register_property(
            tet, PHYSICAL_PROPERTY_NAME::porosity, block_type, missing_id );
        geode::PhysicalPropertyRequirement fallback{
            PHYSICAL_PROPERTY_NAME::porosity
        };
        fallback.value_on_every_element = false;
        geode::SimulationReport report;
        const auto coverage =
            geode::check_physical_property( tet.model, fallback, report );
        check_no_error( report, "missing values allowed" );
        Exception::test(
            !coverage.carries( tet.block_id ) && !coverage.is_carried(),
            "[Test] A Block without any value does not carry the property" );
        check_error( check_property( tet,
                         geode::PhysicalPropertyRequirement{
                             PHYSICAL_PROPERTY_NAME::porosity } ),
            "has no value on 1 of 1 polyhedra", "missing values" );

        auto problem = water_problem();
        auto material = problem.materials()[0];
        material.porosity.reset();
        geode::PhysicalProblemBuilder{ problem }.set_material( 0, material );
        geode::SimulationReport availability;
        geode::check_rock_property_availability(
            problem, tet.model, coverage, availability );
        check_error( availability, "does not cover every polyhedron",
            "material without porosity" );

        geode::PhysicalPropertyRequirement merged{
            PHYSICAL_PROPERTY_NAME::porosity
        };
        merged.merged = true;
        const auto meshed_id = add_attribute( polyhedra( tet ), 0.3 );
        register_property(
            tet, PHYSICAL_PROPERTY_NAME::porosity, block_type, meshed_id );
        check_no_error( check_property( tet, merged ),
            "merged property on every meshed Block" );
        static_cast< void >( add_meshed_block( tet ) );
        check_error( check_property( tet, merged ), "has no porosity attribute",
            "merged property missing on a meshed Block" );

        geode::BRep empty;
        geode::SimulationReport no_property;
        geode::check_rock_property_availability( problem, empty,
            geode::PhysicalPropertyCoverage{ PHYSICAL_PROPERTY_NAME::porosity },
            no_property );
        check_no_error( no_property, "no meshed Block to cover" );
        const TetModel unregistered;
        geode::check_rock_property_availability( problem, unregistered.model,
            geode::PhysicalPropertyCoverage{ PHYSICAL_PROPERTY_NAME::porosity },
            no_property );
        check_error( no_property, "has no porosity PhysicalProperty",
            "unregistered porosity" );
    }

    void test_boundary_property_checks()
    {
        const auto surface_type = geode::Surface3D::component_type_static();
        TetModel tet;
        geode::SimulationReport presence;
        const geode::PhysicalPropertyRequirement pressure_requirement{
            PHYSICAL_PROPERTY_NAME::boundary_pressure
        };
        const auto none = geode::check_physical_property(
            tet.model, pressure_requirement, presence );
        geode::check_physical_property_presence(
            tet.model, none, "no flow", presence );
        check_warning( presence,
            "no boundary_pressure PhysicalProperty: no flow",
            "missing boundary pressure" );

        register_property( tet, PHYSICAL_PROPERTY_NAME::boundary_pressure,
            surface_type, add_attribute( polygons( tet ), 1e5 ) );
        register_property( tet, PHYSICAL_PROPERTY_NAME::boundary_temperature,
            surface_type, geode::uuid{} );
        geode::SimulationReport report;
        const std::array< geode::PhysicalPropertyCoverage, 2 > coverages{
            geode::check_physical_property(
                tet.model, pressure_requirement, report ),
            geode::check_physical_property( tet.model,
                geode::PhysicalPropertyRequirement{
                    PHYSICAL_PROPERTY_NAME::boundary_temperature },
                report )
        };
        check_no_error( report, "boundary properties" );
        geode::check_coinciding_physical_properties(
            tet.model, coverages, report );
        check_error( report,
            "carries boundary_pressure but not boundary_temperature",
            "partial boundary set" );
        geode::check_physical_property_presence(
            tet.model, coverages[1], "no heat", report );
        check_warning( report, "No Surface carries a boundary_temperature",
            "registered but not carried" );

        register_property( tet, PHYSICAL_PROPERTY_NAME::boundary_water_fraction,
            surface_type, add_attribute( polygons( tet ), 2. ) );
        check_error(
            check_property( tet,
                geode::PhysicalPropertyRequirement{
                    PHYSICAL_PROPERTY_NAME::boundary_water_fraction } ),
            "outside [0, 1]", "fraction range" );
        geode::SimulationReport ignored;
        const std::array< PHYSICAL_PROPERTY_NAME, 2 > ignored_properties{
            PHYSICAL_PROPERTY_NAME::boundary_water_fraction,
            PHYSICAL_PROPERTY_NAME::boundary_oil_fraction
        };
        geode::warn_ignored_physical_properties(
            tet.model, ignored_properties, "Test", ignored );
        Exception::test( ignored.warnings.size() == 1
                             && absl::StrContains( ignored.warnings.front(),
                                 "boundary_water_fraction is ignored by Test" ),
            "[Test] Wrong ignored properties: ", ignored.string() );
    }

    void test_block_maps()
    {
        TetModel tet;
        auto problem = water_problem();
        geode::PhysicalProblemBuilder builder{ problem };
        auto top = problem.materials()[0];
        top.name = "top";
        top.regions = { tet.block_id };
        static_cast< void >( builder.add_material( top ) );
        const auto materials =
            geode::physical_problem_block_materials( problem, tet.model );
        Exception::test(
            materials.size() == 1 && materials.at( tet.block_id ) == 1,
            "[Test] Explicit region should win over the default material" );
        Exception::test( geode::physical_problem_block_initial_conditions(
                             problem, tet.model )
                                 .at( tet.block_id )
                             == 0,
            "[Test] Default initial condition should cover the Block" );
        geode::SimulationReport report;
        geode::check_physical_problem_consistency( problem, tet.model, report );
        check_no_error( report, "explicit and default materials" );
        check_warning( report, "Material rock covers no Block",
            "unused default material" );

        static_cast< void >( builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{ tet.block_id }, 2e7 ) ) );
        Exception::test( geode::physical_problem_block_initial_conditions(
                             problem, tet.model )
                                 .at( tet.block_id )
                             == 1,
            "[Test] Explicit region should win over the default initial "
            "condition" );
        geode::SimulationReport condition_report;
        geode::check_physical_problem_consistency(
            problem, tet.model, condition_report );
        check_no_error( condition_report, "explicit and default conditions" );
        check_warning( condition_report, "Initial condition #0 covers no Block",
            "unused default initial condition" );
    }

    [[nodiscard]] geode::Material named_material(
        const geode::PhysicalProblem& problem,
        std::string name,
        std::vector< geode::uuid > regions )
    {
        auto material = problem.materials()[0];
        material.name = std::move( name );
        material.regions = std::move( regions );
        return material;
    }

    void test_block_map_priorities()
    {
        TetModel tet;
        const auto other_block = geode::BRepBuilder{ tet.model }.add_block();
        auto problem = water_problem();
        geode::PhysicalProblemBuilder builder{ problem };
        static_cast< void >( builder.add_material(
            named_material( problem, "top", { tet.block_id } ) ) );
        static_cast< void >( builder.add_material(
            named_material( problem, "bottom", { tet.block_id } ) ) );
        static_cast< void >( builder.add_material(
            named_material( problem, "ghost", { geode::uuid{} } ) ) );
        static_cast< void >( builder.add_material(
            named_material( problem, "surface", { tet.surface_id } ) ) );
        static_cast< void >(
            builder.add_material( named_material( problem, "other", {} ) ) );
        const auto materials =
            geode::physical_problem_block_materials( problem, tet.model );
        Exception::test( materials.size() == 2,
            "[Test] Every Block should get a material, invalid regions should "
            "be skipped" );
        Exception::test( materials.at( tet.block_id ) == 1,
            "[Test] First explicit material should win, got ",
            materials.at( tet.block_id ) );
        Exception::test( materials.at( other_block ) == 0,
            "[Test] First default material should cover the other Block, got ",
            materials.at( other_block ) );

        static_cast< void >( builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{ geode::uuid{}, tet.block_id },
                2e7 ) ) );
        static_cast< void >( builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{ tet.block_id }, 3e7 ) ) );
        static_cast< void >( builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{}, 4e7 ) ) );
        const auto conditions =
            geode::physical_problem_block_initial_conditions(
                problem, tet.model );
        Exception::test( conditions.size() == 2
                             && conditions.at( tet.block_id ) == 1
                             && conditions.at( other_block ) == 0,
            "[Test] Wrong Block initial conditions" );

        geode::SimulationReport report;
        geode::check_physical_problem_consistency( problem, tet.model, report );
        for( const auto* keyword : { "Material bottom covers no Block",
                 "Material other covers no Block",
                 "Initial condition #2 covers no Block",
                 "Initial condition #3 covers no Block" } )
        {
            check_warning( report, keyword, "shadowed item" );
        }
        for( const auto* keyword : { "Material ghost", "Material surface" } )
        {
            Exception::test( !contains( report.warnings, keyword ),
                "[Test] Item with an invalid region is reported as an error, "
                "not as unused: ",
                report.string() );
        }
    }

    void test_surface_vertex_without_unique_vertex()
    {
        TetModel tet;
        geode::BRepBuilder builder{ tet.model };
        const auto& surface = tet.model.surface( tet.surface_id );
        auto surface_builder =
            builder.surface_mesh_builder< geode::TriangulatedSurface3D >(
                surface );
        surface_builder->create_point( geode::Point3D{ { 0, 0, 1 } } );
        surface_builder->create_triangle( { 0, 1, 3 } );
        Exception::test(
            tet.model.unique_vertex( { surface.component_id(), 3 } )
                == geode::NO_ID,
            "[Test] New Surface vertex should have no unique vertex" );
        geode::SimulationReport report;
        geode::check_simulation_surfaces_on_blocks(
            tet.model, { &tet.surface_id, 1 }, report );
        Exception::test( report.errors.size() == 1,
            "[Test] Wrong errors: ", report.string() );
        check_error( report,
            "has 1 polygon vertices which are not Block polyhedron vertices "
            "(first: vertex 3)",
            "Surface vertex without unique vertex" );
        geode::SimulationReport unique_vertices;
        geode::check_simulation_unique_vertices_in_blocks(
            tet.model, unique_vertices );
        check_no_error(
            unique_vertices, "no unique vertex is created for the vertex" );
    }
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesSimulationLibrary::initialize();
        test_report_string();
        test_dispatch();
        test_mesh_checks();
        test_surface_vertex_without_unique_vertex();
        test_rock_property_checks();
        test_merged_and_missing_values();
        test_merged_property_issues();
        test_boundary_property_checks();
        test_block_maps();
        test_block_map_priorities();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
