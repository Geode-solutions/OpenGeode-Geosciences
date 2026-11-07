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

namespace geode
{
    void define_simulation_report( pybind11::module& module )
    {
        pybind11::enum_< SIMULATION_ISSUE_SEVERITY >(
            module, "SIMULATION_ISSUE_SEVERITY" )
            .value( "error", SIMULATION_ISSUE_SEVERITY::error )
            .value( "warning", SIMULATION_ISSUE_SEVERITY::warning )
            .export_values();

        pybind11::enum_< SIMULATION_CHECK_STAGE >(
            module, "SIMULATION_CHECK_STAGE" )
            .value( "problem", SIMULATION_CHECK_STAGE::problem )
            .value( "regions", SIMULATION_CHECK_STAGE::regions )
            .value( "mesh", SIMULATION_CHECK_STAGE::mesh )
            .value( "properties", SIMULATION_CHECK_STAGE::properties )
            .value( "simulator", SIMULATION_CHECK_STAGE::simulator )
            .export_values();

        pybind11::enum_< SIMULATION_ELEMENT_TYPE >(
            module, "SIMULATION_ELEMENT_TYPE" )
            .value( "none", SIMULATION_ELEMENT_TYPE::none )
            .value( "vertex", SIMULATION_ELEMENT_TYPE::vertex )
            .value( "polygon", SIMULATION_ELEMENT_TYPE::polygon )
            .value( "polyhedron", SIMULATION_ELEMENT_TYPE::polyhedron )
            .export_values();

        define_string_representation(
            pybind11::class_< SimulationIssueLocation >(
                module, "SimulationIssueLocation" ) )
            .def( pybind11::init<>() )
            .def_readwrite( "component", &SimulationIssueLocation::component )
            .def_readwrite( "name", &SimulationIssueLocation::name )
            .def_readwrite(
                "element_type", &SimulationIssueLocation::element_type )
            .def_readwrite( "elements", &SimulationIssueLocation::elements )
            .def_readwrite(
                "all_elements", &SimulationIssueLocation::all_elements )
            .def_readwrite(
                "nb_elements", &SimulationIssueLocation::nb_elements )
            .def_readwrite( "detail", &SimulationIssueLocation::detail );

        define_string_representation(
            pybind11::class_< SimulationIssue >( module, "SimulationIssue" ) )
            .def_readonly( "severity", &SimulationIssue::severity )
            .def_readonly( "stage", &SimulationIssue::stage )
            .def_readonly( "subject", &SimulationIssue::subject )
            .def_readonly( "message", &SimulationIssue::message )
            .def_readonly( "fix", &SimulationIssue::fix )
            .def_readonly( "locations", &SimulationIssue::locations )
            .def( "is_error", &SimulationIssue::is_error );

        const auto add_issue =
            []( SimulationReport& report, SIMULATION_ISSUE_SEVERITY severity,
                SIMULATION_CHECK_STAGE stage, std::string subject,
                std::string message, std::string fix,
                std::vector< SimulationIssueLocation > locations ) {
                SimulationIssue issue{ severity, stage, std::move( subject ),
                    std::move( message ) };
                issue.fix = std::move( fix );
                issue.locations = std::move( locations );
                report.add_issue( std::move( issue ) );
            };
        define_string_representation(
            pybind11::class_< SimulationReport >( module, "SimulationReport" ) )
            .def( pybind11::init<>() )
            .def( "simulator", &SimulationReport::simulator )
            .def( "set_simulator", &SimulationReport::set_simulator )
            .def( "model_name", &SimulationReport::model_name )
            .def( "set_model_name", &SimulationReport::set_model_name )
            .def( "add_issue", add_issue, pybind11::arg( "severity" ),
                pybind11::arg( "stage" ), pybind11::arg( "subject" ),
                pybind11::arg( "message" ), pybind11::arg( "fix" ) = "",
                pybind11::arg( "locations" ) =
                    std::vector< SimulationIssueLocation >{} )
            .def(
                "add_error",
                [add_issue]( SimulationReport& report,
                    SIMULATION_CHECK_STAGE stage, std::string subject,
                    std::string message, std::string fix,
                    std::vector< SimulationIssueLocation > locations ) {
                    add_issue( report, SIMULATION_ISSUE_SEVERITY::error, stage,
                        std::move( subject ), std::move( message ),
                        std::move( fix ), std::move( locations ) );
                },
                pybind11::arg( "stage" ), pybind11::arg( "subject" ),
                pybind11::arg( "message" ), pybind11::arg( "fix" ) = "",
                pybind11::arg( "locations" ) =
                    std::vector< SimulationIssueLocation >{} )
            .def(
                "add_warning",
                [add_issue]( SimulationReport& report,
                    SIMULATION_CHECK_STAGE stage, std::string subject,
                    std::string message, std::string fix,
                    std::vector< SimulationIssueLocation > locations ) {
                    add_issue( report, SIMULATION_ISSUE_SEVERITY::warning,
                        stage, std::move( subject ), std::move( message ),
                        std::move( fix ), std::move( locations ) );
                },
                pybind11::arg( "stage" ), pybind11::arg( "subject" ),
                pybind11::arg( "message" ), pybind11::arg( "fix" ) = "",
                pybind11::arg( "locations" ) =
                    std::vector< SimulationIssueLocation >{} )
            .def( "is_simulable", &SimulationReport::is_simulable )
            .def( "nb_errors", &SimulationReport::nb_errors )
            .def( "nb_warnings", &SimulationReport::nb_warnings )
            .def( "issues",
                []( const SimulationReport& report ) {
                    return report.issues();
                } )
            .def( "stage_issues", []( const SimulationReport& report,
                                      SIMULATION_CHECK_STAGE stage ) {
                std::vector< SimulationIssue > issues;
                for( const auto* issue : report.issues( stage ) )
                {
                    issues.push_back( *issue );
                }
                return issues;
            } );
    }
} // namespace geode
