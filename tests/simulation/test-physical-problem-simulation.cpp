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

#include <absl/algorithm/container.h>
#include <absl/strings/ascii.h>
#include <absl/strings/match.h>
#include <absl/strings/str_cat.h>
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
#include <geode/geosciences/simulation/helpers/internal/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/simulation_checker.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace
{
    using Exception = geode::OpenGeodeGeosciencesSimulationException;
    using geode::PHYSICAL_PROPERTY_NAME;
    using geode::SIMULATION_CHECK_STAGE;
    using geode::SIMULATION_ISSUE_SEVERITY;

    // Generic checks, then one simulator warning
    class DummyChecker final : public geode::SimulationChecker
    {
    public:
        DummyChecker(
            const geode::PhysicalProblem& problem, const geode::BRep& model )
            : geode::SimulationChecker( problem, model )
        {
        }

        // Checks available to simulator checkers, exposed for the tests
        using geode::SimulationChecker::check_blocks;
        using geode::SimulationChecker::check_physical_property;
        using geode::SimulationChecker::check_physical_property_presence;
        using geode::SimulationChecker::check_rock_property_availability;
        using geode::SimulationChecker::check_unique_vertices_in_blocks;
        using geode::SimulationChecker::report;
        using geode::SimulationChecker::warn_ignored_physical_properties;

    private:
        void check_simulator() final
        {
            report().set_simulator( "Dummy" );
            report().add_warning(
                SIMULATION_CHECK_STAGE::simulator, "Dummy", "simulator check" );
        }
    };

    // Checker of a model, without any problem
    DummyChecker model_checker( const geode::BRep& model )
    {
        static const geode::PhysicalProblem problem;
        return DummyChecker{ problem, model };
    }

    // Issue of this severity whose subject and message, lower case,
    // contain the keyword
    const geode::SimulationIssue* find_issue(
        const geode::SimulationReport& report,
        SIMULATION_ISSUE_SEVERITY severity,
        std::string_view keyword )
    {
        const auto lower_keyword = absl::AsciiStrToLower( keyword );
        for( const auto& issue : report.issues() )
        {
            if( issue.severity == severity
                && absl::StrContains( absl::AsciiStrToLower( absl::StrCat(
                                          issue.subject, " ", issue.message ) ),
                    lower_keyword ) )
            {
                return &issue;
            }
        }
        return nullptr;
    }

    const geode::SimulationIssue& check_error(
        const geode::SimulationReport& report,
        std::string_view keyword,
        std::string_view context )
    {
        const auto* issue =
            find_issue( report, SIMULATION_ISSUE_SEVERITY::error, keyword );
        Exception::test( issue != nullptr, "[Test] ", context,
            ": no error containing \"", keyword, "\" in: ", report.string() );
        return *issue;
    }

    void check_warning( const geode::SimulationReport& report,
        std::string_view keyword,
        std::string_view context )
    {
        Exception::test(
            find_issue( report, SIMULATION_ISSUE_SEVERITY::warning, keyword )
                != nullptr,
            "[Test] ", context, ": no warning containing \"", keyword,
            "\" in: ", report.string() );
    }

    // Elements of the only location of the issue
    void check_elements( const geode::SimulationIssue& issue,
        geode::SIMULATION_ELEMENT_TYPE type,
        absl::Span< const geode::index_t > elements,
        std::string_view context )
    {
        Exception::test(
            issue.locations.size() == 1
                && issue.locations.front().element_type == type
                && absl::c_equal( issue.locations.front().elements, elements ),
            "[Test] ", context, ": wrong location in ", issue.string() );
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
        report.set_model_name( "\"model\"" );
        report.set_simulator( "Test" );
        Exception::test( report.is_simulable()
                             && report.string()
                                    == "Test can simulate this problem on "
                                       "model \"model\"",
            "[Test] Empty report: ", report.string() );
        report.add_warning( SIMULATION_CHECK_STAGE::properties, "density",
            "ignored, only read by ", "solid mechanics" );
        Exception::test( report.is_simulable() && report.nb_warnings() == 1,
            "[Test] Warning only report: ", report.string() );
        geode::SimulationIssue schedule{ SIMULATION_ISSUE_SEVERITY::error,
            SIMULATION_CHECK_STAGE::problem, "Schedule",
            "end time must be after start time" };
        schedule.fix = "swap them";
        report.add_issue( std::move( schedule ) );
        const geode::uuid block_id;
        const geode::ComponentID block{ geode::Block3D::component_type_static(),
            block_id };
        geode::SimulationIssue unmeshed{ SIMULATION_ISSUE_SEVERITY::error,
            SIMULATION_CHECK_STAGE::mesh, "Blocks",
            "1 Block without polyhedron" };
        auto& unmeshed_location = unmeshed.locations.emplace_back();
        unmeshed_location.component = block;
        unmeshed_location.name = "Unit";
        report.add_issue( std::move( unmeshed ) );
        geode::SimulationIssue porosity{ SIMULATION_ISSUE_SEVERITY::error,
            SIMULATION_CHECK_STAGE::properties, "porosity",
            "values outside (0, 1] on 1 Block" };
        auto& porosity_location = porosity.locations.emplace_back();
        porosity_location.component = block;
        porosity_location.element_type =
            geode::SIMULATION_ELEMENT_TYPE::polyhedron;
        porosity_location.elements = { 0, 1, 2, 3, 4, 5, 6 };
        porosity_location.nb_elements = 10;
        porosity_location.detail = "first value 2";
        report.add_issue( std::move( porosity ) );
        const auto expected = absl::StrCat(
            "Test cannot simulate this problem on model \"model\": 3 errors, "
            "1 warning\n\n"
            "Problem definition\n"
            "  error: Schedule: end time must be after start time\n"
            "    fix: swap them\n\n"
            "Model mesh\n"
            "  error: Blocks: 1 Block without polyhedron\n"
            "    Block \"Unit\"\n\n"
            "Physical properties\n"
            "  error: porosity: values outside (0, 1] on 1 Block\n"
            "    Block ",
            block_id.string(),
            ": 7 of 10 polyhedra (0, 1, 2, 3, 4, ...), first value 2\n"
            "  warning: density: ignored, only read by solid mechanics" );
        Exception::test( !report.is_simulable() && report.nb_errors() == 3
                             && report.string() == expected,
            "[Test] Full report:\n", report.string(), "\nexpected:\n",
            expected );
        Exception::test(
            report.issues( SIMULATION_CHECK_STAGE::properties ).size() == 2,
            "[Test] Wrong properties stage issues" );
    }

    void test_checker()
    {
        geode::SimulationCheckerFactory::register_creator< DummyChecker >(
            "dummy" );
        const TetModel tet;
        auto problem = water_problem();
        const auto report = geode::SimulationCheckerFactory::create(
            "dummy", problem, tet.model )
                                ->check();
        Exception::test( report.is_simulable() && report.simulator() == "Dummy"
                             && report.model_name() == tet.model.id().string(),
            "[Test] Valid problem should be simulable: ", report.string() );
        Exception::test( report.nb_warnings() == 1
                             && report.issues().back().stage
                                    == SIMULATION_CHECK_STAGE::simulator,
            "[Test] Simulator issues come after the generic ones: ",
            report.string() );
        geode::PhysicalProblemBuilder{ problem }.set_schedule(
            { 10., 0., 1., std::nullopt } );
        check_error( geode::SimulationCheckerFactory::create(
                         "dummy", problem, tet.model )
                         ->check(),
            "Schedule end time", "generic issue through the checker" );
    }

    void test_mesh_checks()
    {
        TetModel tet;
        auto valid_checker = model_checker( tet.model );
        valid_checker.check_blocks();
        valid_checker.check_unique_vertices_in_blocks();
        check_no_error( valid_checker.report(), "valid mesh" );

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
        auto broken_checker = model_checker( tet.model );
        broken_checker.check_blocks();
        broken_checker.check_unique_vertices_in_blocks();
        const auto& broken = broken_checker.report();
        check_error( broken, "1 Block without polyhedron", "empty Block" );
        check_elements(
            check_error( broken, "polyhedron vertices without unique vertex",
                "Block vertex without unique vertex" ),
            geode::SIMULATION_ELEMENT_TYPE::vertex,
            std::array< geode::index_t, 1 >{ 3 }, "Block vertex" );
        check_elements( check_error( broken, "are not Block vertices",
                            "unique vertices outside Blocks" ),
            geode::SIMULATION_ELEMENT_TYPE::vertex,
            std::array< geode::index_t, 1 >{ 3 }, "Surface vertex" );
        check_error( broken, "1 unique vertex is not a vertex of any component",
            "isolated unique vertex" );
        Exception::test(
            broken.issues( SIMULATION_CHECK_STAGE::mesh ).size() == 4,
            "[Test] Mesh issues should be in the mesh stage: ",
            broken.string() );

        geode::BRep empty;
        auto empty_checker = model_checker( empty );
        empty_checker.check_blocks();
        check_error(
            empty_checker.report(), "no Block", "model without Block" );
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
        auto checker = model_checker( tet.model );
        static_cast< void >( checker.check_physical_property( requirement ) );
        return std::move( checker.report() );
    }

    void test_rock_property_checks()
    {
        const auto block_type = geode::Block3D::component_type_static();
        const geode::PhysicalPropertyRequirement porosity{
            PHYSICAL_PROPERTY_NAME::porosity
        };
        TetModel tet;
        auto absent_checker = model_checker( tet.model );
        const auto absent = absent_checker.check_physical_property( porosity );
        Exception::test( absent_checker.report().is_simulable()
                             && !absent.is_registered && !absent.is_carried(),
            "[Test] Unregistered property is not an issue" );

        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( polyhedra( tet ), 0.2 ) );
        auto valid_checker = model_checker( tet.model );
        const auto coverage = valid_checker.check_physical_property( porosity );
        check_no_error( valid_checker.report(), "valid porosity" );
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
            "values outside [0, +inf)", "negative permeability" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity,
            geode::Surface3D::component_type_static(),
            add_attribute( polygons( tet ), 0.2 ) );
        check_error( check_property( tet, porosity ), "registered on Surface",
            "porosity on Surfaces" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( tet.model.block( tet.block_id )
                               .mesh()
                               .vertex_attribute_manager(),
                0.2 ) );
        check_error( check_property( tet, porosity ), "stored on vertices",
            "porosity on vertices" );
        register_property( tet, PHYSICAL_PROPERTY_NAME::porosity, block_type,
            add_attribute( polyhedra( tet ), 0.2F ) );
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
            name, attribute_id, std::move( values ), properties );
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
            "not transferable", "merged attribute not transferable" );
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
        check_error( check_property( unnamed, merged_porosity ),
            "unnamed attribute", "merged attribute without name" );

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
        check_error( mixed_report, "attribute value type differs on 1 Block",
            "merged attribute with several value types" );
        Exception::test( mixed_report.nb_errors() == 1,
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
        auto fallback_checker = model_checker( tet.model );
        const auto coverage =
            fallback_checker.check_physical_property( fallback );
        check_no_error( fallback_checker.report(), "missing values allowed" );
        Exception::test(
            !coverage.carries( tet.block_id ) && !coverage.is_carried(),
            "[Test] A Block without any value does not carry the property" );
        const auto missing_report =
            check_property( tet, geode::PhysicalPropertyRequirement{
                                     PHYSICAL_PROPERTY_NAME::porosity } );
        const auto& missing_values = check_error(
            missing_report, "porosity missing on 1 Block", "missing values" );
        Exception::test(
            missing_values.locations.size() == 1
                && missing_values.locations.front().all_elements
                && missing_values.locations.front().elements.empty(),
            "[Test] A Block missing every value lists no element: ",
            missing_values.string() );

        auto problem = water_problem();
        auto material = problem.materials()[0];
        material.porosity.reset();
        geode::PhysicalProblemBuilder{ problem }.set_material( 0, material );
        DummyChecker availability{ problem, tet.model };
        availability.check_rock_property_availability( { &coverage, 1 } );
        check_error( availability.report(), "does not cover every polyhedron",
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
        const auto missing_attribute_report = check_property( tet, merged );
        const auto& missing_attribute = check_error( missing_attribute_report,
            "porosity missing on 1 Block",
            "merged property missing on a meshed Block" );
        Exception::test(
            missing_attribute.locations.size() == 1
                && missing_attribute.locations.front().detail == "no attribute"
                && missing_attribute.locations.front().all_elements
                && missing_attribute.locations.front().nb_elements == 1,
            "[Test] Missing attribute location: ", missing_attribute.string() );

        const std::array< geode::PhysicalPropertyCoverage, 1 > no_coverage{
            geode::PhysicalPropertyCoverage{ PHYSICAL_PROPERTY_NAME::porosity }
        };
        geode::BRep empty;
        DummyChecker no_block{ problem, empty };
        no_block.check_rock_property_availability( no_coverage );
        check_no_error( no_block.report(), "no meshed Block to cover" );
        const TetModel unregistered;
        DummyChecker no_property{ problem, unregistered.model };
        no_property.check_rock_property_availability( no_coverage );
        check_error( no_property.report(), "has no porosity physical property",
            "unregistered porosity" );
    }

    void test_boundary_property_checks()
    {
        const auto surface_type = geode::Surface3D::component_type_static();
        TetModel tet;
        auto presence = model_checker( tet.model );
        const geode::PhysicalPropertyRequirement pressure_requirement{
            PHYSICAL_PROPERTY_NAME::boundary_pressure
        };
        const auto none =
            presence.check_physical_property( pressure_requirement );
        presence.check_physical_property_presence( none, "no flow" );
        check_warning( presence.report(),
            "boundary_pressure not in the model: no flow",
            "missing boundary pressure" );

        register_property( tet, PHYSICAL_PROPERTY_NAME::boundary_pressure,
            surface_type, add_attribute( polygons( tet ), 1e5 ) );
        register_property( tet, PHYSICAL_PROPERTY_NAME::boundary_temperature,
            surface_type, geode::uuid{} );
        auto boundaries = model_checker( tet.model );
        const std::array< geode::PhysicalPropertyCoverage, 2 > coverages{
            boundaries.check_physical_property( pressure_requirement ),
            boundaries.check_physical_property(
                geode::PhysicalPropertyRequirement{
                    PHYSICAL_PROPERTY_NAME::boundary_temperature } )
        };
        check_no_error( boundaries.report(), "boundary properties" );
        boundaries.check_physical_property_presence( coverages[1], "no heat" );
        check_warning( boundaries.report(),
            "boundary_temperature no Surface carries a value: no heat",
            "registered but not carried" );

        register_property( tet, PHYSICAL_PROPERTY_NAME::boundary_water_fraction,
            surface_type, add_attribute( polygons( tet ), 2. ) );
        check_error(
            check_property( tet,
                geode::PhysicalPropertyRequirement{
                    PHYSICAL_PROPERTY_NAME::boundary_water_fraction } ),
            "outside [0, 1]", "fraction range" );
        auto ignored_checker = model_checker( tet.model );
        auto& ignored = ignored_checker.report();
        ignored.set_simulator( "Test" );
        const std::array< PHYSICAL_PROPERTY_NAME, 2 > ignored_properties{
            PHYSICAL_PROPERTY_NAME::boundary_water_fraction,
            PHYSICAL_PROPERTY_NAME::boundary_oil_fraction
        };
        ignored_checker.warn_ignored_physical_properties( ignored_properties );
        Exception::test(
            ignored.nb_warnings() == 1
                && ignored.issues().front().stage
                       == SIMULATION_CHECK_STAGE::simulator
                && find_issue( ignored, SIMULATION_ISSUE_SEVERITY::warning,
                       "boundary_water_fraction ignored by Test" )
                       != nullptr,
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
            geode::internal::physical_problem_block_materials(
                problem, tet.model );
        Exception::test(
            materials.size() == 1 && materials.at( tet.block_id ) == 1,
            "[Test] Explicit region should win over the default material" );
        Exception::test(
            geode::internal::physical_problem_block_initial_conditions(
                problem, tet.model )
                    .at( tet.block_id )
                == 0,
            "[Test] Default initial condition should cover the Block" );
        const auto report = DummyChecker{ problem, tet.model }.check();
        check_no_error( report, "explicit and default materials" );
        check_warning( report, "Material \"rock\" covers no Block",
            "unused default material" );

        static_cast< void >( builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{ tet.block_id }, 2e7 ) ) );
        Exception::test(
            geode::internal::physical_problem_block_initial_conditions(
                problem, tet.model )
                    .at( tet.block_id )
                == 1,
            "[Test] Explicit region should win over the default initial "
            "condition" );
        const auto condition_report =
            DummyChecker{ problem, tet.model }.check();
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
            geode::internal::physical_problem_block_materials(
                problem, tet.model );
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
            geode::internal::physical_problem_block_initial_conditions(
                problem, tet.model );
        Exception::test( conditions.size() == 2
                             && conditions.at( tet.block_id ) == 1
                             && conditions.at( other_block ) == 0,
            "[Test] Wrong Block initial conditions" );

        const auto report = DummyChecker{ problem, tet.model }.check();
        for( const auto* keyword : { "Material \"bottom\" covers no Block",
                 "Material \"other\" covers no Block",
                 "Initial condition #2 covers no Block",
                 "Initial condition #3 covers no Block" } )
        {
            check_warning( report, keyword, "shadowed item" );
        }
        for( const auto* keyword :
            { "Material \"ghost\"", "Material \"surface\"" } )
        {
            Exception::test( find_issue( report,
                                 SIMULATION_ISSUE_SEVERITY::warning, keyword )
                                 == nullptr,
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
        auto unique_vertices = model_checker( tet.model );
        unique_vertices.check_unique_vertices_in_blocks();
        check_no_error( unique_vertices.report(),
            "no unique vertex is created for the vertex" );
    }
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesSimulationLibrary::initialize();
        test_report_string();
        test_checker();
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
