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

#include <geode/geosciences/simulation/helpers/physical_problem_simulation.hpp>

#include <algorithm>
#include <exception>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/ascii.h>
#include <absl/strings/str_join.h>
#include <absl/types/span.h>

#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/component.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/helpers/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/physical_problem_validation.hpp>

namespace
{
    void add_issues( std::string_view title,
        absl::Span< const std::string > issues,
        std::string& message )
    {
        if( issues.empty() )
        {
            return;
        }
        absl::StrAppend( &message, message.empty() ? "" : "\n", issues.size(),
            " ", title, issues.size() > 1 ? "s" : "", ":" );
        for( const auto& issue : issues )
        {
            absl::StrAppend( &message, "\n - ", issue );
        }
    }

    bool are_valid_regions(
        const geode::BRep& model, absl::Span< const geode::uuid > regions )
    {
        return std::all_of(
            regions.begin(), regions.end(), [&model]( const auto& region ) {
                return geode::physical_problem_region_blocks( model, region )
                    .has_value();
            } );
    }

    absl::flat_hash_set< geode::index_t > used_items(
        const absl::flat_hash_map< geode::uuid, geode::index_t >& block_items )
    {
        absl::flat_hash_set< geode::index_t > items;
        for( const auto& [block, item] : block_items )
        {
            items.insert( item );
        }
        return items;
    }

    void warn_unused_items( const geode::PhysicalProblem& problem,
        const geode::BRep& model,
        geode::SimulationReport& report )
    {
        const auto materials = problem.materials();
        const auto used_materials = used_items(
            geode::physical_problem_block_materials( problem, model ) );
        for( const auto material : geode::Indices{ materials } )
        {
            if( !used_materials.contains( material )
                && are_valid_regions( model, materials[material].regions ) )
            {
                report.add_warning(
                    "Material ", materials[material].name, " covers no Block" );
            }
        }
        const auto used_conditions =
            used_items( geode::physical_problem_block_initial_conditions(
                problem, model ) );
        for( const auto condition :
            geode::Range{ problem.nb_initial_conditions() } )
        {
            if( !used_conditions.contains( condition )
                && are_valid_regions(
                    model, problem.initial_condition( condition ).regions() ) )
            {
                report.add_warning(
                    "Initial condition #", condition, " covers no Block" );
            }
        }
    }
} // namespace

namespace geode
{
    bool SimulationReport::is_simulable() const
    {
        return errors.empty();
    }

    std::string SimulationReport::string() const
    {
        std::string message;
        add_issues( "error", errors, message );
        add_issues( "warning", warnings, message );
        if( message.empty() )
        {
            return "No simulation issue";
        }
        return message;
    }

    void SimulationReport::add_errors( std::vector< std::string > issues )
    {
        errors.insert( errors.end(), std::make_move_iterator( issues.begin() ),
            std::make_move_iterator( issues.end() ) );
    }

    SimulationChecker::SimulationChecker() = default;

    SimulationChecker::~SimulationChecker() = default;

    SimulationReport check_physical_problem_simulation(
        const PhysicalProblem& problem,
        const BRep& model,
        std::string_view simulator )
    {
        SimulationReport report;
        const auto key = absl::AsciiStrToLower( simulator );
        if( !SimulationCheckerFactory::has_creator( key ) )
        {
            const auto creators = SimulationCheckerFactory::list_creators();
            std::vector< std::string > simulators{ creators.begin(),
                creators.end() };
            absl::c_sort( simulators );
            report.add_error( "Unknown simulator \"", simulator,
                "\". Registered simulators: ",
                simulators.empty() ? "none"
                                   : absl::StrJoin( simulators, ", " ) );
            return report;
        }
        try
        {
            return SimulationCheckerFactory::create( key )->check(
                problem, model );
        }
        catch( const std::exception& exception )
        {
            report.add_error( "Checking the problem for ", key,
                " failed: ", exception.what() );
            return report;
        }
    }

    bool is_physical_problem_simulable( const PhysicalProblem& problem,
        const BRep& model,
        std::string_view simulator )
    {
        return check_physical_problem_simulation( problem, model, simulator )
            .is_simulable();
    }

    void check_physical_problem_consistency( const PhysicalProblem& problem,
        const BRep& model,
        SimulationReport& report )
    {
        report.add_errors( validate_physical_problem( problem ) );
        report.add_errors(
            validate_physical_problem_regions( problem, model ) );
        warn_unused_items( problem, model, report );
    }

    std::string simulation_component_description( const Component3D& component )
    {
        if( component.name() && !component.name()->empty() )
        {
            return absl::StrCat( component.component_type().get(), " \"",
                component.name().value(), "\" (", component.id().string(),
                ")" );
        }
        return absl::StrCat(
            component.component_type().get(), " ", component.id().string() );
    }
} // namespace geode
