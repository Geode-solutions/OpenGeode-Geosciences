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

#include <algorithm>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/strings/ascii.h>
#include <absl/strings/str_cat.h>
#include <absl/types/span.h>

#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/helpers/internal/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace
{
    using geode::SIMULATION_CHECK_STAGE;
    using geode::SIMULATION_ISSUE_SEVERITY;
    using geode::internal::component_location;
    using geode::internal::count_string;
    using geode::internal::initial_condition_subject;
    using geode::internal::material_subject;

    // Checks that the regions of items of one kind (materials or initial
    // conditions) are model regions, and that every Block gets one item
    class RegionCoverageChecker
    {
    public:
        RegionCoverageChecker( const geode::BRep& model,
            std::string_view item_kind,
            geode::SimulationReport& report )
            : model_( model ), item_kind_( item_kind ), report_( report )
        {
        }

        void check( geode::index_t item,
            std::string subject,
            absl::Span< const geode::uuid > regions )
        {
            for( const auto& region : regions )
            {
                const auto blocks =
                    geode::internal::physical_problem_region_blocks(
                        model_, region );
                if( !blocks )
                {
                    geode::SimulationIssue issue{
                        SIMULATION_ISSUE_SEVERITY::error,
                        SIMULATION_CHECK_STAGE::regions, subject,
                        absl::StrCat( "region ", region.string(),
                            " is not a Block or a collection of Blocks of "
                            "the model" )
                    };
                    issue.fix = "use Block, BlockCollection or "
                                "StratigraphicUnit ids of this model";
                    report_.add_issue( std::move( issue ) );
                    continue;
                }
                for( const auto& block : blocks.value() )
                {
                    const auto [owner, inserted] =
                        block_items_.try_emplace( block, item );
                    if( !inserted && owner->second != item )
                    {
                        overlaps_[{ owner->second, item }].push_back( block );
                    }
                }
            }
            subjects_.push_back( std::move( subject ) );
        }

        void report_overlaps()
        {
            for( const auto& [items, blocks] : overlaps_ )
            {
                geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                    SIMULATION_CHECK_STAGE::regions,
                    absl::StrCat( subjects_[items.first], " and ",
                        subjects_[items.second] ),
                    absl::StrCat( "cover the same ",
                        count_string( blocks.size(), "Block" ) ) };
                issue.fix = absl::StrCat(
                    "each Block must be in the regions of one ", item_kind_ );
                for( const auto& block : blocks )
                {
                    issue.add_location( model_.block( block ) );
                }
                report_.add_issue( std::move( issue ) );
            }
        }

        void check_coverage(
            const absl::flat_hash_map< geode::uuid, geode::index_t >&
                block_items )
        {
            std::vector< geode::SimulationIssueLocation > uncovered;
            for( const auto& block : model_.blocks() )
            {
                if( !block_items.contains( block.id() ) )
                {
                    uncovered.push_back( component_location( block ) );
                }
            }
            if( uncovered.empty() )
            {
                return;
            }
            auto subject = absl::StrCat( item_kind_, "s" );
            subject[0] = absl::ascii_toupper( subject[0] );
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::regions, std::move( subject ),
                absl::StrCat( uncovered.size(),
                    uncovered.size() > 1 ? " Blocks have no "
                                         : " Block has no ",
                    item_kind_ ) };
            issue.fix = absl::StrCat( "add each Block to the regions of one ",
                item_kind_, ", or define one ", item_kind_, " without region" );
            issue.locations = std::move( uncovered );
            report_.add_issue( std::move( issue ) );
        }

    private:
        const geode::BRep& model_;
        std::string_view item_kind_;
        geode::SimulationReport& report_;
        absl::flat_hash_map< geode::uuid, geode::index_t > block_items_;
        std::vector< std::string > subjects_;
        std::map< std::pair< geode::index_t, geode::index_t >,
            std::vector< geode::uuid > >
            overlaps_;
    };

    bool are_valid_regions(
        const geode::BRep& model, absl::Span< const geode::uuid > regions )
    {
        return std::all_of(
            regions.begin(), regions.end(), [&model]( const auto& region ) {
                return geode::internal::physical_problem_region_blocks(
                    model, region )
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

    // Items with valid regions covering no Block (invalid regions are
    // already errors)
    void warn_unused_items( const geode::PhysicalProblem& problem,
        const geode::BRep& model,
        const absl::flat_hash_map< geode::uuid, geode::index_t >&
            block_materials,
        const absl::flat_hash_map< geode::uuid, geode::index_t >&
            block_conditions,
        geode::SimulationReport& report )
    {
        const auto materials = problem.materials();
        const auto used_materials = used_items( block_materials );
        for( const auto material : geode::Indices{ materials } )
        {
            if( !used_materials.contains( material )
                && are_valid_regions( model, materials[material].regions ) )
            {
                report.add_warning( SIMULATION_CHECK_STAGE::regions,
                    material_subject( materials[material].name ),
                    "covers no Block" );
            }
        }
        const auto used_conditions = used_items( block_conditions );
        for( const auto condition :
            geode::Range{ problem.nb_initial_conditions() } )
        {
            if( !used_conditions.contains( condition )
                && are_valid_regions(
                    model, problem.initial_condition( condition ).regions() ) )
            {
                report.add_warning( SIMULATION_CHECK_STAGE::regions,
                    initial_condition_subject( condition ), "covers no Block" );
            }
        }
    }
} // namespace

namespace geode
{
    void SimulationChecker::check_physical_problem_regions()
    {
        const auto& problem = this->problem();
        const auto& model = this->model();
        auto& report = this->report();
        if( problem.model_id() && problem.model_id().value() != model.id() )
        {
            report.add_error( SIMULATION_CHECK_STAGE::regions, "Problem",
                "built for model ", problem.model_id()->string(),
                ", not for model ", model.id().string() );
        }
        if( !problem.has_flow() )
        {
            return;
        }
        const auto block_materials =
            internal::physical_problem_block_materials( problem, model );
        const auto block_conditions =
            internal::physical_problem_block_initial_conditions(
                problem, model );
        RegionCoverageChecker material_checker{ model, "material", report };
        const auto materials = problem.materials();
        for( const auto material_id : Indices{ materials } )
        {
            const auto& material = materials[material_id];
            material_checker.check( material_id,
                material_subject( material.name ), material.regions );
        }
        material_checker.report_overlaps();
        if( !materials.empty() )
        {
            material_checker.check_coverage( block_materials );
        }
        RegionCoverageChecker condition_checker{ model, "initial condition",
            report };
        for( const auto condition_id :
            Range{ problem.nb_initial_conditions() } )
        {
            condition_checker.check( condition_id,
                initial_condition_subject( condition_id ),
                problem.initial_condition( condition_id ).regions() );
        }
        condition_checker.report_overlaps();
        if( problem.nb_initial_conditions() != 0 )
        {
            condition_checker.check_coverage( block_conditions );
        }
        warn_unused_items(
            problem, model, block_materials, block_conditions, report );
    }
} // namespace geode
