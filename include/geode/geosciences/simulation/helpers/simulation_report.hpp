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

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <absl/strings/str_cat.h>

#include <geode/model/mixin/core/component_type.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Component );
    ALIAS_3D( Component );
} // namespace geode

namespace geode
{
    /*!
     * An error prevents the simulation. A warning points to data the
     * simulation ignores or that is probably wrong, the simulation can run.
     */
    enum struct SIMULATION_ISSUE_SEVERITY : std::uint8_t
    {
        error,
        warning
    };

    /// Part of the check that found an issue, in report order
    enum struct SIMULATION_CHECK_STAGE : std::uint8_t
    {
        /// Fluid, materials, initial conditions, schedule... (no model)
        problem,
        /// Blocks covered by the materials and initial conditions
        regions,
        /// Component meshes
        mesh,
        /// Model physical properties
        properties,
        /// What the simulator does not support or ignores
        simulator
    };

    /// Type of the mesh elements of a SimulationIssueLocation
    enum struct SIMULATION_ELEMENT_TYPE : std::uint8_t
    {
        /// The issue is about the component, not some of its elements
        none,
        vertex,
        polygon,
        polyhedron
    };

    /// Model component, and some of its mesh elements, involved in an issue
    struct opengeode_geosciences_simulation_api SimulationIssueLocation
    {
        /// Component type, name (if any) and elements, on one line
        [[nodiscard]] std::string string() const;

        ComponentID component;
        /// Component name, empty if it has none
        std::string name;
        SIMULATION_ELEMENT_TYPE element_type{ SIMULATION_ELEMENT_TYPE::none };
        /// Every element involved, empty if all_elements
        std::vector< index_t > elements;
        /// Every element of this type is involved, they are not listed
        bool all_elements{ false };
        /// Number of elements of this type in the component mesh
        index_t nb_elements{ 0 };
        /// Optional precision, e.g. "no attribute" or "first value -1"
        std::string detail;
    };

    struct opengeode_geosciences_simulation_api SimulationIssue
    {
        SimulationIssue() = default;
        SimulationIssue( SIMULATION_ISSUE_SEVERITY severity_in,
            SIMULATION_CHECK_STAGE stage_in,
            std::string subject_in,
            std::string message_in );

        /// Add a location on the whole component
        SimulationIssueLocation& add_location( const Component3D& component );

        [[nodiscard]] bool is_error() const;

        /// The issue, its locations and its fix, one per line
        [[nodiscard]] std::string string() const;

        SIMULATION_ISSUE_SEVERITY severity{ SIMULATION_ISSUE_SEVERITY::error };
        SIMULATION_CHECK_STAGE stage{ SIMULATION_CHECK_STAGE::problem };
        /*!
         * What the issue is about: a problem item (e.g. Material "Sand",
         * Initial condition #1), a physical property name...
         */
        std::string subject;
        std::string message;
        /// How to solve the issue, empty if not obvious
        std::string fix;
        std::vector< SimulationIssueLocation > locations;
    };

    /*!
     * Every issue found when checking that a simulator can simulate a
     * problem on a model. The problem is simulable if there is no error.
     */
    class opengeode_geosciences_simulation_api SimulationReport
    {
    public:
        /// Empty for the simulator independent checks
        [[nodiscard]] std::string_view simulator() const;

        /// Set by each simulator checker, e.g. "GEOS"
        void set_simulator( std::string simulator );

        [[nodiscard]] std::string_view model_name() const;

        void set_model_name( std::string model_name );

        [[nodiscard]] bool is_simulable() const;

        [[nodiscard]] index_t nb_errors() const;

        [[nodiscard]] index_t nb_warnings() const;

        [[nodiscard]] const std::vector< SimulationIssue >& issues() const;

        [[nodiscard]] std::vector< const SimulationIssue* > issues(
            SIMULATION_CHECK_STAGE stage ) const;

        /*!
         * Verdict line, then issues grouped by stage. Element lists are
         * truncated, use issues() for every element.
         */
        [[nodiscard]] std::string string() const;

        /// Error without fix nor location, the message pieces are
        /// concatenated
        template < typename... Args >
        void add_error( SIMULATION_CHECK_STAGE stage,
            std::string subject,
            const Args&... message )
        {
            add_issue( SimulationIssue{ SIMULATION_ISSUE_SEVERITY::error, stage,
                std::move( subject ), absl::StrCat( message... ) } );
        }

        /// Warning without fix nor location, the message pieces are
        /// concatenated
        template < typename... Args >
        void add_warning( SIMULATION_CHECK_STAGE stage,
            std::string subject,
            const Args&... message )
        {
            add_issue( SimulationIssue{ SIMULATION_ISSUE_SEVERITY::warning,
                stage, std::move( subject ), absl::StrCat( message... ) } );
        }

        void add_issue( SimulationIssue issue );

    private:
        std::string simulator_;
        std::string model_name_;
        std::vector< SimulationIssue > issues_;
    };

    /// Title of the stage in reports, e.g. "Region assignment"
    [[nodiscard]] std::string_view opengeode_geosciences_simulation_api
        simulation_check_stage_string( SIMULATION_CHECK_STAGE stage );
} // namespace geode
