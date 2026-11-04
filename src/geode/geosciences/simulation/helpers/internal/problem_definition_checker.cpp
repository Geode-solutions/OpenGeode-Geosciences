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

#include <geode/geosciences/simulation/helpers/internal/problem_definition_checker.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numeric>
#include <string>
#include <string_view>
#include <utility>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/str_cat.h>
#include <absl/types/span.h>

#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

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
#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace
{
    using geode::SIMULATION_CHECK_STAGE;
    using geode::SIMULATION_ISSUE_SEVERITY;
    using geode::internal::initial_condition_subject;
    using geode::internal::is_multiphase;
    using geode::internal::is_non_negative;
    using geode::internal::is_positive;
    using geode::internal::material_subject;

    constexpr double COMPOSITION_TOLERANCE{ 1e-6 };

    bool are_positive( absl::Span< const double > values )
    {
        return std::all_of( values.begin(), values.end(), is_positive );
    }

    bool are_finite( absl::Span< const double > values )
    {
        return std::all_of( values.begin(), values.end(), []( double value ) {
            return std::isfinite( value );
        } );
    }

    bool is_strictly_increasing( absl::Span< const double > values )
    {
        return are_finite( values )
               && std::adjacent_find( values.begin(), values.end(),
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

    // Two tables for the same two phases, in any wetting order
    template < typename Tables >
    bool has_duplicated_phase_pair( const Tables& tables )
    {
        absl::flat_hash_set< std::pair< geode::PHASE, geode::PHASE > > pairs;
        for( const auto& table : tables )
        {
            if( !pairs
                     .emplace( std::minmax(
                         table.wetting_phase, table.non_wetting_phase ) )
                     .second )
            {
                return true;
            }
        }
        return false;
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
        ProblemValidator( const geode::PhysicalProblem& problem,
            geode::SimulationReport& report )
            : problem_( problem ), report_( report )
        {
        }

        void run()
        {
            if( problem_.solid_mechanics() && !problem_.has_flow() )
            {
                check_solid_mechanics();
            }
            else if( check_flow() )
            {
                if( has_fluid() )
                {
                    check_fluid();
                }
                check_materials();
                check_initial_conditions();
                check_source_terms();
                check_thermal();
                check_poromechanics();
            }
            check_gravity();
            check_schedule();
            check_output();
        }

    private:
        template < typename... Args >
        void add_issue( std::string subject, const Args&... message )
        {
            report_.add_error( SIMULATION_CHECK_STAGE::problem,
                std::move( subject ), message... );
        }

        void add_issue_with_fix(
            std::string subject, std::string message, std::string fix )
        {
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::problem, std::move( subject ),
                std::move( message ) };
            issue.fix = std::move( fix );
            report_.add_issue( std::move( issue ) );
        }

        [[nodiscard]] bool has_fluid() const
        {
            return problem_.has_flow() && problem_.flow().has_fluid();
        }

        [[nodiscard]] bool is_single_phase_flow() const
        {
            return problem_.has_flow()
                   && dynamic_cast< const geode::SinglePhaseFlow* >(
                          &problem_.flow() )
                          != nullptr;
        }

        [[nodiscard]] bool is_multiphase() const
        {
            return problem_.has_flow()
                   && dynamic_cast< const geode::MultiphaseFlow* >(
                          &problem_.flow() )
                          != nullptr;
        }

        [[nodiscard]] bool is_single_phase_fluid() const
        {
            return dynamic_cast< const geode::SinglePhaseFluid* >( &fluid() )
                   != nullptr;
        }

        [[nodiscard]] const geode::FluidModel& fluid() const
        {
            return problem_.fluid();
        }

        // False if there is no flow model to check
        bool check_flow()
        {
            if( !problem_.has_flow() )
            {
                add_issue_with_fix( "Problem",
                    "no flow model nor solid mechanics",
                    "define a flow model, solid mechanics or both" );
                return false;
            }
            if( !problem_.flow().has_fluid() )
            {
                add_issue( "Flow model", "no fluid" );
                return true;
            }
            const auto nb_phases = fluid().phases().size();
            if( is_multiphase() )
            {
                if( is_single_phase_fluid() )
                {
                    add_issue( "Flow model",
                        "multiphase flow needs a multiphase fluid "
                        "(DeadOilFluid, Co2BrineFluid), not a "
                        "SinglePhaseFluid" );
                }
                else if( nb_phases < 2 )
                {
                    add_issue( "Flow model",
                        "multiphase flow needs at least 2 fluid phases" );
                }
            }
            else if( is_single_phase_flow() && !is_single_phase_fluid()
                     && nb_phases != 1 )
            {
                add_issue(
                    "Flow model", "single phase flow needs exactly 1 phase" );
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
                    add_issue( "Fluid", "phase ", geode::phase_name( phase ),
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
                    add_issue( "Fluid", "a component has no name" );
                }
                else if( !names.insert( component.name ).second )
                {
                    add_issue( "Fluid", "component ", component.name,
                        " is defined several times" );
                }
                if( !is_non_negative( component.molar_weight ) )
                {
                    add_issue( "Fluid", "component ", component.name,
                        " molar weight must be finite and non-negative" );
                }
            }
        }

        void check_single_phase_fluid( const geode::SinglePhaseFluid& fluid )
        {
            if( fluid.phases().size() != 1 )
            {
                add_issue( "Fluid", "SinglePhaseFluid needs exactly 1 phase" );
            }
            if( !is_positive( fluid.reference_density() ) )
            {
                add_issue(
                    "Fluid", "SinglePhaseFluid density must be positive" );
            }
            if( !is_positive( fluid.reference_viscosity() ) )
            {
                add_issue(
                    "Fluid", "SinglePhaseFluid viscosity must be positive" );
            }
            if( !is_non_negative( fluid.compressibility() ) )
            {
                add_issue( "Fluid",
                    "SinglePhaseFluid compressibility must not be negative" );
            }
            if( !std::isfinite( fluid.viscosibility() )
                || !std::isfinite( fluid.reference_pressure() ) )
            {
                add_issue( "Fluid", "SinglePhaseFluid viscosibility and "
                                    "reference pressure must be finite" );
            }
            if( fluid.components().size() != 1 )
            {
                add_issue(
                    "Fluid", "SinglePhaseFluid needs exactly 1 component" );
            }
        }

        void check_dead_oil_fluid( const geode::DeadOilFluid& fluid )
        {
            const auto nb_phases = fluid.phases().size();
            if( nb_phases < 2 )
            {
                add_issue_with_fix( "Fluid", "DeadOilFluid needs 2 or 3 phases",
                    "use a SinglePhaseFluid for one phase" );
            }
            if( fluid.components().size() != nb_phases )
            {
                add_issue( "Fluid", "DeadOilFluid needs one component per "
                                    "phase" );
            }
            if( fluid.surface_densities().size() != nb_phases
                || !are_positive( fluid.surface_densities() ) )
            {
                add_issue( "Fluid", "DeadOilFluid needs one positive surface "
                                    "density per phase" );
            }
            check_dead_oil_tables( fluid );
            if( has_phase( fluid.phases(), geode::PHASE::water )
                != fluid.water_pvt().has_value() )
            {
                add_issue( "Fluid", "DeadOilFluid water PVT must be given if "
                                    "and only if water is a phase" );
            }
            if( fluid.water_pvt() )
            {
                check_water_pvt( fluid.water_pvt().value() );
            }
        }

        void check_water_pvt( const geode::WaterPvt& pvt )
        {
            if( !is_positive( pvt.formation_volume_factor )
                || !is_positive( pvt.viscosity ) )
            {
                add_issue( "Fluid", "DeadOilFluid water PVT formation volume "
                                    "factor and viscosity must be positive" );
            }
            if( !is_non_negative( pvt.compressibility ) )
            {
                add_issue( "Fluid", "DeadOilFluid water PVT compressibility "
                                    "must not be negative" );
            }
            if( !std::isfinite( pvt.reference_pressure ) )
            {
                add_issue( "Fluid", "DeadOilFluid water PVT reference "
                                    "pressure must be finite" );
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
                    add_issue( "Fluid", "DeadOilFluid needs ", expected,
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
                add_issue( "Fluid",
                    "DeadOilFluid PVT tables are for oil and gas only" );
            }
            if( table.pressures.size() < 2
                || table.formation_volume_factors.size()
                       != table.pressures.size()
                || table.viscosities.size() != table.pressures.size() )
            {
                add_issue( "Fluid", "DeadOilFluid ", name,
                    " PVT table needs at least 2 rows of equal size" );
            }
            if( !is_strictly_increasing( table.pressures ) )
            {
                add_issue( "Fluid", "DeadOilFluid ", name,
                    " PVT pressures must be strictly increasing" );
            }
            if( !are_positive( table.formation_volume_factors )
                || !are_positive( table.viscosities ) )
            {
                add_issue( "Fluid", "DeadOilFluid ", name,
                    " PVT formation volume factors and viscosities must be "
                    "positive" );
            }
        }

        void check_co2_brine_fluid( const geode::Co2BrineFluid& fluid )
        {
            const std::array< geode::PHASE, 2 > expected{ geode::PHASE::gas,
                geode::PHASE::water };
            if( !same_phase_set( fluid.phases(), expected )
                || fluid.components().size() != 2 )
            {
                add_issue( "Fluid", "Co2BrineFluid needs phases gas and water "
                                    "and 2 components" );
            }
            if( !is_non_negative( fluid.salinity() ) )
            {
                add_issue(
                    "Fluid", "Co2BrineFluid salinity must not be negative" );
            }
            check_validity_range( fluid.pressure_range(), "pressure" );
            check_validity_range( fluid.temperature_range(), "temperature" );
        }

        void check_validity_range(
            const geode::ValidityRange& range, std::string_view name )
        {
            if( !std::isfinite( range.min ) || !std::isfinite( range.max )
                || range.min >= range.max || !is_positive( range.step ) )
            {
                add_issue( "Fluid", "Co2BrineFluid ", name,
                    " range needs min < max and a positive step" );
            }
        }

        void check_composition(
            absl::Span< const double > composition, const std::string& subject )
        {
            if( composition.size() != fluid().components().size() )
            {
                add_issue( subject,
                    "composition needs one value per fluid component" );
                return;
            }
            const auto sum =
                std::accumulate( composition.begin(), composition.end(), 0. );
            if( !are_in_range( composition, 0, 1 )
                || std::fabs( sum - 1 ) > COMPOSITION_TOLERANCE )
            {
                add_issue( subject,
                    "composition values must be in [0, 1] and sum to 1" );
            }
        }

        void check_materials()
        {
            if( problem_.materials().empty() )
            {
                add_issue( "Materials", "no material is defined" );
                return;
            }
            absl::flat_hash_set< std::string > names;
            absl::flat_hash_set< geode::uuid > regions;
            geode::index_t nb_defaults{ 0 };
            for( const auto& material : problem_.materials() )
            {
                if( material.name.empty()
                    || !names.insert( material.name ).second )
                {
                    add_issue( "Materials",
                        "names must be unique and not "
                        "empty (\"",
                        material.name, "\")" );
                }
                nb_defaults += material.regions.empty() ? 1 : 0;
                check_unique_regions( material.regions, regions, "Materials" );
                check_material_values( material );
                check_saturation_functions( material );
            }
            if( nb_defaults > 1 )
            {
                add_issue( "Materials", "at most one material can have no "
                                        "region" );
            }
        }

        void check_unique_regions( absl::Span< const geode::uuid > regions,
            absl::flat_hash_set< geode::uuid >& known_regions,
            std::string_view subject )
        {
            for( const auto& region : regions )
            {
                if( !known_regions.insert( region ).second )
                {
                    add_issue( std::string{ subject }, "region ",
                        region.string(), " is assigned several times" );
                }
            }
        }

        void check_material_values( const geode::Material& material )
        {
            const auto subject = material_subject( material.name );
            if( !is_non_negative( material.pore_compressibility ) )
            {
                add_issue(
                    subject, "pore compressibility must not be negative" );
            }
            if( !std::isfinite( material.reference_pressure ) )
            {
                add_issue( subject, "reference pressure must be finite" );
            }
            if( material.porosity
                && ( !is_positive( material.porosity.value() )
                     || material.porosity.value() > 1 ) )
            {
                add_issue( subject, "porosity must be in (0, 1]" );
            }
            if( material.permeability
                && !are_positive( material.permeability.value() ) )
            {
                add_issue( subject, "permeability must be positive" );
            }
        }

        void check_saturation_functions( const geode::Material& material )
        {
            const auto subject = material_subject( material.name );
            if( !material.relative_permeability && is_multiphase() )
            {
                add_issue( subject,
                    "needs a relative permeability for multiphase flow" );
            }
            if( !has_fluid() )
            {
                return;
            }
            if( material.relative_permeability )
            {
                check_relative_permeability(
                    *material.relative_permeability, subject );
            }
            if( material.capillary_pressure )
            {
                check_capillary_pressure(
                    *material.capillary_pressure, subject );
            }
        }

        void check_relative_permeability(
            const geode::RelativePermeability& relperm,
            const std::string& subject )
        {
            if( !same_phase_set( relperm.phases(), fluid().phases() ) )
            {
                add_issue( subject,
                    "relative permeability phases must be the fluid phases" );
            }
            if( const auto* brooks_corey = dynamic_cast<
                    const geode::BrooksCoreyRelativePermeability* >(
                    &relperm ) )
            {
                check_brooks_corey_relperm( *brooks_corey, subject );
            }
            else if( const auto* table = dynamic_cast<
                         const geode::TableRelativePermeability* >( &relperm ) )
            {
                check_table_relperm( *table, subject );
            }
        }

        void check_brooks_corey_relperm(
            const geode::BrooksCoreyRelativePermeability& relperm,
            const std::string& subject )
        {
            const auto nb_phases = relperm.phases().size();
            if( relperm.residual_saturations().size() != nb_phases
                || relperm.exponents().size() != nb_phases
                || relperm.max_values().size() != nb_phases )
            {
                add_issue( subject, "Brooks-Corey relative permeability needs "
                                    "one value per phase" );
                return;
            }
            check_residual_saturations(
                relperm.residual_saturations(), subject );
            if( !are_positive( relperm.exponents() ) )
            {
                add_issue( subject, "Brooks-Corey exponents must be positive" );
            }
            if( !are_in_range( relperm.max_values(), 0, 1 ) )
            {
                add_issue( subject, "relative permeability maximum values "
                                    "must be in [0, 1]" );
            }
        }

        void check_residual_saturations(
            absl::Span< const double > saturations, const std::string& subject )
        {
            const auto sum =
                std::accumulate( saturations.begin(), saturations.end(), 0. );
            if( !are_in_range( saturations, 0, 1 ) || sum >= 1 )
            {
                add_issue( subject, "residual saturations must be in [0, 1) "
                                    "with a sum below 1" );
            }
        }

        void check_table_relperm(
            const geode::TableRelativePermeability& relperm,
            const std::string& subject )
        {
            const auto nb_phases = relperm.phases().size();
            if( relperm.tables().empty() )
            {
                add_issue(
                    subject, "table relative permeability has no table" );
            }
            else if( nb_phases >= 2
                     && relperm.tables().size() != nb_phases - 1 )
            {
                add_issue( subject, "table relative permeability needs ",
                    nb_phases - 1, " tables for ", nb_phases, " phases" );
            }
            for( const auto& table : relperm.tables() )
            {
                check_table_phases(
                    table.wetting_phase, table.non_wetting_phase, subject );
                check_saturation_table( table.wetting_kr, subject, true );
                check_saturation_table( table.non_wetting_kr, subject, true );
            }
            if( has_duplicated_phase_pair( relperm.tables() ) )
            {
                add_issue( subject, "table relative permeability has several "
                                    "tables for the same two phases" );
            }
        }

        void check_table_phases( geode::PHASE wetting,
            geode::PHASE non_wetting,
            const std::string& subject )
        {
            if( wetting == non_wetting
                || !has_phase( fluid().phases(), wetting )
                || !has_phase( fluid().phases(), non_wetting ) )
            {
                add_issue( subject, "table phases must be two different fluid "
                                    "phases" );
            }
        }

        void check_saturation_table( const geode::SaturationTable& table,
            const std::string& subject,
            bool is_relperm )
        {
            if( table.saturations.size() < 2
                || table.values.size() != table.saturations.size() )
            {
                add_issue( subject, "saturation table needs at least 2 points "
                                    "of equal size" );
                return;
            }
            if( !is_strictly_increasing( table.saturations )
                || !are_in_range( table.saturations, 0, 1 ) )
            {
                add_issue( subject, "table saturations must be strictly "
                                    "increasing in [0, 1]" );
            }
            if( is_relperm && !are_in_range( table.values, 0, 1 ) )
            {
                add_issue(
                    subject, "relative permeability values must be in [0, 1]" );
            }
            if( !is_relperm && !are_finite( table.values ) )
            {
                add_issue( subject, "capillary pressure values must be "
                                    "finite" );
            }
        }

        void check_capillary_pressure(
            const geode::CapillaryPressure& capillary,
            const std::string& subject )
        {
            if( const auto* brooks_corey =
                    dynamic_cast< const geode::BrooksCoreyCapillaryPressure* >(
                        &capillary ) )
            {
                check_brooks_corey_capillary( *brooks_corey, subject );
            }
            else if( const auto* table =
                         dynamic_cast< const geode::TableCapillaryPressure* >(
                             &capillary ) )
            {
                check_table_capillary( *table, subject );
            }
        }

        void check_brooks_corey_capillary(
            const geode::BrooksCoreyCapillaryPressure& capillary,
            const std::string& subject )
        {
            const auto nb_phases = capillary.phases().size();
            if( !same_phase_set( capillary.phases(), fluid().phases() )
                || capillary.residual_saturations().size() != nb_phases
                || capillary.entry_pressures().size() != nb_phases
                || capillary.pore_size_distribution_indices().size()
                       != nb_phases )
            {
                add_issue( subject, "Brooks-Corey capillary pressure needs "
                                    "one value per fluid phase" );
                return;
            }
            check_residual_saturations(
                capillary.residual_saturations(), subject );
            if( !are_in_range( capillary.entry_pressures(), 0,
                    std::numeric_limits< double >::max() ) )
            {
                add_issue( subject, "capillary entry pressures must be finite "
                                    "and not negative" );
            }
            if( !are_positive( capillary.pore_size_distribution_indices() ) )
            {
                add_issue( subject,
                    "pore size distribution indices must be positive" );
            }
        }

        void check_table_capillary(
            const geode::TableCapillaryPressure& capillary,
            const std::string& subject )
        {
            if( absl::c_any_of( capillary.phases(), [this]( auto phase ) {
                    return !has_phase( fluid().phases(), phase );
                } ) )
            {
                add_issue(
                    subject, "capillary pressure phases must be fluid phases" );
            }
            const auto nb_phases = capillary.phases().size();
            if( capillary.tables().empty() )
            {
                add_issue( subject, "table capillary pressure has no table" );
            }
            else if( nb_phases >= 2
                     && capillary.tables().size() > nb_phases - 1 )
            {
                add_issue( subject, "table capillary pressure needs at most ",
                    nb_phases - 1, " tables for ", nb_phases, " phases" );
            }
            for( const auto& table : capillary.tables() )
            {
                check_table_phases(
                    table.wetting_phase, table.non_wetting_phase, subject );
                check_saturation_table( table.pressure, subject, false );
            }
            if( has_duplicated_phase_pair( capillary.tables() ) )
            {
                add_issue( subject, "table capillary pressure has several "
                                    "tables for the same two phases" );
            }
        }

        void check_initial_conditions()
        {
            if( problem_.nb_initial_conditions() == 0 )
            {
                add_issue(
                    "Initial conditions", "no initial condition is defined" );
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
                    condition.regions(), regions, "Initial conditions" );
                if( const auto* uniform =
                        dynamic_cast< const geode::UniformInitialCondition* >(
                            &condition ) )
                {
                    check_uniform_condition(
                        *uniform, initial_condition_subject( condition_id ) );
                }
            }
            if( nb_defaults > 1 )
            {
                add_issue( "Initial conditions",
                    "at most one initial condition can have no region" );
            }
        }

        void check_uniform_condition(
            const geode::UniformInitialCondition& condition,
            const std::string& subject )
        {
            if( !std::isfinite( condition.pressure() ) )
            {
                add_issue( subject, "pressure must be finite" );
            }
            if( is_multiphase() )
            {
                if( has_fluid() )
                {
                    check_composition( condition.composition(), subject );
                }
            }
            else if( !condition.composition().empty() )
            {
                add_issue( subject, "single phase initial condition must have "
                                    "no composition" );
            }
            if( problem_.thermal() && !condition.temperature() )
            {
                add_issue( subject, "thermal problem initial condition needs a "
                                    "temperature" );
            }
            if( condition.temperature()
                && !is_positive( condition.temperature().value() ) )
            {
                add_issue( subject, "temperature must be positive (K)" );
            }
        }

        void check_source_terms()
        {
            absl::flat_hash_set< std::pair< std::string, std::string > >
                sources;
            for( const auto& source : problem_.source_terms() )
            {
                const auto subject =
                    absl::StrCat( "Source term on well \"", source.well, "\"" );
                if( source.well.empty() )
                {
                    add_issue( "Source terms", "a source term has no well" );
                }
                if( has_fluid()
                    && absl::c_none_of( fluid().components(),
                        [&source]( const geode::FluidComponent& component ) {
                            return component.name == source.component;
                        } ) )
                {
                    add_issue(
                        subject, "unknown component ", source.component );
                }
                if( !std::isfinite( source.rate ) )
                {
                    add_issue( subject, "rate must be finite" );
                }
                if( !sources.emplace( source.well, source.component ).second )
                {
                    add_issue( subject, "several source terms of component ",
                        source.component );
                }
            }
        }

        void check_solid_mechanics()
        {
            if( problem_.thermal() )
            {
                add_issue( "Thermal parameters", "require a flow model" );
            }
            if( problem_.nb_initial_conditions() != 0 )
            {
                add_issue( "Initial conditions", "require a flow model" );
            }
            if( !problem_.source_terms().empty() )
            {
                add_issue( "Source terms", "require a flow model" );
            }
        }

        // The Biot coefficient gives the pore compressibility
        void check_poromechanics()
        {
            if( !problem_.solid_mechanics() )
            {
                return;
            }
            for( const auto& material : problem_.materials() )
            {
                if( material.pore_compressibility != 0
                    || material.reference_pressure != 0 )
                {
                    add_issue_with_fix( material_subject( material.name ),
                        "pore compressibility and reference pressure must be "
                        "0 with solid mechanics",
                        "set them to 0: the Biot coefficient gives the "
                        "pore compressibility" );
                }
            }
        }

        void check_thermal()
        {
            if( !problem_.thermal() )
            {
                return;
            }
            if( !is_positive( problem_.thermal()->reference_temperature ) )
            {
                add_issue( "Thermal parameters",
                    "reference temperature must be positive (K)" );
            }
            for( const auto& material : problem_.materials() )
            {
                check_material_thermal( material );
            }
            if( !has_fluid() )
            {
                return;
            }
            const auto* single_phase =
                dynamic_cast< const geode::SinglePhaseFluid* >( &fluid() );
            if( single_phase != nullptr && !single_phase->thermal() )
            {
                add_issue( "Fluid", "SinglePhaseFluid needs thermal "
                                    "properties for a thermal problem" );
            }
            else if( single_phase != nullptr )
            {
                check_fluid_thermal( single_phase->thermal().value() );
            }
        }

        void check_material_thermal( const geode::Material& material )
        {
            const auto subject = material_subject( material.name );
            if( !material.thermal )
            {
                add_issue(
                    subject, "needs thermal properties for a thermal problem" );
            }
            else if( !is_positive( material.thermal->volumetric_heat_capacity )
                     || !are_positive(
                         material.thermal->thermal_conductivity ) )
            {
                add_issue( subject, "heat capacity and thermal conductivity "
                                    "must be positive" );
            }
        }

        void check_fluid_thermal( const geode::FluidThermalProperties& thermal )
        {
            if( !is_positive( thermal.specific_heat_capacity ) )
            {
                add_issue( "Fluid", "SinglePhaseFluid specific heat capacity "
                                    "must be positive" );
            }
            if( !std::isfinite( thermal.thermal_expansion )
                || !std::isfinite( thermal.viscosity_temperature_coefficient ) )
            {
                add_issue( "Fluid",
                    "SinglePhaseFluid thermal expansion and viscosity "
                    "temperature coefficient must be finite" );
            }
        }

        void check_gravity()
        {
            const auto& gravity = problem_.gravity();
            for( const auto axis : geode::LRange{ 3 } )
            {
                if( !std::isfinite( gravity.value( axis ) ) )
                {
                    add_issue( "Gravity", "must be finite" );
                    return;
                }
            }
        }

        void check_schedule()
        {
            const auto& schedule = problem_.schedule();
            if( !std::isfinite( schedule.start_time )
                || !std::isfinite( schedule.end_time )
                || schedule.end_time <= schedule.start_time )
            {
                add_issue( "Schedule", "end time must be after start time" );
            }
            if( !is_positive( schedule.initial_time_step ) )
            {
                add_issue( "Schedule", "initial time step must be positive" );
            }
            if( schedule.max_time_step
                && !( schedule.max_time_step.value()
                      >= schedule.initial_time_step ) )
            {
                add_issue( "Schedule", "maximum time step must not be smaller "
                                       "than the initial time step" );
            }
        }

        void check_output()
        {
            const auto& output = problem_.output();
            const auto& schedule = problem_.schedule();
            if( output.frequency && !is_positive( output.frequency.value() ) )
            {
                add_issue( "Output", "frequency must be positive" );
            }
            if( output.restart_frequency
                && !is_positive( output.restart_frequency.value() ) )
            {
                add_issue( "Output", "restart frequency must be positive" );
            }
            if( !are_in_range(
                    output.times, schedule.start_time, schedule.end_time ) )
            {
                add_issue( "Output", "times must be inside the schedule" );
            }
        }

    private:
        const geode::PhysicalProblem& problem_;
        geode::SimulationReport& report_;
    };

    void warn_unused_materials(
        const geode::PhysicalProblem& problem, geode::SimulationReport& report )
    {
        if( !problem.has_flow() && !problem.materials().empty() )
        {
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::warning,
                SIMULATION_CHECK_STAGE::problem, "Materials",
                "ignored by solid mechanics without flow" };
            issue.fix = "the elastic properties and density are model physical "
                        "properties";
            report.add_issue( std::move( issue ) );
        }
    }
} // namespace

namespace geode::internal
{
    void check_physical_problem_definition(
        const PhysicalProblem& problem, SimulationReport& report )
    {
        ProblemValidator{ problem, report }.run();
        if( problem.solid_mechanics() )
        {
            warn_unused_materials( problem, report );
        }
    }
} // namespace geode::internal
