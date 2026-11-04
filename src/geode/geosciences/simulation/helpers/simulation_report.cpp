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

#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

#include <algorithm>
#include <array>

#include <absl/algorithm/container.h>
#include <absl/strings/str_join.h>
#include <absl/types/span.h>

#include <geode/basic/range.hpp>

#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>

namespace
{
    // Elements and locations listed in report texts
    constexpr geode::index_t MAX_LISTED_ELEMENTS{ 5 };
    constexpr geode::index_t MAX_LISTED_LOCATIONS{ 10 };

    constexpr std::array< geode::SIMULATION_CHECK_STAGE, 5 > STAGES{
        geode::SIMULATION_CHECK_STAGE::problem,
        geode::SIMULATION_CHECK_STAGE::regions,
        geode::SIMULATION_CHECK_STAGE::mesh,
        geode::SIMULATION_CHECK_STAGE::properties,
        geode::SIMULATION_CHECK_STAGE::simulator
    };

    std::string_view element_type_string(
        geode::SIMULATION_ELEMENT_TYPE type, bool plural )
    {
        switch( type )
        {
            case geode::SIMULATION_ELEMENT_TYPE::vertex:
                return plural ? "vertices" : "vertex";
            case geode::SIMULATION_ELEMENT_TYPE::polygon:
                return plural ? "polygons" : "polygon";
            case geode::SIMULATION_ELEMENT_TYPE::polyhedron:
                return plural ? "polyhedra" : "polyhedron";
            case geode::SIMULATION_ELEMENT_TYPE::none:
                return "";
        }
        return "";
    }

    // "12 of 400 polygons (3, 4, 17, 18, 40, ...)"
    std::string elements_string(
        const geode::SimulationIssueLocation& location )
    {
        const auto nb_listed =
            static_cast< geode::index_t >( location.elements.size() );
        std::string message;
        if( nb_listed == location.nb_elements )
        {
            absl::StrAppend( &message, nb_listed == 1 ? "its " : "all ",
                nb_listed == 1 ? "" : absl::StrCat( nb_listed, " " ),
                element_type_string( location.element_type, nb_listed != 1 ) );
        }
        else if( location.nb_elements != 0 )
        {
            absl::StrAppend( &message, nb_listed, " of ", location.nb_elements,
                " ",
                element_type_string(
                    location.element_type, location.nb_elements != 1 ) );
        }
        else
        {
            absl::StrAppend( &message, nb_listed, " ",
                element_type_string( location.element_type, nb_listed != 1 ) );
        }
        if( nb_listed == location.nb_elements && nb_listed > 1 )
        {
            return message;
        }
        const auto listed =
            absl::MakeConstSpan( location.elements )
                .first( std::min( nb_listed, MAX_LISTED_ELEMENTS ) );
        absl::StrAppend( &message, " (", absl::StrJoin( listed, ", " ),
            nb_listed > MAX_LISTED_ELEMENTS ? ", ..." : "", ")" );
        return message;
    }

    // "3 errors, 1 warning", empty without issue
    std::string issue_counts( const geode::SimulationReport& report )
    {
        std::vector< std::string > counts;
        for( const auto& [count, name] :
            { std::pair{ report.nb_errors(), "error" },
                std::pair{ report.nb_warnings(), "warning" } } )
        {
            if( count != 0 )
            {
                counts.push_back(
                    geode::internal::count_string( count, name ) );
            }
        }
        return absl::StrJoin( counts, ", " );
    }

    std::string verdict( const geode::SimulationReport& report )
    {
        const auto* ability = report.is_simulable() ? "can" : "cannot";
        auto message =
            report.simulator().empty()
                ? absl::StrCat( "This problem ", ability, " be simulated" )
                : absl::StrCat( report.simulator(), " ", ability,
                    " simulate this problem" );
        if( !report.model_name().empty() )
        {
            absl::StrAppend( &message, " on model ", report.model_name() );
        }
        const auto counts = issue_counts( report );
        if( !counts.empty() )
        {
            absl::StrAppend( &message, ": ", counts );
        }
        return message;
    }
} // namespace

namespace geode
{
    std::string SimulationIssueLocation::string() const
    {
        auto message = name.empty() ? component.string()
                                    : absl::StrCat( component.type.get(), " \"",
                                        name, "\"" );
        std::vector< std::string > precisions;
        if( element_type != SIMULATION_ELEMENT_TYPE::none && !elements.empty() )
        {
            precisions.push_back( elements_string( *this ) );
        }
        if( !detail.empty() )
        {
            precisions.push_back( detail );
        }
        if( !precisions.empty() )
        {
            absl::StrAppend(
                &message, ": ", absl::StrJoin( precisions, ", " ) );
        }
        return message;
    }

    SimulationIssue::SimulationIssue( SIMULATION_ISSUE_SEVERITY severity_in,
        SIMULATION_CHECK_STAGE stage_in,
        std::string subject_in,
        std::string message_in )
        : severity( severity_in ),
          stage( stage_in ),
          subject( std::move( subject_in ) ),
          message( std::move( message_in ) )
    {
    }

    SimulationIssueLocation& SimulationIssue::add_location(
        const Component3D& component )
    {
        return locations.emplace_back(
            internal::component_location( component ) );
    }

    bool SimulationIssue::is_error() const
    {
        return severity == SIMULATION_ISSUE_SEVERITY::error;
    }

    std::string SimulationIssue::string() const
    {
        auto text = absl::StrCat( is_error() ? "✗ " : "! ", subject,
            subject.empty() ? "" : ": ", message );
        const auto nb_listed = std::min(
            static_cast< index_t >( locations.size() ), MAX_LISTED_LOCATIONS );
        for( const auto location : Range{ nb_listed } )
        {
            absl::StrAppend( &text, "\n    ", locations[location].string() );
        }
        if( locations.size() > nb_listed )
        {
            absl::StrAppend( &text, "\n    ... and ",
                locations.size() - nb_listed, " more components" );
        }
        if( !fix.empty() )
        {
            absl::StrAppend( &text, "\n    fix: ", fix );
        }
        return text;
    }

    std::string_view SimulationReport::simulator() const
    {
        return simulator_;
    }

    void SimulationReport::set_simulator( std::string simulator )
    {
        simulator_ = std::move( simulator );
    }

    std::string_view SimulationReport::model_name() const
    {
        return model_name_;
    }

    void SimulationReport::set_model_name( std::string model_name )
    {
        model_name_ = std::move( model_name );
    }

    bool SimulationReport::is_simulable() const
    {
        return nb_errors() == 0;
    }

    index_t SimulationReport::nb_errors() const
    {
        return static_cast< index_t >(
            absl::c_count_if( issues_, []( const SimulationIssue& issue ) {
                return issue.is_error();
            } ) );
    }

    index_t SimulationReport::nb_warnings() const
    {
        return static_cast< index_t >( issues_.size() ) - nb_errors();
    }

    const std::vector< SimulationIssue >& SimulationReport::issues() const
    {
        return issues_;
    }

    std::vector< const SimulationIssue* > SimulationReport::issues(
        SIMULATION_CHECK_STAGE stage ) const
    {
        std::vector< const SimulationIssue* > stage_issues;
        for( const auto& issue : issues_ )
        {
            if( issue.stage == stage )
            {
                stage_issues.push_back( &issue );
            }
        }
        return stage_issues;
    }

    std::string SimulationReport::string() const
    {
        auto message = verdict( *this );
        for( const auto stage : STAGES )
        {
            auto stage_issues = issues( stage );
            if( stage_issues.empty() )
            {
                continue;
            }
            // Errors first, each severity in check order
            std::stable_sort( stage_issues.begin(), stage_issues.end(),
                []( const SimulationIssue* first,
                    const SimulationIssue* second ) {
                    return first->is_error() && !second->is_error();
                } );
            absl::StrAppend(
                &message, "\n\n", simulation_check_stage_string( stage ) );
            for( const auto* issue : stage_issues )
            {
                absl::StrAppend( &message, "\n  ", issue->string() );
            }
        }
        return message;
    }

    void SimulationReport::add_issue( SimulationIssue issue )
    {
        issues_.push_back( std::move( issue ) );
    }

    std::string_view simulation_check_stage_string(
        SIMULATION_CHECK_STAGE stage )
    {
        switch( stage )
        {
            case SIMULATION_CHECK_STAGE::problem:
                return "Problem definition";
            case SIMULATION_CHECK_STAGE::regions:
                return "Region assignment";
            case SIMULATION_CHECK_STAGE::mesh:
                return "Model mesh";
            case SIMULATION_CHECK_STAGE::properties:
                return "Physical properties";
            case SIMULATION_CHECK_STAGE::simulator:
                return "Simulator support";
        }
        return "Unknown stage";
    }
} // namespace geode
