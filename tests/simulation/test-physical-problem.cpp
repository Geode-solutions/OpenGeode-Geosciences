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

#include <geode/tests_config.hpp>

#include <algorithm>
#include <array>
#include <filesystem>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string>

#include <absl/strings/match.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/common.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/block_collection.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/builder/physical_problem_builder.hpp>
#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/saturation_functions.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/solver_options.hpp>
#include <geode/geosciences/simulation/core/source_term.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>
#include <geode/geosciences/simulation/helpers/physical_problem_validation.hpp>
#include <geode/geosciences/simulation/io/physical_problem_input.hpp>
#include <geode/geosciences/simulation/io/physical_problem_output.hpp>

namespace
{
    using Exception = geode::OpenGeodeGeosciencesSimulationException;

    // Constructor arguments of a DeadOilFluid, modified by validation tests
    struct DeadOilData
    {
        std::vector< geode::PHASE > phases;
        std::vector< geode::FluidComponent > components;
        std::vector< double > surface_densities;
        std::vector< geode::HydrocarbonPvtTable > hydrocarbon_pvt;
        std::optional< geode::WaterPvt > water_pvt;
    };

    DeadOilData spe10_dead_oil_data()
    {
        DeadOilData data;
        data.phases = { geode::PHASE::oil, geode::PHASE::gas,
            geode::PHASE::water };
        data.components = { { "oil", 0.114 }, { "gas", 0.016 },
            { "water", 0.018 } };
        data.surface_densities = { 800.0, 0.9907, 1022.0 };
        geode::HydrocarbonPvtTable oil_table;
        oil_table.phase = geode::PHASE::oil;
        oil_table.pressures = { 1e5, 1e7, 5e7 };
        oil_table.formation_volume_factors = { 1.02, 1.01, 0.99 };
        oil_table.viscosities = { 2.9e-3, 3e-3, 3.1e-3 };
        geode::HydrocarbonPvtTable gas_table;
        gas_table.phase = geode::PHASE::gas;
        gas_table.pressures = { 1e5, 1e7, 5e7 };
        gas_table.formation_volume_factors = { 0.9, 0.012, 0.003 };
        gas_table.viscosities = { 1e-5, 1.5e-5, 2.5e-5 };
        data.hydrocarbon_pvt = { oil_table, gas_table };
        data.water_pvt = geode::WaterPvt{ 3.0e7, 1.03, 4.4e-10, 3e-4 };
        return data;
    }

    std::shared_ptr< geode::DeadOilFluid > dead_oil( DeadOilData data )
    {
        return std::make_shared< geode::DeadOilFluid >(
            std::move( data.phases ), std::move( data.components ),
            std::move( data.surface_densities ),
            std::move( data.hydrocarbon_pvt ), std::move( data.water_pvt ) );
    }

    std::shared_ptr< geode::DeadOilFluid > spe10_dead_oil()
    {
        return dead_oil( spe10_dead_oil_data() );
    }

    std::shared_ptr< geode::BrooksCoreyRelativePermeability > brooks_corey(
        std::vector< double > residual_saturations = { 0.1, 0., 0.2 } )
    {
        return std::make_shared< geode::BrooksCoreyRelativePermeability >(
            std::vector< geode::PHASE >{
                geode::PHASE::oil, geode::PHASE::gas, geode::PHASE::water },
            std::move( residual_saturations ),
            std::vector< double >{ 2., 2., 2. },
            std::vector< double >{ 1., 1., 1. } );
    }

    geode::Material reservoir_material( std::string name,
        std::shared_ptr< geode::RelativePermeability > relperm )
    {
        geode::Material material;
        material.name = std::move( name );
        material.pore_compressibility = 1e-9;
        material.reference_pressure = 1e5;
        material.porosity = 0.2;
        material.permeability = std::array< double, 3 >{ 1e-13, 1e-13, 1e-14 };
        material.relative_permeability = std::move( relperm );
        return material;
    }

    geode::PhysicalProblem dead_oil_problem(
        const geode::uuid& model_id, const geode::uuid& block_id )
    {
        geode::PhysicalProblem problem;
        geode::PhysicalProblemBuilder builder{ problem };
        builder.set_name( "spe10" );
        builder.set_model_id( model_id );
        auto flow = std::make_shared< geode::MultiphaseFlow >(
            geode::COMPOSITION_BASIS::mass );
        flow->set_fluid( spe10_dead_oil() );
        builder.set_flow( flow );
        const auto relperm = brooks_corey();
        auto top = reservoir_material( "top", relperm );
        top.regions = { block_id };
        builder.add_material( top );
        builder.add_material( reservoir_material( "default", relperm ) );
        builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{}, 4.1369e7, 300.,
                std::vector< double >{ 0.9995, 0., 0.0005 } ) );
        builder.set_schedule( { 0., 2e6, 1e3, 5e5 } );
        geode::OutputSettings output;
        output.frequency = 1e5;
        output.restart = true;
        output.restart_frequency = 1e6;
        builder.set_output( output );
        builder.add_source_term( { "injector", "water", 10. } );
        builder.set_solver_option( "geos", "nonlinear/newtonTol", "1e-4" );
        return problem;
    }

    void check_no_issue( const geode::PhysicalProblem& problem )
    {
        const auto issues = geode::validate_physical_problem( problem );
        Exception::test( issues.empty(),
            "[Test] Unexpected issues: ", absl::StrJoin( issues, " | " ) );
    }

    void check_issue( const geode::PhysicalProblem& problem,
        std::string_view keyword,
        std::string_view context )
    {
        const auto issues = geode::validate_physical_problem( problem );
        const auto found = std::any_of(
            issues.begin(), issues.end(), [keyword]( const auto& issue ) {
                return absl::StrContains( issue, keyword );
            } );
        Exception::test( found, "[Test] ", context, ": no issue containing \"",
            keyword, "\" in: ", absl::StrJoin( issues, " | " ) );
    }

    void check_dead_oil_fluid( const geode::PhysicalProblem& problem )
    {
        const auto* fluid =
            dynamic_cast< const geode::DeadOilFluid* >( &problem.fluid() );
        Exception::test( fluid != nullptr, "[Test] Fluid should be dead oil" );
        Exception::test(
            fluid->phases().size() == 3
                && fluid->components()[2].name == "water"
                && fluid->hydrocarbon_pvt().size() == 2
                && fluid->hydrocarbon_pvt()[1].viscosities[2] == 2.5e-5
                && fluid->water_pvt()
                && fluid->water_pvt()->compressibility == 4.4e-10,
            "[Test] Wrong dead oil fluid" );
        const auto* flow =
            dynamic_cast< const geode::MultiphaseFlow* >( &problem.flow() );
        Exception::test(
            flow != nullptr && flow->basis() == geode::COMPOSITION_BASIS::mass,
            "[Test] Wrong flow model" );
    }

    void check_materials(
        const geode::PhysicalProblem& problem, const geode::uuid& block_id )
    {
        const auto materials = problem.materials();
        Exception::test( materials.size() == 2, "[Test] Wrong nb materials" );
        Exception::test( materials[0].name == "top"
                             && materials[0].regions.size() == 1
                             && materials[0].regions[0] == block_id
                             && materials[0].porosity == 0.2
                             && materials[0].permeability->at( 2 ) == 1e-14
                             && !materials[0].capillary_pressure
                             && materials[1].regions.empty(),
            "[Test] Wrong materials" );
        Exception::test( materials[0].relative_permeability
                             == materials[1].relative_permeability,
            "[Test] Relative permeability should be shared" );
        const auto* relperm =
            dynamic_cast< const geode::BrooksCoreyRelativePermeability* >(
                materials[0].relative_permeability.get() );
        Exception::test(
            relperm != nullptr && relperm->residual_saturations()[2] == 0.2,
            "[Test] Wrong relative permeability" );
    }

    void check_dead_oil_problem( const geode::PhysicalProblem& problem,
        const geode::uuid& model_id,
        const geode::uuid& block_id )
    {
        Exception::test(
            problem.name() == "spe10", "[Test] Wrong problem name" );
        Exception::test( problem.model_id() == model_id, "[Test] Wrong model" );
        check_dead_oil_fluid( problem );
        check_materials( problem, block_id );
        Exception::test( problem.nb_initial_conditions() == 1,
            "[Test] Wrong nb initial conditions" );
        const auto* condition =
            dynamic_cast< const geode::UniformInitialCondition* >(
                &problem.initial_condition( 0 ) );
        Exception::test( condition != nullptr
                             && condition->pressure() == 4.1369e7
                             && condition->temperature() == 300.
                             && condition->composition()[2] == 0.0005,
            "[Test] Wrong initial condition" );
        Exception::test( problem.schedule().end_time == 2e6
                             && problem.schedule().max_time_step == 5e5,
            "[Test] Wrong schedule" );
        Exception::test( problem.output().frequency == 1e5
                             && problem.output().restart
                             && problem.output().restart_frequency == 1e6,
            "[Test] Wrong output" );
        Exception::test(
            problem.gravity().value( 2 ) == -9.81, "[Test] Wrong gravity" );
        Exception::test(
            problem.solver_options().get( "geos", "nonlinear/newtonTol" )
                == "1e-4",
            "[Test] Wrong solver option" );
        Exception::test( !problem.thermal(), "[Test] Should be isothermal" );
        Exception::test( problem.source_terms().size() == 1
                             && problem.source_terms()[0].well == "injector"
                             && problem.source_terms()[0].component == "water"
                             && problem.source_terms()[0].rate == 10.,
            "[Test] Wrong source term" );
    }

    void test_dead_oil_roundtrip()
    {
        const geode::uuid model_id;
        const geode::uuid block_id;
        const auto problem = dead_oil_problem( model_id, block_id );
        check_no_issue( problem );
        check_dead_oil_problem( problem, model_id, block_id );

        const auto filename = absl::StrCat(
            "dead_oil.", geode::PhysicalProblem::native_extension_static() );
        geode::save_physical_problem( problem, filename );
        const auto reloaded = geode::load_physical_problem( filename );
        Exception::test( reloaded.id() == problem.id(), "[Test] Wrong id" );
        check_dead_oil_problem( reloaded, model_id, block_id );
        check_no_issue( reloaded );

        const auto clone = reloaded.clone();
        check_dead_oil_problem( clone, model_id, block_id );
        Exception::test( &clone.fluid() == &reloaded.fluid(),
            "[Test] Clone should share the fluid" );
    }

    std::shared_ptr< geode::SinglePhaseFlow > thermal_water_flow(
        std::optional< geode::FluidThermalProperties > thermal )
    {
        auto fluid = std::make_shared< geode::SinglePhaseFluid >(
            std::vector< geode::PHASE >{ geode::PHASE::water },
            std::vector< geode::FluidComponent >{ { "water", 0.018 } }, 1000.,
            1e-3, 5e-10, 0., 0., thermal );
        auto flow = std::make_shared< geode::SinglePhaseFlow >();
        flow->set_fluid( fluid );
        return flow;
    }

    geode::PhysicalProblem thermal_single_phase_problem()
    {
        geode::PhysicalProblem problem;
        geode::PhysicalProblemBuilder builder{ problem };
        builder.set_flow( thermal_water_flow(
            geode::FluidThermalProperties{ 4180., 2e-4, 0. } ) );
        builder.set_thermal( geode::ThermalParameters{ 300. } );
        geode::Material material;
        material.name = "rock";
        material.porosity = 0.1;
        material.permeability = std::array< double, 3 >{ 1e-14, 1e-14, 1e-14 };
        material.thermal =
            geode::RockThermalProperties{ 2e6, { 2.5, 2.5, 2.5 } };
        builder.add_material( material );
        builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{}, 1e7, 350. ) );
        builder.set_schedule( { 0., 1e6, 1e2, std::nullopt } );
        builder.set_gravity( geode::Vector3D{ { 0., 0., 0. } } );
        return problem;
    }

    void test_thermal_single_phase_roundtrip()
    {
        const auto problem = thermal_single_phase_problem();
        check_no_issue( problem );
        const auto filename = absl::StrCat(
            "thermal.", geode::PhysicalProblem::native_extension_static() );
        geode::save_physical_problem( problem, filename );
        const auto reloaded = geode::load_physical_problem( filename );
        check_no_issue( reloaded );
        const auto* fluid =
            dynamic_cast< const geode::SinglePhaseFluid* >( &reloaded.fluid() );
        Exception::test(
            fluid != nullptr && fluid->thermal()
                && fluid->thermal()->specific_heat_capacity == 4180.,
            "[Test] Wrong thermal fluid" );
        Exception::test(
            reloaded.thermal()
                && reloaded.thermal()->reference_temperature == 300.,
            "[Test] Wrong thermal parameters" );
        Exception::test(
            reloaded.materials()[0].thermal
                && reloaded.materials()[0].thermal->thermal_conductivity[1]
                       == 2.5
                && !reloaded.materials()[0].relative_permeability,
            "[Test] Wrong rock thermal properties" );
        Exception::test( reloaded.gravity().value( 2 ) == 0.
                             && !reloaded.schedule().max_time_step,
            "[Test] Wrong gravity or schedule" );
    }

    geode::SaturationTable saturation_table( std::vector< double > values )
    {
        return { { 0., 0.5, 1. }, std::move( values ) };
    }

    // Constructor arguments of a Co2BrineFluid, modified by validation tests
    struct Co2BrineData
    {
        double salinity{ 1. };
        geode::ValidityRange pressure_range{ 1e5, 7.5e7, 1e5 };
        geode::ValidityRange temperature_range{ 285., 395., 5. };
    };

    std::vector< geode::PHASE > co2_brine_phases()
    {
        return { geode::PHASE::gas, geode::PHASE::water };
    }

    std::shared_ptr< geode::MultiphaseFlow > co2_brine_flow(
        const Co2BrineData& data )
    {
        auto fluid =
            std::make_shared< geode::Co2BrineFluid >( co2_brine_phases(),
                std::vector< geode::FluidComponent >{
                    { "co2", 0.044 }, { "water", 0.018 } },
                geode::CO2_BRINE_CORRELATION::ezrokhi, data.salinity,
                data.pressure_range, data.temperature_range,
                std::vector< double >{ 0.1, -1e-4, 1e-7 } );
        auto flow = std::make_shared< geode::MultiphaseFlow >();
        flow->set_fluid( fluid );
        return flow;
    }

    geode::TwoPhaseRelativePermeabilityTable relperm_table(
        geode::PHASE wetting, geode::PHASE non_wetting )
    {
        return { wetting, non_wetting, saturation_table( { 0., 0.2, 1. } ),
            saturation_table( { 0., 0.3, 0.9 } ) };
    }

    std::shared_ptr< geode::TableCapillaryPressure > capillary_table(
        std::vector< geode::PHASE > phases,
        geode::PHASE wetting,
        geode::PHASE non_wetting )
    {
        return std::make_shared< geode::TableCapillaryPressure >(
            std::move( phases ),
            std::vector< geode::TwoPhaseCapillaryPressureTable >{ { wetting,
                non_wetting, saturation_table( { 1e5, 1e4, 0. } ) } } );
    }

    geode::PhysicalProblem co2_brine_problem()
    {
        geode::PhysicalProblem problem;
        geode::PhysicalProblemBuilder builder{ problem };
        builder.set_flow( co2_brine_flow( Co2BrineData{} ) );
        auto relperm = std::make_shared< geode::TableRelativePermeability >(
            co2_brine_phases(),
            std::vector< geode::TwoPhaseRelativePermeabilityTable >{
                relperm_table( geode::PHASE::water, geode::PHASE::gas ) } );
        auto material = reservoir_material( "aquifer", relperm );
        material.capillary_pressure = capillary_table(
            co2_brine_phases(), geode::PHASE::water, geode::PHASE::gas );
        builder.add_material( material );
        builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{}, 2e7, 330.,
                std::vector< double >{ 0., 1. } ) );
        builder.set_schedule( { 0., 1e7, 1e3, 1e6 } );
        return problem;
    }

    void test_co2_brine_roundtrip()
    {
        const auto problem = co2_brine_problem();
        check_no_issue( problem );
        const auto filename = absl::StrCat(
            "co2_brine.", geode::PhysicalProblem::native_extension_static() );
        geode::save_physical_problem( problem, filename );
        const auto reloaded = geode::load_physical_problem( filename );
        check_no_issue( reloaded );
        const auto* fluid =
            dynamic_cast< const geode::Co2BrineFluid* >( &reloaded.fluid() );
        Exception::test(
            fluid != nullptr
                && fluid->correlation() == geode::CO2_BRINE_CORRELATION::ezrokhi
                && fluid->temperature_range().max == 395.
                && fluid->ezrokhi_density_coefficients().size() == 3,
            "[Test] Wrong CO2-brine fluid" );
        const auto& material = reloaded.materials()[0];
        const auto* relperm =
            dynamic_cast< const geode::TableRelativePermeability* >(
                material.relative_permeability.get() );
        Exception::test(
            relperm != nullptr && relperm->tables().size() == 1
                && relperm->tables()[0].non_wetting_kr.values[1] == 0.3,
            "[Test] Wrong table relative permeability" );
        const auto* capillary =
            dynamic_cast< const geode::TableCapillaryPressure* >(
                material.capillary_pressure.get() );
        Exception::test(
            capillary != nullptr
                && capillary->tables()[0].pressure.values[0] == 1e5,
            "[Test] Wrong table capillary pressure" );
    }

    void test_phase_component()
    {
        const auto three_phase = spe10_dead_oil();
        Exception::test(
            geode::phase_component( *three_phase, geode::PHASE::gas ) == 1
                && geode::phase_component( *three_phase, geode::PHASE::water )
                       == 2,
            "[Test] Wrong three-phase components" );
        auto data = spe10_dead_oil_data();
        data.phases = { geode::PHASE::oil, geode::PHASE::water };
        data.components = { { "oil", 0.114 }, { "water", 0.018 } };
        const auto two_phase = dead_oil( data );
        Exception::test(
            geode::phase_component( *two_phase, geode::PHASE::water ) == 1
                && !geode::phase_component( *two_phase, geode::PHASE::gas ),
            "[Test] Wrong two-phase components" );
        data.components.pop_back();
        const auto missing_component = dead_oil( data );
        Exception::test(
            !geode::phase_component( *missing_component, geode::PHASE::oil ),
            "[Test] Phase components need one component per phase" );
    }

    template < typename Modifier >
    void check_modified_problem( geode::PhysicalProblem problem,
        Modifier modifier,
        std::string_view keyword,
        std::string_view context )
    {
        geode::PhysicalProblemBuilder builder{ problem };
        modifier( problem, builder );
        check_issue( problem, keyword, context );
    }

    template < typename Modifier >
    void check_broken_problem(
        Modifier modifier, std::string_view keyword, std::string_view context )
    {
        check_modified_problem(
            dead_oil_problem( geode::uuid{}, geode::uuid{} ), modifier, keyword,
            context );
    }

    template < typename Modifier >
    void check_broken_co2_brine(
        Modifier modifier, std::string_view keyword, std::string_view context )
    {
        check_modified_problem(
            co2_brine_problem(), modifier, keyword, context );
    }

    template < typename Modifier >
    void check_broken_thermal(
        Modifier modifier, std::string_view keyword, std::string_view context )
    {
        check_modified_problem(
            thermal_single_phase_problem(), modifier, keyword, context );
    }

    std::shared_ptr< geode::MultiphaseFlow > modified_dead_oil_flow(
        const std::function< void( DeadOilData& ) >& modifier )
    {
        auto data = spe10_dead_oil_data();
        modifier( data );
        auto flow = std::make_shared< geode::MultiphaseFlow >();
        flow->set_fluid( dead_oil( std::move( data ) ) );
        return flow;
    }

    void test_fluid_validation()
    {
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.hydrocarbon_pvt.pop_back();
                    } ) );
            },
            "PVT table for phase gas", "missing gas PVT" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.hydrocarbon_pvt[0].pressures = { 1e7, 1e5, 5e7 };
                    } ) );
            },
            "strictly increasing", "PVT pressures" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.water_pvt.reset();
                    } ) );
            },
            "water PVT", "missing water PVT" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.components[1].name = "oil";
                    } ) );
            },
            "defined several times", "duplicated component" );
    }

    void test_material_validation()
    {
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.porosity = 1.5;
                builder.set_material( 0, material );
            },
            "porosity", "porosity range" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[1];
                material.name = "top";
                builder.set_material( 1, material );
            },
            "unique", "duplicated material name" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.regions.clear();
                builder.set_material( 0, material );
            },
            "no region", "two default materials" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.relative_permeability.reset();
                builder.set_material( 0, material );
            },
            "needs a relative permeability", "missing relperm" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.relative_permeability =
                    brooks_corey( { 0.5, 0.2, 0.4 } );
                builder.set_material( 0, material );
            },
            "residual saturations", "residual saturation sum" );
    }

    void test_condition_and_schedule_validation()
    {
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.remove_initial_condition( 0 );
                builder.add_initial_condition(
                    std::make_shared< geode::UniformInitialCondition >(
                        std::vector< geode::uuid >{}, 1e7, std::nullopt,
                        std::vector< double >{ 0.5, 0.2, 0.2 } ) );
            },
            "sum to 1", "composition sum" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.remove_initial_condition( 0 );
            },
            "No initial condition", "no initial condition" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_schedule( { 10., 5., 1., std::nullopt } );
            },
            "end time", "schedule order" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                geode::OutputSettings output;
                output.times = { 1e9 };
                builder.set_output( output );
            },
            "Output times", "output time" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                geode::OutputSettings output;
                output.restart_frequency = 0.;
                builder.set_output( output );
            },
            "Restart frequency", "restart frequency" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.add_source_term( { "injector", "brine", 1. } );
            },
            "unknown component brine", "source component" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.add_source_term( { "injector", "water", -1. } );
            },
            "Several source terms", "duplicated source term" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.remove_source_term( 0 );
                builder.add_source_term( { "", "water", 1. } );
            },
            "no well", "source well" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_thermal( geode::ThermalParameters{ 300. } );
            },
            "needs thermal properties", "thermal material" );
    }

    std::shared_ptr< geode::SinglePhaseFluid > water_fluid(
        std::vector< geode::PHASE > phases, double compressibility )
    {
        return std::make_shared< geode::SinglePhaseFluid >( std::move( phases ),
            std::vector< geode::FluidComponent >{ { "water", 0.018 } }, 1000.,
            1e-3, compressibility, 0., 0. );
    }

    std::shared_ptr< geode::SinglePhaseFlow > single_phase_flow(
        std::vector< geode::PHASE > phases, double compressibility )
    {
        auto flow = std::make_shared< geode::SinglePhaseFlow >();
        flow->set_fluid( water_fluid( std::move( phases ), compressibility ) );
        return flow;
    }

    void test_flow_fluid_pairing()
    {
        check_broken_problem(
            []( auto&, auto& builder ) {
                auto flow = std::make_shared< geode::MultiphaseFlow >();
                flow->set_fluid( water_fluid(
                    { geode::PHASE::water, geode::PHASE::oil }, 0. ) );
                builder.set_flow( flow );
            },
            "not a SinglePhaseFluid",
            "multiphase flow with single phase fluid" );
    }

    void test_generic_rules()
    {
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow( single_phase_flow(
                    { geode::PHASE::water, geode::PHASE::oil }, 0. ) );
            },
            "SinglePhaseFluid needs exactly 1 phase", "two phase fluid" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    single_phase_flow( { geode::PHASE::water }, -1. ) );
            },
            "compressibility must not be negative",
            "negative compressibility" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                auto flow = std::make_shared< geode::SinglePhaseFlow >();
                auto data = spe10_dead_oil_data();
                data.phases = { geode::PHASE::oil };
                data.components = { { "oil", 0.114 } };
                data.surface_densities = { 800. };
                data.hydrocarbon_pvt.pop_back();
                data.water_pvt.reset();
                flow->set_fluid( dead_oil( std::move( data ) ) );
                builder.set_flow( flow );
            },
            "DeadOilFluid needs 2 or 3 phases", "single phase dead oil" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.hydrocarbon_pvt[0].viscosities[1] = 0.;
                    } ) );
            },
            "viscosities must be positive", "PVT viscosity" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.water_pvt->viscosity = -1.;
                    } ) );
            },
            "water PVT formation volume factor", "water PVT viscosity" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                builder.remove_material( 1 );
                builder.remove_material( 0 );
                static_cast< void >( problem );
            },
            "No material", "no material" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.reference_pressure =
                    std::numeric_limits< double >::infinity();
                builder.set_material( 0, material );
            },
            "reference pressure must be finite",
            "material reference pressure" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.porosity = std::numeric_limits< double >::quiet_NaN();
                builder.set_material( 0, material );
            },
            "porosity", "NaN porosity" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.relative_permeability =
                    std::make_shared< geode::BrooksCoreyRelativePermeability >(
                        std::vector< geode::PHASE >{ geode::PHASE::oil,
                            geode::PHASE::gas, geode::PHASE::water },
                        std::vector< double >{ 0.1, 0., 0.2 },
                        std::vector< double >{ 2., 0., 2. },
                        std::vector< double >{ 1., 1., 1. } );
                builder.set_material( 0, material );
            },
            "exponents must be positive", "Brooks-Corey exponent" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.remove_initial_condition( 0 );
                builder.add_initial_condition(
                    std::make_shared< geode::UniformInitialCondition >(
                        std::vector< geode::uuid >{},
                        std::numeric_limits< double >::quiet_NaN(), 300.,
                        std::vector< double >{ 0.9995, 0., 0.0005 } ) );
            },
            "Initial condition #0 pressure must be finite", "NaN pressure" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_gravity( geode::Vector3D{
                    { 0., 0., std::numeric_limits< double >::infinity() } } );
            },
            "Gravity must be finite", "infinite gravity" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_schedule(
                    { 0., 10., std::numeric_limits< double >::quiet_NaN(),
                        std::nullopt } );
            },
            "initial time step must be positive", "NaN time step" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.hydrocarbon_pvt[0].pressures[1] =
                            std::numeric_limits< double >::quiet_NaN();
                    } ) );
            },
            "strictly increasing", "NaN PVT pressure" );
        check_broken_problem(
            []( auto&, auto& builder ) {
                builder.set_flow(
                    modified_dead_oil_flow( []( DeadOilData& fluid ) {
                        fluid.components[0].molar_weight =
                            std::numeric_limits< double >::quiet_NaN();
                    } ) );
            },
            "molar weight must be finite", "NaN molar weight" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.capillary_pressure =
                    std::make_shared< geode::TableCapillaryPressure >(
                        std::vector< geode::PHASE >{
                            geode::PHASE::water, geode::PHASE::oil },
                        std::vector< geode::TwoPhaseCapillaryPressureTable >{
                            { geode::PHASE::water, geode::PHASE::oil,
                                saturation_table( { 1e5,
                                    std::numeric_limits< double >::infinity(),
                                    0. } ) } } );
                builder.set_material( 0, material );
            },
            "capillary pressure values must be finite",
            "infinite capillary pressure" );
    }

    void test_co2_brine_validation()
    {
        const auto nan = std::numeric_limits< double >::quiet_NaN();
        for( const auto salinity : { -1., nan } )
        {
            check_broken_co2_brine(
                [salinity]( auto&, auto& builder ) {
                    Co2BrineData data;
                    data.salinity = salinity;
                    builder.set_flow( co2_brine_flow( data ) );
                },
                "Co2BrineFluid salinity must not be negative",
                absl::StrCat( "salinity ", salinity ) );
        }
        for( const auto& range : { geode::ValidityRange{ 7.5e7, 1e5, 1e5 },
                 geode::ValidityRange{ 1e5, 1e5, 1e5 },
                 geode::ValidityRange{ nan, 7.5e7, 1e5 } } )
        {
            check_broken_co2_brine(
                [&range]( auto&, auto& builder ) {
                    Co2BrineData data;
                    data.pressure_range = range;
                    builder.set_flow( co2_brine_flow( data ) );
                },
                "Co2BrineFluid pressure range needs min < max and a positive "
                "step",
                "pressure range bounds" );
        }
        for( const auto step : { 0., -5., nan } )
        {
            check_broken_co2_brine(
                [step]( auto&, auto& builder ) {
                    Co2BrineData data;
                    data.temperature_range.step = step;
                    builder.set_flow( co2_brine_flow( data ) );
                },
                "Co2BrineFluid temperature range needs min < max and a "
                "positive step",
                absl::StrCat( "temperature range step ", step ) );
        }
    }

    [[nodiscard]] std::shared_ptr< geode::BrooksCoreyCapillaryPressure >
        brooks_corey_capillary( std::vector< double > entry_pressures,
            std::vector< double > indices = { 2., 2., 2. } )
    {
        return std::make_shared< geode::BrooksCoreyCapillaryPressure >(
            std::vector< geode::PHASE >{
                geode::PHASE::oil, geode::PHASE::gas, geode::PHASE::water },
            std::vector< double >{ 0.1, 0., 0.2 }, std::move( entry_pressures ),
            std::move( indices ) );
    }

    void test_brooks_corey_capillary_validation()
    {
        auto problem = dead_oil_problem( geode::uuid{}, geode::uuid{} );
        auto material = problem.materials()[0];
        material.capillary_pressure = brooks_corey_capillary( { 0., 1e4, 0. } );
        geode::PhysicalProblemBuilder{ problem }.set_material( 0, material );
        check_no_issue( problem );

        const auto nan = std::numeric_limits< double >::quiet_NaN();
        for( const auto entry_pressure :
            { -1., nan, std::numeric_limits< double >::infinity() } )
        {
            check_broken_problem(
                [entry_pressure]( const auto& broken, auto& builder ) {
                    auto broken_material = broken.materials()[0];
                    broken_material.capillary_pressure =
                        brooks_corey_capillary( { 0., entry_pressure, 0. } );
                    builder.set_material( 0, broken_material );
                },
                "capillary entry pressures must be finite and not negative",
                absl::StrCat( "entry pressure ", entry_pressure ) );
        }
        check_broken_problem(
            []( const auto& broken, auto& builder ) {
                auto broken_material = broken.materials()[0];
                broken_material.capillary_pressure =
                    brooks_corey_capillary( { 0., 1e4, 0. }, { 2., 0., 2. } );
                builder.set_material( 0, broken_material );
            },
            "pore size distribution indices must be positive",
            "pore size distribution index" );
        check_broken_problem(
            []( const auto& broken, auto& builder ) {
                auto broken_material = broken.materials()[0];
                broken_material.capillary_pressure =
                    brooks_corey_capillary( { 0., 1e4 } );
                builder.set_material( 0, broken_material );
            },
            "Brooks-Corey capillary pressure needs one value per fluid phase",
            "missing entry pressure" );
    }

    void test_table_saturation_validation()
    {
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.relative_permeability =
                    std::make_shared< geode::TableRelativePermeability >(
                        std::vector< geode::PHASE >{ geode::PHASE::oil,
                            geode::PHASE::gas, geode::PHASE::water },
                        std::vector< geode::TwoPhaseRelativePermeabilityTable >{
                            relperm_table(
                                geode::PHASE::water, geode::PHASE::oil ) } );
                builder.set_material( 0, material );
            },
            "table relative permeability needs 2 tables for 3 phases",
            "three phases with one relperm table" );
        check_broken_co2_brine(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.relative_permeability =
                    std::make_shared< geode::TableRelativePermeability >(
                        co2_brine_phases(),
                        std::vector< geode::TwoPhaseRelativePermeabilityTable >{
                            relperm_table(
                                geode::PHASE::water, geode::PHASE::gas ),
                            relperm_table(
                                geode::PHASE::water, geode::PHASE::gas ) } );
                builder.set_material( 0, material );
            },
            "table relative permeability needs 1 tables for 2 phases",
            "two phases with two relperm tables" );
        check_broken_co2_brine(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.relative_permeability = std::make_shared<
                    geode::TableRelativePermeability >( co2_brine_phases(),
                    std::vector< geode::TwoPhaseRelativePermeabilityTable >{} );
                builder.set_material( 0, material );
            },
            "table relative permeability has no table", "no relperm table" );

        const std::array< std::array< geode::PHASE, 2 >, 3 > wrong_phases{ {
            { geode::PHASE::water, geode::PHASE::water },
            { geode::PHASE::water, geode::PHASE::oil },
            { geode::PHASE::oil, geode::PHASE::gas },
        } };
        for( const auto& phases : wrong_phases )
        {
            check_broken_co2_brine(
                [&phases]( const auto& problem, auto& builder ) {
                    auto material = problem.materials()[0];
                    material.capillary_pressure = capillary_table(
                        co2_brine_phases(), phases[0], phases[1] );
                    builder.set_material( 0, material );
                },
                "table phases must be two different fluid phases",
                absl::StrCat( "capillary table phases ",
                    geode::phase_name( phases[0] ), "-",
                    geode::phase_name( phases[1] ) ) );
        }
        check_broken_co2_brine(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.capillary_pressure =
                    capillary_table( { geode::PHASE::water, geode::PHASE::oil },
                        geode::PHASE::water, geode::PHASE::gas );
                builder.set_material( 0, material );
            },
            "capillary pressure phases must be the fluid phases",
            "capillary phases" );
        check_broken_problem(
            []( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.relative_permeability =
                    std::make_shared< geode::TableRelativePermeability >(
                        std::vector< geode::PHASE >{ geode::PHASE::oil,
                            geode::PHASE::gas, geode::PHASE::water },
                        std::vector< geode::TwoPhaseRelativePermeabilityTable >{
                            relperm_table(
                                geode::PHASE::water, geode::PHASE::oil ),
                            relperm_table(
                                geode::PHASE::oil, geode::PHASE::water ) } );
                builder.set_material( 0, material );
            },
            "relative permeability has several tables for the same two phases",
            "two water-oil relperm tables" );
        const auto pressure_table = []( geode::PHASE wetting,
                                        geode::PHASE non_wetting ) {
            return geode::TwoPhaseCapillaryPressureTable{ wetting, non_wetting,
                saturation_table( { 1e5, 1e4, 0. } ) };
        };
        check_broken_problem(
            [&pressure_table]( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.capillary_pressure =
                    std::make_shared< geode::TableCapillaryPressure >(
                        std::vector< geode::PHASE >{ geode::PHASE::oil,
                            geode::PHASE::gas, geode::PHASE::water },
                        std::vector< geode::TwoPhaseCapillaryPressureTable >{
                            pressure_table(
                                geode::PHASE::water, geode::PHASE::oil ),
                            pressure_table(
                                geode::PHASE::oil, geode::PHASE::gas ),
                            pressure_table(
                                geode::PHASE::water, geode::PHASE::gas ) } );
                builder.set_material( 0, material );
            },
            "table capillary pressure needs at most 2 tables for 3 phases",
            "three capillary tables" );
        check_broken_problem(
            [&pressure_table]( const auto& problem, auto& builder ) {
                auto material = problem.materials()[0];
                material.capillary_pressure =
                    std::make_shared< geode::TableCapillaryPressure >(
                        std::vector< geode::PHASE >{ geode::PHASE::oil,
                            geode::PHASE::gas, geode::PHASE::water },
                        std::vector< geode::TwoPhaseCapillaryPressureTable >{
                            pressure_table(
                                geode::PHASE::water, geode::PHASE::oil ),
                            pressure_table(
                                geode::PHASE::water, geode::PHASE::oil ) } );
                builder.set_material( 0, material );
            },
            "capillary pressure has several tables for the same two phases",
            "two water-oil capillary tables" );
    }

    void test_thermal_validation()
    {
        const auto nan = std::numeric_limits< double >::quiet_NaN();
        for( const auto heat_capacity : { 0., -1., nan } )
        {
            check_broken_thermal(
                [heat_capacity]( const auto& problem, auto& builder ) {
                    auto material = problem.materials()[0];
                    material.thermal->volumetric_heat_capacity = heat_capacity;
                    builder.set_material( 0, material );
                },
                "heat capacity and thermal conductivity must be positive",
                absl::StrCat( "heat capacity ", heat_capacity ) );
        }
        for( const auto conductivity : { 0., -2.5, nan } )
        {
            check_broken_thermal(
                [conductivity]( const auto& problem, auto& builder ) {
                    auto material = problem.materials()[0];
                    material.thermal->thermal_conductivity[1] = conductivity;
                    builder.set_material( 0, material );
                },
                "heat capacity and thermal conductivity must be positive",
                absl::StrCat( "thermal conductivity ", conductivity ) );
        }
        check_broken_thermal(
            []( auto&, auto& builder ) {
                builder.set_flow( thermal_water_flow( std::nullopt ) );
            },
            "SinglePhaseFluid needs thermal properties for a thermal problem",
            "fluid without thermal properties" );
        for( const auto specific_heat : { 0., nan } )
        {
            check_broken_thermal(
                [specific_heat]( auto&, auto& builder ) {
                    builder.set_flow(
                        thermal_water_flow( geode::FluidThermalProperties{
                            specific_heat, 2e-4, 0. } ) );
                },
                "SinglePhaseFluid specific heat capacity must be positive",
                absl::StrCat( "fluid specific heat ", specific_heat ) );
        }
        const auto infinity = std::numeric_limits< double >::infinity();
        for( const auto& thermal :
            { geode::FluidThermalProperties{ 4180., infinity, 0. },
                geode::FluidThermalProperties{ 4180., 2e-4, nan } } )
        {
            check_broken_thermal(
                [&thermal]( auto&, auto& builder ) {
                    builder.set_flow( thermal_water_flow( thermal ) );
                },
                "SinglePhaseFluid thermal expansion and viscosity temperature "
                "coefficient must be finite",
                "non-finite fluid thermal coefficient" );
        }
    }

    class ExternalFlow : public geode::FlowModel
    {
    public:
        [[nodiscard]] std::string_view type_name() const override
        {
            return "ExternalFlow";
        }
    };

    void test_external_flow()
    {
        auto problem = dead_oil_problem( geode::uuid{}, geode::uuid{} );
        geode::PhysicalProblemBuilder builder{ problem };
        auto flow = std::make_shared< ExternalFlow >();
        flow->set_fluid( dead_oil( spe10_dead_oil_data() ) );
        builder.set_flow( flow );
        const auto issues = geode::validate_physical_problem( problem );
        Exception::test( std::none_of( issues.begin(), issues.end(),
                             []( const auto& issue ) {
                                 return absl::StrContainsIgnoreCase(
                                     issue, "single phase" );
                             } ),
            "[Test] A flow from another library is not single phase: ",
            absl::StrJoin( issues, " | " ) );
    }

    void test_exhaustive_validation()
    {
        geode::PhysicalProblem problem;
        geode::PhysicalProblemBuilder builder{ problem };
        geode::Material material;
        material.name = "rock";
        material.porosity = 2.;
        builder.add_material( material );
        builder.set_schedule( { 10., 0., 1., std::nullopt } );
        for( const auto* keyword : { "No flow model", "porosity",
                 "No initial condition", "Schedule end time" } )
        {
            check_issue( problem, keyword, "exhaustive report" );
        }
    }

    void test_region_validation()
    {
        geode::BRep model;
        geode::BRepBuilder model_builder{ model };
        const auto& block_id = model_builder.add_block();
        const auto& collection_id = model_builder.add_block_collection();
        model_builder.add_block_in_block_collection(
            model.block( block_id ), model.block_collection( collection_id ) );

        auto problem = dead_oil_problem( model.id(), block_id );
        Exception::test(
            geode::validate_physical_problem_regions( problem, model ).empty(),
            "[Test] Block region should be valid" );
        geode::PhysicalProblemBuilder builder{ problem };
        auto material = problem.materials()[0];
        material.regions = { collection_id };
        builder.set_material( 0, material );
        Exception::test(
            geode::validate_physical_problem_regions( problem, model ).empty(),
            "[Test] Collection region should be valid" );
        auto overlapping = problem.materials()[1];
        overlapping.regions = { block_id };
        builder.set_material( 1, overlapping );
        const auto overlap_issues =
            geode::validate_physical_problem_regions( problem, model );
        Exception::test(
            overlap_issues.size() == 1
                && absl::StrContains( overlap_issues.front(), "is covered by" ),
            "[Test] Block covered by a collection and by another material "
            "should be reported" );
        overlapping.regions.clear();
        builder.set_material( 1, overlapping );
        material.regions = { model_builder.add_surface() };
        builder.set_material( 0, material );
        Exception::test(
            geode::validate_physical_problem_regions( problem, model ).size()
                == 1,
            "[Test] Surface region should be reported" );
        material.regions = { geode::uuid{} };
        builder.set_material( 0, material );
        Exception::test(
            geode::validate_physical_problem_regions( problem, model ).size()
                == 1,
            "[Test] Unknown region should be reported" );
        builder.set_model_id( geode::uuid{} );
        Exception::test(
            geode::validate_physical_problem_regions( problem, model ).size()
                == 2,
            "[Test] Wrong model id should be reported" );
        builder.set_model_id( model.id() );
        material.regions = { block_id };
        builder.set_material( 0, material );
        builder.remove_material( 1 );
        const auto& other_block = model_builder.add_block();
        const auto coverage_issues =
            geode::validate_physical_problem_regions( problem, model );
        Exception::test( coverage_issues.size() == 1
                             && absl::StrContains(
                                 coverage_issues.front(), other_block.string() )
                             && absl::StrContains(
                                 coverage_issues.front(), "has no material" ),
            "[Test] Block without material should be reported: ",
            absl::StrJoin( coverage_issues, " | " ) );

        builder.add_material( reservoir_material( "default", brooks_corey() ) );
        builder.remove_initial_condition( 0 );
        builder.add_initial_condition(
            std::make_shared< geode::UniformInitialCondition >(
                std::vector< geode::uuid >{ block_id }, 4.1369e7, 300.,
                std::vector< double >{ 0.9995, 0., 0.0005 } ) );
        const auto condition_issues =
            geode::validate_physical_problem_regions( problem, model );
        Exception::test( condition_issues.size() == 1
                             && absl::StrContains( condition_issues.front(),
                                 other_block.string() )
                             && absl::StrContains( condition_issues.front(),
                                 "has no initial condition" ),
            "[Test] Block without initial condition should be reported: ",
            absl::StrJoin( condition_issues, " | " ) );
    }

    class UnregisteredFluid final : public geode::FluidModel
    {
    public:
        [[nodiscard]] std::string_view type_name() const final
        {
            return "UnregisteredFluid";
        }
    };

    void test_unregistered_type()
    {
        auto problem = dead_oil_problem( geode::uuid{}, geode::uuid{} );
        auto flow = std::make_shared< geode::MultiphaseFlow >();
        flow->set_fluid( std::make_shared< UnregisteredFluid >() );
        geode::PhysicalProblemBuilder{ problem }.set_flow( flow );
        const auto filename = absl::StrCat( "unregistered.",
            geode::PhysicalProblem::native_extension_static() );
        bool has_thrown{ false };
        try
        {
            geode::save_physical_problem( problem, filename );
        }
        catch( const geode::OpenGeodeException& )
        {
            has_thrown = true;
        }
        Exception::test(
            has_thrown, "[Test] Saving an unregistered type should throw" );
        Exception::test( !std::filesystem::exists( filename ),
            "[Test] No partial file should remain" );
    }
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesSimulationLibrary::initialize();
        geode::OpenGeodeModelLibrary::initialize();
        test_dead_oil_roundtrip();
        test_thermal_single_phase_roundtrip();
        test_co2_brine_roundtrip();
        test_phase_component();
        test_fluid_validation();
        test_material_validation();
        test_condition_and_schedule_validation();
        test_flow_fluid_pairing();
        test_generic_rules();
        test_co2_brine_validation();
        test_brooks_corey_capillary_validation();
        test_table_saturation_validation();
        test_thermal_validation();
        test_external_flow();
        test_exhaustive_validation();
        test_region_validation();
        test_unregistered_type();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
