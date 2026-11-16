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

#pragma once

#include <pybind11/native_enum.h>

#include <geode/model/mixin/core/component.hpp>

#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace geode
{
    void define_simulation_report( pybind11::module& module )
    {
        pybind11::native_enum< SIMULATION_ISSUE_SEVERITY >( module,
            "SimulationIssueSeverity", "enum.Enum",
            "Severity of an issue: an error prevents the simulation, a "
            "warning points to data the simulation ignores or that is "
            "probably wrong" )
            .value( "error", SIMULATION_ISSUE_SEVERITY::error )
            .value( "warning", SIMULATION_ISSUE_SEVERITY::warning )
            .finalize();
        pybind11::native_enum< SIMULATION_CHECK_STAGE >( module,
            "SimulationCheckStage", "enum.Enum",
            "Part of the check that found an issue, in report order: problem "
            "(fluid, materials, initial conditions, schedule, without the "
            "model), regions (the Blocks covered by the materials and the "
            "initial conditions), mesh (the component meshes), properties "
            "(the physical properties of the model) and simulator (what the "
            "simulator does not support or ignores)" )
            .value( "problem", SIMULATION_CHECK_STAGE::problem )
            .value( "regions", SIMULATION_CHECK_STAGE::regions )
            .value( "mesh", SIMULATION_CHECK_STAGE::mesh )
            .value( "properties", SIMULATION_CHECK_STAGE::properties )
            .value( "simulator", SIMULATION_CHECK_STAGE::simulator )
            .finalize();
        pybind11::native_enum< SIMULATION_ELEMENT_TYPE >( module,
            "SimulationElementType", "enum.Enum",
            "Type of the mesh elements of a SimulationIssueLocation; none "
            "when the issue is about the component, not some of its "
            "elements" )
            .value( "none", SIMULATION_ELEMENT_TYPE::none )
            .value( "vertex", SIMULATION_ELEMENT_TYPE::vertex )
            .value( "polygon", SIMULATION_ELEMENT_TYPE::polygon )
            .value( "polyhedron", SIMULATION_ELEMENT_TYPE::polyhedron )
            .finalize();
        module.def( "simulation_check_stage_string",
            &simulation_check_stage_string, pybind11::arg( "stage" ),
            "Return the title of the SimulationCheckStage in the reports, "
            "e.g. Region assignment" );

        pybind11::class_< SimulationIssueLocation > location_class( module,
            "SimulationIssueLocation",
            "Model component, and some of its mesh elements, involved in an "
            "issue" );
        define_string_representation( location_class,
            []( const SimulationIssueLocation& location ) {
                return "SimulationIssueLocation(component="
                       + python_repr( location.component ) + ", element_type="
                       + python_repr( location.element_type ) + ", nb_elements="
                       + python_repr( location.nb_elements ) + ")";
            } )
            .def( pybind11::init<>(), "Build an empty location" );
        def_readwrite_named( location_class, "component",
            &SimulationIssueLocation::component,
            "ComponentID of the component of the issue" );
        def_readwrite_named( location_class, "name",
            &SimulationIssueLocation::name,
            "Name of the component, empty if it has none" );
        def_readwrite_named( location_class, "element_type",
            &SimulationIssueLocation::element_type,
            "SimulationElementType of the elements" );
        def_readwrite_named( location_class, "elements",
            &SimulationIssueLocation::elements,
            "Indices of the elements involved, empty if all_elements" );
        def_readwrite_named( location_class, "all_elements",
            &SimulationIssueLocation::all_elements,
            "True if every element of this type is involved, they are not "
            "listed" );
        def_readwrite_named( location_class, "nb_elements",
            &SimulationIssueLocation::nb_elements,
            "Number of elements of this type in the mesh of the component" );
        def_readwrite_named( location_class, "detail",
            &SimulationIssueLocation::detail,
            "Optional precision, e.g. no attribute" );

        pybind11::class_< SimulationIssue > issue_class( module,
            "SimulationIssue",
            "Issue found when checking that a simulator can simulate a "
            "problem on a model: its severity, stage, subject, message, "
            "optional fix and locations" );
        define_string_representation( issue_class,
            []( const SimulationIssue& issue ) {
                return "SimulationIssue(severity="
                       + python_repr( issue.severity )
                       + ", stage=" + python_repr( issue.stage )
                       + ", subject=" + python_repr( issue.subject )
                       + ", message=" + python_repr( issue.message ) + ")";
            } )
            .def( "add_location", &SimulationIssue::add_location,
                pybind11::arg( "component" ),
                pybind11::return_value_policy::reference_internal,
                "Add a location on the whole Component3D and return it" )
            .def( "is_error", &SimulationIssue::is_error,
                "Return True if the issue is an error" )
            .def_readonly( "severity", &SimulationIssue::severity,
                "SimulationIssueSeverity of the issue" )
            .def_readonly( "stage", &SimulationIssue::stage,
                "SimulationCheckStage that found the issue" )
            .def_readonly( "subject", &SimulationIssue::subject,
                "What the issue is about: a problem item, a physical "
                "property name..." )
            .def_readonly(
                "message", &SimulationIssue::message, "Message of the issue" )
            .def_readonly( "fix", &SimulationIssue::fix,
                "How to solve the issue, empty if it is not obvious" )
            .def_readonly( "locations", &SimulationIssue::locations,
                "SimulationIssueLocations of the issue" );

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
            pybind11::class_< SimulationReport >( module, "SimulationReport",
                "Every issue found when checking that a simulator can "
                "simulate a problem on a model; the problem is simulable if "
                "there is no error" ),
            []( const SimulationReport& report ) {
                return "SimulationReport(simulator="
                       + python_repr( std::string{ report.simulator() } )
                       + ", model_name="
                       + python_repr( std::string{ report.model_name() } )
                       + ", nb_errors=" + python_repr( report.nb_errors() )
                       + ", nb_warnings=" + python_repr( report.nb_warnings() )
                       + ")";
            } )
            .def( pybind11::init<>(), "Build an empty report" )
            .def( "simulator", &SimulationReport::simulator,
                "Return the name of the simulator that checked, e.g. GEOS; "
                "empty for the simulator independent checks" )
            .def( "set_simulator", &SimulationReport::set_simulator,
                pybind11::arg( "simulator" ),
                "Set the name of the simulator that checked" )
            .def( "model_name", &SimulationReport::model_name,
                "Return the name of the model, empty if not set" )
            .def( "set_model_name", &SimulationReport::set_model_name,
                pybind11::arg( "model_name" ), "Set the name of the model" )
            .def( "add_issue", add_issue, pybind11::arg( "severity" ),
                pybind11::arg( "stage" ), pybind11::arg( "subject" ),
                pybind11::arg( "message" ), pybind11::arg( "fix" ) = "",
                pybind11::arg( "locations" ) =
                    std::vector< SimulationIssueLocation >{},
                "Add an issue with its severity, stage, subject and "
                "message, and optionally how to fix it and its locations" )
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
                    std::vector< SimulationIssueLocation >{},
                "Add an error with its stage, subject and message, and "
                "optionally how to fix it and its locations" )
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
                    std::vector< SimulationIssueLocation >{},
                "Add a warning with its stage, subject and message, and "
                "optionally how to fix it and its locations" )
            .def( "is_simulable", &SimulationReport::is_simulable,
                "Return True if the report has no error" )
            .def( "nb_errors", &SimulationReport::nb_errors,
                "Return the number of errors" )
            .def( "nb_warnings", &SimulationReport::nb_warnings,
                "Return the number of warnings" )
            .def(
                "issues",
                []( const SimulationReport& report ) {
                    return report.issues();
                },
                "Return a list of copies of the issues, in the order they "
                "were added" )
            .def(
                "stage_issues",
                []( const SimulationReport& report,
                    SIMULATION_CHECK_STAGE stage ) {
                    std::vector< SimulationIssue > issues;
                    for( const auto* issue : report.issues( stage ) )
                    {
                        issues.push_back( *issue );
                    }
                    return issues;
                },
                pybind11::arg( "stage" ),
                "Return a list of copies of the issues of the "
                "SimulationCheckStage" );
    }
} // namespace geode
