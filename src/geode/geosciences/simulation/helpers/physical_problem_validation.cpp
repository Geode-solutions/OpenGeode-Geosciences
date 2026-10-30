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
#include <array>
#include <cmath>
#include <limits>
#include <numeric>
#include <optional>
#include <string_view>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/str_cat.h>
#include <absl/types/span.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/saturation_functions.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/solid_mechanics.hpp>
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
            if( problem_.solid_mechanics() )
            {
                check_solid_mechanics();
            }
            else if( check_flow() )
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
                add_issue( "No flow model nor solid mechanics is defined" );
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

        void check_solid_mechanics()
        {
            if( problem_.has_flow() )
            {
                add_issue( "Coupled flow and solid mechanics (poromechanics) "
                           "is not supported yet" );
                return;
            }
            if( problem_.thermal() )
            {
                add_issue( "Thermal parameters require a flow model" );
            }
            if( problem_.nb_initial_conditions() != 0 )
            {
                add_issue( "Initial conditions require a flow model" );
            }
            if( !problem_.source_terms().empty() )
            {
                add_issue( "Source terms require a flow model" );
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

    struct NamedProperty
    {
        geode::PHYSICAL_PROPERTY_NAME property;
        std::string_view name;
    };

    constexpr std::array< NamedProperty, 5 > FLOW_BOUNDARY_PROPERTIES{ {
        { geode::PHYSICAL_PROPERTY_NAME::boundary_pressure,
            "boundary_pressure" },
        { geode::PHYSICAL_PROPERTY_NAME::boundary_temperature,
            "boundary_temperature" },
        { geode::PHYSICAL_PROPERTY_NAME::boundary_oil_fraction,
            "boundary_oil_fraction" },
        { geode::PHYSICAL_PROPERTY_NAME::boundary_gas_fraction,
            "boundary_gas_fraction" },
        { geode::PHYSICAL_PROPERTY_NAME::boundary_water_fraction,
            "boundary_water_fraction" },
    } };

    constexpr std::array< NamedProperty, 3 > MECHANICS_BOUNDARY_PROPERTIES{ {
        { geode::PHYSICAL_PROPERTY_NAME::boundary_displacement,
            "boundary_displacement" },
        { geode::PHYSICAL_PROPERTY_NAME::boundary_traction,
            "boundary_traction" },
        { geode::PHYSICAL_PROPERTY_NAME::boundary_normal_traction,
            "boundary_normal_traction" },
    } };

    constexpr std::array< std::string_view, 3 > AXIS_NAMES{ "x", "y", "z" };

    using Vector3 = std::array< double, 3 >;

    template < typename T >
    std::string_view value_type_name();

    template <>
    std::string_view value_type_name< double >()
    {
        return "double";
    }

    template <>
    std::string_view value_type_name< Vector3 >()
    {
        return "std::array< double, 3 >";
    }

    bool is_finite( double value )
    {
        return std::isfinite( value );
    }

    bool is_finite( const Vector3& value )
    {
        return absl::c_all_of( value, []( double component ) {
            return std::isfinite( component );
        } );
    }

    // Free displacement components are NaN, which equals no value
    bool same_components( const Vector3& first, const Vector3& second )
    {
        for( const auto axis : geode::LRange{ 3 } )
        {
            if( first[axis] != second[axis]
                && !(
                    std::isnan( first[axis] ) && std::isnan( second[axis] ) ) )
            {
                return false;
            }
        }
        return true;
    }

    // Model physical properties read by solid mechanics, and boundary
    // conditions of the other process
    class PhysicalPropertyChecker
    {
    public:
        PhysicalPropertyChecker(
            const geode::PhysicalProblem& problem, const geode::BRep& model )
            : problem_( problem ), model_( model )
        {
        }

        std::vector< std::string > run()
        {
            if( problem_.solid_mechanics() )
            {
                check_absent_properties( FLOW_BOUNDARY_PROPERTIES,
                    "a flow boundary condition: it needs a flow model, not "
                    "solid mechanics" );
                check_elastic_properties();
                check_initial_stresses();
                check_displacements();
                check_tractions();
            }
            else if( problem_.has_flow() )
            {
                check_absent_properties( MECHANICS_BOUNDARY_PROPERTIES,
                    "a mechanical boundary condition: it needs solid "
                    "mechanics, not a flow model" );
            }
            return std::move( issues_ );
        }

    private:
        template < typename... Args >
        void add_issue( const Args&... args )
        {
            issues_.emplace_back( absl::StrCat( args... ) );
        }

        void check_absent_properties(
            absl::Span< const NamedProperty > properties,
            std::string_view reason )
        {
            for( const auto& [property, name] : properties )
            {
                if( model_.has_physical_property( property ) )
                {
                    add_issue( "Physical property ", name, " is ", reason );
                }
            }
        }

        // Attribute of the property, if defined on the expected components
        std::optional< geode::uuid > property_attribute(
            const NamedProperty& property,
            const geode::ComponentType& component_type,
            std::string_view component_name )
        {
            if( !model_.has_physical_property( property.property ) )
            {
                return std::nullopt;
            }
            const auto& info =
                model_.physical_property_info( property.property );
            if( info.component_type != component_type )
            {
                add_issue( "Physical property ", property.name,
                    " must be defined on ", component_name );
                return std::nullopt;
            }
            return info.attribute_id;
        }

        template < typename T, typename IsValid >
        void check_block_values( const NamedProperty& property,
            std::string_view requirement,
            const IsValid& is_valid )
        {
            const auto attribute_id = property_attribute( property,
                geode::Block3D::component_type_static(), "Blocks" );
            if( !attribute_id )
            {
                return;
            }
            for( const auto& block : model_.blocks() )
            {
                const auto& mesh = block.mesh();
                if( mesh.nb_polyhedra() == 0 )
                {
                    continue;
                }
                const auto& manager = mesh.polyhedron_attribute_manager();
                if( !manager.attribute_exists( attribute_id.value() ) )
                {
                    add_issue( "Block ", block.id().string(), " has no ",
                        property.name, " attribute" );
                    continue;
                }
                if( manager.attribute_type( attribute_id.value() )
                    != typeid( T ).name() )
                {
                    add_issue( property.name, " attribute on Block ",
                        block.id().string(), " must store ",
                        value_type_name< T >(), " values" );
                    continue;
                }
                const auto attribute =
                    manager.find_read_only_attribute< T >(
                        attribute_id.value() );
                geode::index_t nb_invalid{ 0 };
                geode::index_t first_invalid{ geode::NO_ID };
                for( const auto polyhedron :
                    geode::Range{ mesh.nb_polyhedra() } )
                {
                    if( attribute->has_value( polyhedron )
                        && is_valid( attribute->value( polyhedron ) ) )
                    {
                        continue;
                    }
                    if( nb_invalid == 0 )
                    {
                        first_invalid = polyhedron;
                    }
                    nb_invalid++;
                }
                if( nb_invalid != 0 )
                {
                    add_issue( "Block ", block.id().string(), " has ",
                        nb_invalid, " polyhedra without ", requirement, " ",
                        property.name, " value (first: polyhedron ",
                        first_invalid, ")" );
                }
            }
        }

        void check_required_block_property( const NamedProperty& property )
        {
            if( !model_.has_physical_property( property.property ) )
            {
                add_issue( "Solid mechanics needs a ", property.name,
                    " physical property on the model Blocks" );
            }
        }

        void check_elastic_properties()
        {
            const auto is_positive = []( double value ) {
                return value > 0;
            };
            for( const auto& modulus :
                { NamedProperty{ geode::PHYSICAL_PROPERTY_NAME::bulk_modulus,
                      "bulk_modulus" },
                    NamedProperty{ geode::PHYSICAL_PROPERTY_NAME::shear_modulus,
                        "shear_modulus" } } )
            {
                check_required_block_property( modulus );
                check_block_values< double >(
                    modulus, "a positive", is_positive );
            }
            const NamedProperty density{ geode::PHYSICAL_PROPERTY_NAME::density,
                "density" };
            if( problem_.gravity().length() > 0 )
            {
                check_required_block_property( density );
            }
            check_block_values< double >(
                density, "a non-negative", []( double value ) {
                    return value >= 0;
                } );
        }

        void check_initial_stresses()
        {
            for( const auto& stress :
                { NamedProperty{ geode::PHYSICAL_PROPERTY_NAME::initial_stress,
                      "initial_stress" },
                    NamedProperty{
                        geode::PHYSICAL_PROPERTY_NAME::initial_shear_stress,
                        "initial_shear_stress" } } )
            {
                check_block_values< Vector3 >(
                    stress, "a finite", []( const Vector3& value ) {
                        return is_finite( value );
                    } );
            }
        }

        // Value on each polygon of each Surface carrying the property
        template < typename T, typename Check >
        void check_surface_values(
            const NamedProperty& property, const Check& check )
        {
            const auto attribute_id = property_attribute( property,
                geode::Surface3D::component_type_static(), "Surfaces" );
            if( !attribute_id )
            {
                return;
            }
            for( const auto& surface : model_.surfaces() )
            {
                const auto& mesh = surface.mesh();
                const auto& manager = mesh.polygon_attribute_manager();
                if( mesh.nb_polygons() == 0
                    || !manager.attribute_exists( attribute_id.value() ) )
                {
                    continue;
                }
                if( manager.attribute_type( attribute_id.value() )
                    != typeid( T ).name() )
                {
                    add_issue( property.name, " attribute on Surface ",
                        surface.id().string(), " must store ",
                        value_type_name< T >(), " values" );
                    continue;
                }
                const auto attribute =
                    manager.find_read_only_attribute< T >(
                        attribute_id.value() );
                std::vector< T > values;
                values.reserve( mesh.nb_polygons() );
                for( const auto polygon : geode::Range{ mesh.nb_polygons() } )
                {
                    if( !attribute->has_value( polygon ) )
                    {
                        add_issue( property.name, " attribute on Surface ",
                            surface.id().string(), " has no value on polygon ",
                            polygon );
                        break;
                    }
                    values.push_back( attribute->value( polygon ) );
                }
                if( values.size() == mesh.nb_polygons() )
                {
                    check( surface, values );
                }
            }
        }

        void check_displacements()
        {
            std::array< bool, 3 > displaced_axes{ false, false, false };
            check_surface_values< Vector3 >(
                MECHANICS_BOUNDARY_PROPERTIES[0],
                [this, &displaced_axes]( const geode::Surface3D& surface,
                    absl::Span< const Vector3 > values ) {
                    const auto& displacement = values.front();
                    if( !absl::c_all_of(
                            values, [&displacement]( const Vector3& value ) {
                                return same_components( value, displacement );
                            } ) )
                    {
                        add_issue( "boundary_displacement on Surface ",
                            surface.id().string(),
                            " must be uniform on the Surface" );
                        return;
                    }
                    for( const auto axis : geode::LRange{ 3 } )
                    {
                        displaced_axes[axis] = displaced_axes[axis]
                                               || !std::isnan(
                                                   displacement[axis] );
                    }
                } );
            for( const auto axis : geode::LRange{ 3 } )
            {
                if( !displaced_axes[axis] )
                {
                    add_issue( "Solid mechanics needs a displacement imposed "
                               "along ",
                        AXIS_NAMES[axis],
                        " on at least one Surface to prevent rigid body "
                        "motions" );
                }
            }
        }

        template < typename T >
        void check_finite_surface_values( const NamedProperty& property )
        {
            check_surface_values< T >(
                property, [this, &property]( const geode::Surface3D& surface,
                              absl::Span< const T > values ) {
                    if( !absl::c_all_of( values, []( const T& value ) {
                            return is_finite( value );
                        } ) )
                    {
                        add_issue( property.name, " on Surface ",
                            surface.id().string(), " must be finite" );
                    }
                } );
        }

        void check_tractions()
        {
            check_finite_surface_values< Vector3 >(
                MECHANICS_BOUNDARY_PROPERTIES[1] );
            check_finite_surface_values< double >(
                MECHANICS_BOUNDARY_PROPERTIES[2] );
        }

    private:
        const geode::PhysicalProblem& problem_;
        const geode::BRep& model_;
        std::vector< std::string > issues_;
    };

    struct MeanStressSigns
    {
        geode::index_t nb_positives{ 0 };
        geode::index_t nb_negatives{ 0 };
    };

    // Sign of the mean normal stress of each cell, uniform values included
    MeanStressSigns mean_stress_signs(
        const geode::BRep& model, const geode::uuid& attribute_id )
    {
        MeanStressSigns signs;
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            const auto& manager = mesh.polyhedron_attribute_manager();
            if( !manager.attribute_exists( attribute_id )
                || manager.attribute_type( attribute_id )
                       != typeid( std::array< double, 3 > ).name() )
            {
                continue;
            }
            const auto stress =
                manager.find_read_only_attribute< std::array< double, 3 > >(
                    attribute_id );
            for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
            {
                const auto& value = stress->value( polyhedron );
                const auto trace = value[0] + value[1] + value[2];
                if( trace > 0 )
                {
                    signs.nb_positives++;
                }
                else if( trace < 0 )
                {
                    signs.nb_negatives++;
                }
            }
        }
        return signs;
    }
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
        const auto materials = problem.materials();
        for( const auto material_id : Indices{ materials } )
        {
            const auto& material = materials[material_id];
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

    std::vector< std::string > validate_physical_problem_properties(
        const PhysicalProblem& problem, const BRep& model )
    {
        return PhysicalPropertyChecker{ problem, model }.run();
    }

    std::vector< std::string > physical_problem_warnings(
        const PhysicalProblem& problem, const BRep& model )
    {
        std::vector< std::string > warnings;
        const auto& mechanics = problem.solid_mechanics();
        if( !mechanics )
        {
            return warnings;
        }
        if( !problem.materials().empty() )
        {
            warnings.emplace_back(
                "Materials are ignored by solid mechanics: the elastic "
                "properties and density are the model physical properties" );
        }
        if( !model.has_physical_property(
                PHYSICAL_PROPERTY_NAME::initial_stress ) )
        {
            return warnings;
        }
        const auto signs = mean_stress_signs(
            model, model
                       .physical_property_info(
                           PHYSICAL_PROPERTY_NAME::initial_stress )
                       .attribute_id );
        const auto compression_positive =
            mechanics->sign_convention
            == STRESS_SIGN_CONVENTION::compression_positive;
        const auto nb_compressions =
            compression_positive ? signs.nb_positives : signs.nb_negatives;
        const auto nb_tensions =
            compression_positive ? signs.nb_negatives : signs.nb_positives;
        if( nb_tensions > nb_compressions )
        {
            warnings.emplace_back( absl::StrCat( "Read as ",
                compression_positive ? "compression positive"
                                     : "tension positive",
                " values, the mean initial_stress is a tension in ",
                nb_tensions, " of ", nb_tensions + nb_compressions,
                " cells: the model may use the opposite sign convention" ) );
        }
        return warnings;
    }
} // namespace geode
