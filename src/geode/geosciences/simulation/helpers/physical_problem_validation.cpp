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

#include <geode/geosciences/simulation/helpers/physical_problem_validation.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <optional>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/str_cat.h>
#include <absl/types/span.h>

#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/saturation_functions.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/source_term.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

namespace
{
    constexpr double COMPOSITION_TOLERANCE{ 1e-6 };

    bool is_strictly_increasing( absl::Span< const double > values )
    {
        return std::adjacent_find( values.begin(), values.end(),
                   []( double previous, double next ) {
                       return next <= previous;
                   } )
               == values.end();
    }

    bool are_in_range(
        absl::Span< const double > values, double min, double max )
    {
        return std::all_of(
            values.begin(), values.end(), [min, max]( double value ) {
                return value >= min && value <= max;
            } );
    }

    bool has_phase(
        absl::Span< const geode::PHASE > phases, geode::PHASE phase )
    {
        return absl::c_linear_search( phases, phase );
    }

    bool same_phase_set( absl::Span< const geode::PHASE > phases,
        absl::Span< const geode::PHASE > reference )
    {
        if( phases.size() != reference.size() )
        {
            return false;
        }
        return std::all_of(
            reference.begin(), reference.end(), [&phases]( auto phase ) {
                return has_phase( phases, phase );
            } );
    }

    class ProblemValidator
    {
    public:
        explicit ProblemValidator( const geode::PhysicalProblem& problem )
            : problem_( problem )
        {
        }

        std::vector< std::string > run()
        {
            if( check_flow() )
            {
                check_fluid();
                check_materials();
                check_initial_conditions();
                check_source_terms();
                check_thermal();
            }
            check_schedule();
            check_output();
            return std::move( issues_ );
        }

    private:
        template < typename... Args >
        void add_issue( const Args&... args )
        {
            issues_.emplace_back( absl::StrCat( args... ) );
        }

        bool is_multiphase() const
        {
            return dynamic_cast< const geode::MultiphaseFlow* >(
                       &problem_.flow() )
                   != nullptr;
        }

        const geode::FluidModel& fluid() const
        {
            return problem_.fluid();
        }

        bool check_flow()
        {
            if( !problem_.has_flow() )
            {
                add_issue( "No flow model is defined" );
                return false;
            }
            if( !problem_.flow().has_fluid() )
            {
                add_issue( "The flow model has no fluid" );
                return false;
            }
            const auto nb_phases = fluid().phases().size();
            if( is_multiphase() && nb_phases < 2 )
            {
                add_issue( "Multiphase flow needs at least 2 fluid phases" );
            }
            if( dynamic_cast< const geode::SinglePhaseFlow* >(
                    &problem_.flow() )
                && nb_phases != 1 )
            {
                add_issue( "Single phase flow needs exactly 1 fluid phase" );
            }
            return true;
        }

        void check_fluid()
        {
            check_fluid_phases();
            check_fluid_components();
            if( const auto* single_phase =
                    dynamic_cast< const geode::SinglePhaseFluid* >( &fluid() ) )
            {
                check_single_phase_fluid( *single_phase );
            }
            else if( const auto* dead_oil =
                         dynamic_cast< const geode::DeadOilFluid* >(
                             &fluid() ) )
            {
                check_dead_oil_fluid( *dead_oil );
            }
            else if( const auto* co2_brine =
                         dynamic_cast< const geode::Co2BrineFluid* >(
                             &fluid() ) )
            {
                check_co2_brine_fluid( *co2_brine );
            }
        }

        void check_fluid_phases()
        {
            const auto& phases = fluid().phases();
            for( const auto phase : phases )
            {
                if( std::count( phases.begin(), phases.end(), phase ) > 1 )
                {
                    add_issue( "Fluid phase ", geode::phase_name( phase ),
                        " is defined several times" );
                    return;
                }
            }
        }

        void check_fluid_components()
        {
            absl::flat_hash_set< std::string > names;
            for( const auto& component : fluid().components() )
            {
                if( component.name.empty() )
                {
                    add_issue( "A fluid component has no name" );
                }
                else if( !names.insert( component.name ).second )
                {
                    add_issue( "Fluid component ", component.name,
                        " is defined several times" );
                }
                if( component.molar_weight < 0 )
                {
                    add_issue( "Fluid component ", component.name,
                        " has a negative molar weight" );
                }
            }
        }

        void check_single_phase_fluid( const geode::SinglePhaseFluid& fluid )
        {
            if( fluid.reference_density() <= 0 )
            {
                add_issue( "SinglePhaseFluid density must be positive" );
            }
            if( fluid.reference_viscosity() <= 0 )
            {
                add_issue( "SinglePhaseFluid viscosity must be positive" );
            }
            if( fluid.components().size() != 1 )
            {
                add_issue( "SinglePhaseFluid needs exactly 1 component" );
            }
        }

        void check_dead_oil_fluid( const geode::DeadOilFluid& fluid )
        {
            const auto nb_phases = fluid.phases().size();
            if( fluid.components().size() != nb_phases )
            {
                add_issue( "DeadOilFluid needs one component per phase" );
            }
            if( fluid.surface_densities().size() != nb_phases
                || !are_in_range( fluid.surface_densities(),
                    std::numeric_limits< double >::min(),
                    std::numeric_limits< double >::max() ) )
            {
                add_issue( "DeadOilFluid needs one positive surface density "
                           "per phase" );
            }
            check_dead_oil_tables( fluid );
            if( has_phase( fluid.phases(), geode::PHASE::water )
                != fluid.water_pvt().has_value() )
            {
                add_issue( "DeadOilFluid water PVT must be given if and only "
                           "if water is a phase" );
            }
        }

        void check_dead_oil_tables( const geode::DeadOilFluid& fluid )
        {
            for( const auto phase : { geode::PHASE::oil, geode::PHASE::gas } )
            {
                const auto nb_tables =
                    std::count_if( fluid.hydrocarbon_pvt().begin(),
                        fluid.hydrocarbon_pvt().end(),
                        [phase]( const auto& table ) {
                            return table.phase == phase;
                        } );
                const auto expected =
                    has_phase( fluid.phases(), phase ) ? 1 : 0;
                if( nb_tables != expected )
                {
                    add_issue( "DeadOilFluid needs ", expected,
                        " PVT table for phase ", geode::phase_name( phase ),
                        ", found ", nb_tables );
                }
            }
            for( const auto& table : fluid.hydrocarbon_pvt() )
            {
                check_pvt_table( table );
            }
        }

        void check_pvt_table( const geode::HydrocarbonPvtTable& table )
        {
            const auto name = geode::phase_name( table.phase );
            if( table.phase == geode::PHASE::water )
            {
                add_issue( "DeadOilFluid PVT tables are for oil and gas only" );
            }
            if( table.pressures.size() < 2
                || table.formation_volume_factors.size()
                       != table.pressures.size()
                || table.viscosities.size() != table.pressures.size() )
            {
                add_issue( "DeadOilFluid ", name,
                    " PVT table needs at least 2 rows of equal size" );
            }
            if( !is_strictly_increasing( table.pressures ) )
            {
                add_issue( "DeadOilFluid ", name,
                    " PVT pressures must be strictly increasing" );
            }
        }

        void check_co2_brine_fluid( const geode::Co2BrineFluid& fluid )
        {
            const std::array< geode::PHASE, 2 > expected{ geode::PHASE::gas,
                geode::PHASE::water };
            if( !same_phase_set( fluid.phases(), expected )
                || fluid.components().size() != 2 )
            {
                add_issue( "Co2BrineFluid needs phases gas and water and 2 "
                           "components" );
            }
            check_validity_range( fluid.pressure_range(), "pressure" );
            check_validity_range( fluid.temperature_range(), "temperature" );
        }

        void check_validity_range(
            const geode::ValidityRange& range, std::string_view name )
        {
            if( range.min >= range.max || range.step <= 0 )
            {
                add_issue( "Co2BrineFluid ", name,
                    " range needs min < max and a positive step" );
            }
        }

        void check_composition(
            absl::Span< const double > composition, std::string_view context )
        {
            if( composition.size() != fluid().components().size() )
            {
                add_issue( context, " composition needs one value per fluid "
                                    "component" );
                return;
            }
            const auto sum =
                std::accumulate( composition.begin(), composition.end(), 0. );
            if( !are_in_range( composition, 0, 1 )
                || std::fabs( sum - 1 ) > COMPOSITION_TOLERANCE )
            {
                add_issue( context,
                    " composition values must be in [0, 1] and sum to 1" );
            }
        }

        void check_materials()
        {
            absl::flat_hash_set< std::string > names;
            absl::flat_hash_set< geode::uuid > regions;
            geode::index_t nb_defaults{ 0 };
            for( const auto& material : problem_.materials() )
            {
                if( material.name.empty()
                    || !names.insert( material.name ).second )
                {
                    add_issue( "Material names must be unique and not empty (",
                        material.name, ")" );
                }
                nb_defaults += material.regions.empty() ? 1 : 0;
                check_unique_regions( material.regions, regions, "Material" );
                check_material_values( material );
                check_saturation_functions( material );
            }
            if( nb_defaults > 1 )
            {
                add_issue( "At most one material can have no region" );
            }
        }

        void check_unique_regions( absl::Span< const geode::uuid > regions,
            absl::flat_hash_set< geode::uuid >& known_regions,
            std::string_view context )
        {
            for( const auto& region : regions )
            {
                if( !known_regions.insert( region ).second )
                {
                    add_issue( context, " region ", region.string(),
                        " is assigned several times" );
                }
            }
        }

        void check_material_values( const geode::Material& material )
        {
            if( material.pore_compressibility < 0 )
            {
                add_issue( "Material ", material.name,
                    " pore compressibility must not be negative" );
            }
            if( material.porosity
                && ( material.porosity.value() <= 0
                     || material.porosity.value() > 1 ) )
            {
                add_issue(
                    "Material ", material.name, " porosity must be in (0, 1]" );
            }
            if( material.permeability
                && !are_in_range( material.permeability.value(),
                    std::numeric_limits< double >::min(),
                    std::numeric_limits< double >::max() ) )
            {
                add_issue( "Material ", material.name,
                    " permeability must be positive" );
            }
        }

        void check_saturation_functions( const geode::Material& material )
        {
            if( material.relative_permeability )
            {
                check_relative_permeability(
                    *material.relative_permeability, material.name );
            }
            else if( is_multiphase() )
            {
                add_issue( "Material ", material.name,
                    " needs a relative permeability for multiphase flow" );
            }
            if( material.capillary_pressure )
            {
                check_capillary_pressure(
                    *material.capillary_pressure, material.name );
            }
        }

        void check_relative_permeability(
            const geode::RelativePermeability& relperm,
            std::string_view material )
        {
            if( !same_phase_set( relperm.phases(), fluid().phases() ) )
            {
                add_issue( "Material ", material,
                    " relative permeability phases must be the fluid phases" );
            }
            if( const auto* brooks_corey = dynamic_cast<
                    const geode::BrooksCoreyRelativePermeability* >(
                    &relperm ) )
            {
                check_brooks_corey_relperm( *brooks_corey, material );
            }
            else if( const auto* table = dynamic_cast<
                         const geode::TableRelativePermeability* >( &relperm ) )
            {
                check_table_relperm( *table, material );
            }
        }

        void check_brooks_corey_relperm(
            const geode::BrooksCoreyRelativePermeability& relperm,
            std::string_view material )
        {
            const auto nb_phases = relperm.phases().size();
            if( relperm.residual_saturations().size() != nb_phases
                || relperm.exponents().size() != nb_phases
                || relperm.max_values().size() != nb_phases )
            {
                add_issue( "Material ", material,
                    " Brooks-Corey relative permeability needs one value per "
                    "phase" );
                return;
            }
            check_residual_saturations(
                relperm.residual_saturations(), material );
            if( !are_in_range( relperm.max_values(), 0, 1 ) )
            {
                add_issue( "Material ", material,
                    " relative permeability maximum values must be in [0, "
                    "1]" );
            }
        }

        void check_residual_saturations(
            absl::Span< const double > saturations, std::string_view material )
        {
            const auto sum =
                std::accumulate( saturations.begin(), saturations.end(), 0. );
            if( !are_in_range( saturations, 0, 1 ) || sum >= 1 )
            {
                add_issue( "Material ", material,
                    " residual saturations must be in [0, 1) with a sum "
                    "below 1" );
            }
        }

        void check_table_relperm(
            const geode::TableRelativePermeability& relperm,
            std::string_view material )
        {
            if( relperm.tables().empty() )
            {
                add_issue( "Material ", material,
                    " table relative permeability has no table" );
            }
            for( const auto& table : relperm.tables() )
            {
                check_table_phases(
                    table.wetting_phase, table.non_wetting_phase, material );
                check_saturation_table( table.wetting_kr, material, true );
                check_saturation_table( table.non_wetting_kr, material, true );
            }
        }

        void check_table_phases( geode::PHASE wetting,
            geode::PHASE non_wetting,
            std::string_view material )
        {
            if( wetting == non_wetting
                || !has_phase( fluid().phases(), wetting )
                || !has_phase( fluid().phases(), non_wetting ) )
            {
                add_issue( "Material ", material,
                    " table phases must be two different fluid phases" );
            }
        }

        void check_saturation_table( const geode::SaturationTable& table,
            std::string_view material,
            bool is_relperm )
        {
            if( table.saturations.size() < 2
                || table.values.size() != table.saturations.size() )
            {
                add_issue( "Material ", material,
                    " saturation table needs at least 2 points of equal "
                    "size" );
                return;
            }
            if( !is_strictly_increasing( table.saturations )
                || !are_in_range( table.saturations, 0, 1 ) )
            {
                add_issue( "Material ", material,
                    " table saturations must be strictly increasing in [0, "
                    "1]" );
            }
            if( is_relperm && !are_in_range( table.values, 0, 1 ) )
            {
                add_issue( "Material ", material,
                    " relative permeability values must be in [0, 1]" );
            }
        }

        void check_capillary_pressure(
            const geode::CapillaryPressure& capillary,
            std::string_view material )
        {
            if( const auto* brooks_corey =
                    dynamic_cast< const geode::BrooksCoreyCapillaryPressure* >(
                        &capillary ) )
            {
                check_brooks_corey_capillary( *brooks_corey, material );
            }
            else if( const auto* table =
                         dynamic_cast< const geode::TableCapillaryPressure* >(
                             &capillary ) )
            {
                check_table_capillary( *table, material );
            }
        }

        void check_brooks_corey_capillary(
            const geode::BrooksCoreyCapillaryPressure& capillary,
            std::string_view material )
        {
            const auto nb_phases = capillary.phases().size();
            if( !same_phase_set( capillary.phases(), fluid().phases() )
                || capillary.residual_saturations().size() != nb_phases
                || capillary.entry_pressures().size() != nb_phases
                || capillary.pore_size_distribution_indices().size()
                       != nb_phases )
            {
                add_issue( "Material ", material,
                    " Brooks-Corey capillary pressure needs one value per "
                    "fluid phase" );
                return;
            }
            check_residual_saturations(
                capillary.residual_saturations(), material );
            if( !are_in_range( capillary.pore_size_distribution_indices(),
                    std::numeric_limits< double >::min(),
                    std::numeric_limits< double >::max() ) )
            {
                add_issue( "Material ", material,
                    " pore size distribution indices must be positive" );
            }
        }

        void check_table_capillary(
            const geode::TableCapillaryPressure& capillary,
            std::string_view material )
        {
            if( capillary.tables().empty() )
            {
                add_issue( "Material ", material,
                    " table capillary pressure has no table" );
            }
            for( const auto& table : capillary.tables() )
            {
                check_table_phases(
                    table.wetting_phase, table.non_wetting_phase, material );
                check_saturation_table( table.pressure, material, false );
            }
        }

        void check_initial_conditions()
        {
            if( problem_.nb_initial_conditions() == 0 )
            {
                add_issue( "No initial condition is defined" );
                return;
            }
            absl::flat_hash_set< geode::uuid > regions;
            geode::index_t nb_defaults{ 0 };
            for( const auto condition_id :
                geode::Range{ problem_.nb_initial_conditions() } )
            {
                const auto& condition =
                    problem_.initial_condition( condition_id );
                nb_defaults += condition.regions().empty() ? 1 : 0;
                check_unique_regions(
                    condition.regions(), regions, "Initial condition" );
                if( const auto* uniform =
                        dynamic_cast< const geode::UniformInitialCondition* >(
                            &condition ) )
                {
                    check_uniform_condition( *uniform );
                }
            }
            if( nb_defaults > 1 )
            {
                add_issue( "At most one initial condition can have no region" );
            }
        }

        void check_uniform_condition(
            const geode::UniformInitialCondition& condition )
        {
            if( is_multiphase() )
            {
                check_composition(
                    condition.composition(), "Initial condition" );
            }
            else if( !condition.composition().empty() )
            {
                add_issue(
                    "Single phase initial condition must have no composition" );
            }
            if( problem_.thermal() && !condition.temperature() )
            {
                add_issue( "Thermal problem initial condition needs a "
                           "temperature" );
            }
            if( condition.temperature()
                && condition.temperature().value() <= 0 )
            {
                add_issue( "Initial temperature must be positive (K)" );
            }
        }

        void check_source_terms()
        {
            absl::flat_hash_set< std::pair< std::string, std::string > >
                sources;
            for( const auto& source : problem_.source_terms() )
            {
                if( source.well.empty() )
                {
                    add_issue( "A source term has no well" );
                }
                if( absl::c_none_of( fluid().components(),
                        [&source]( const geode::FluidComponent& component ) {
                            return component.name == source.component;
                        } ) )
                {
                    add_issue( "Source term on well ", source.well,
                        " uses unknown component ", source.component );
                }
                if( !std::isfinite( source.rate ) )
                {
                    add_issue( "Source term on well ", source.well,
                        " has no finite rate" );
                }
                if( !sources.emplace( source.well, source.component ).second )
                {
                    add_issue( "Several source terms of component ",
                        source.component, " on well ", source.well );
                }
            }
        }

        void check_thermal()
        {
            if( !problem_.thermal() )
            {
                return;
            }
            if( problem_.thermal()->reference_temperature <= 0 )
            {
                add_issue( "Reference temperature must be positive (K)" );
            }
            for( const auto& material : problem_.materials() )
            {
                if( !material.thermal )
                {
                    add_issue( "Material ", material.name,
                        " needs thermal properties for a thermal problem" );
                }
            }
            const auto* single_phase =
                dynamic_cast< const geode::SinglePhaseFluid* >( &fluid() );
            if( single_phase && !single_phase->thermal() )
            {
                add_issue( "SinglePhaseFluid needs thermal properties for a "
                           "thermal problem" );
            }
        }

        void check_schedule()
        {
            const auto& schedule = problem_.schedule();
            if( schedule.end_time <= schedule.start_time )
            {
                add_issue( "Schedule end time must be after start time" );
            }
            if( schedule.initial_time_step <= 0 )
            {
                add_issue( "Schedule initial time step must be positive" );
            }
            if( schedule.max_time_step
                && schedule.max_time_step.value() < schedule.initial_time_step )
            {
                add_issue( "Schedule maximum time step must not be smaller "
                           "than the initial time step" );
            }
        }

        void check_output()
        {
            const auto& output = problem_.output();
            const auto& schedule = problem_.schedule();
            if( output.frequency && output.frequency.value() <= 0 )
            {
                add_issue( "Output frequency must be positive" );
            }
            if( output.restart_frequency
                && output.restart_frequency.value() <= 0 )
            {
                add_issue( "Restart frequency must be positive" );
            }
            if( !are_in_range(
                    output.times, schedule.start_time, schedule.end_time ) )
            {
                add_issue( "Output times must be inside the schedule" );
            }
        }

    private:
        const geode::PhysicalProblem& problem_;
        std::vector< std::string > issues_;
    };

    // Blocks of a region: the Block itself, or the items of a collection of
    // Blocks (BlockCollection, StratigraphicUnit...). No value if the region
    // is neither.
    std::optional< std::vector< geode::uuid > > region_blocks(
        const geode::BRep& model, const geode::uuid& region )
    {
        if( model.has_block( region ) )
        {
            return std::vector< geode::uuid >{ region };
        }
        std::vector< geode::uuid > blocks;
        for( const auto& item : model.items( region ) )
        {
            if( item.type != geode::Block3D::component_type_static() )
            {
                return std::nullopt;
            }
            blocks.push_back( item.id );
        }
        if( blocks.empty() )
        {
            return std::nullopt;
        }
        return blocks;
    }

    // Checks that the regions of items of one kind (materials or initial
    // conditions) are model regions, and that no Block gets two items
    class RegionCoverageChecker
    {
    public:
        RegionCoverageChecker( const geode::BRep& model,
            std::string_view item_kind,
            std::vector< std::string >& issues )
            : model_( model ), item_kind_( item_kind ), issues_( issues )
        {
        }

        void check( geode::index_t item,
            std::string_view item_name,
            absl::Span< const geode::uuid > regions )
        {
            for( const auto& region : regions )
            {
                const auto blocks = region_blocks( model_, region );
                if( !blocks )
                {
                    issues_.emplace_back( absl::StrCat( "Region ",
                        region.string(), " of ", item_kind_, " ", item_name,
                        " is not a Block or a collection of Blocks of the "
                        "model" ) );
                    continue;
                }
                for( const auto& block : blocks.value() )
                {
                    const auto [owner, inserted] =
                        block_items_.try_emplace( block, item, item_name );
                    if( !inserted && owner->second.first != item )
                    {
                        issues_.emplace_back( absl::StrCat( "Block ",
                            block.string(), " is covered by ", item_kind_, "s ",
                            owner->second.second, " and ", item_name ) );
                    }
                }
            }
        }

    private:
        const geode::BRep& model_;
        std::string_view item_kind_;
        std::vector< std::string >& issues_;
        absl::flat_hash_map< geode::uuid,
            std::pair< geode::index_t, std::string > >
            block_items_;
    };
} // namespace

namespace geode
{
    std::vector< std::string > validate_physical_problem(
        const PhysicalProblem& problem )
    {
        return ProblemValidator{ problem }.run();
    }

    std::vector< std::string > validate_physical_problem_regions(
        const PhysicalProblem& problem, const BRep& model )
    {
        std::vector< std::string > issues;
        if( problem.model_id() && problem.model_id().value() != model.id() )
        {
            issues.emplace_back( absl::StrCat( "Problem was built for model ",
                problem.model_id()->string(), ", not for model ",
                model.id().string() ) );
        }
        RegionCoverageChecker material_checker{ model, "material", issues };
        for( const auto material_id : Indices{ problem.materials() } )
        {
            const auto& material = problem.materials()[material_id];
            material_checker.check(
                material_id, material.name, material.regions );
        }
        RegionCoverageChecker condition_checker{ model, "initial condition",
            issues };
        for( const auto condition_id :
            Range{ problem.nb_initial_conditions() } )
        {
            condition_checker.check( condition_id,
                absl::StrCat( "#", condition_id ),
                problem.initial_condition( condition_id ).regions() );
        }
        return issues;
    }
} // namespace geode
