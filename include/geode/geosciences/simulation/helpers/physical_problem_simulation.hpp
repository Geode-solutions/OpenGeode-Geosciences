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

#include <string>
#include <string_view>
#include <vector>

#include <absl/strings/str_cat.h>

#include <geode/basic/factory.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Component );
    ALIAS_3D( Component );
    class BRep;
    class PhysicalProblem;
} // namespace geode

namespace geode
{
    /*!
     * Result of a simulation check: errors prevent the simulation, warnings
     * point to data the simulation may not use as expected.
     */
    struct opengeode_geosciences_simulation_api SimulationReport
    {
        /// True if there is no error
        [[nodiscard]] bool is_simulable() const;

        /// Errors then warnings, one per line
        [[nodiscard]] std::string string() const;

        template < typename... Args >
        void add_error( const Args&... args )
        {
            errors.emplace_back( absl::StrCat( args... ) );
        }

        template < typename... Args >
        void add_warning( const Args&... args )
        {
            warnings.emplace_back( absl::StrCat( args... ) );
        }

        void add_errors( std::vector< std::string > issues );

        std::vector< std::string > errors;
        std::vector< std::string > warnings;
    };

    /*!
     * Checks that a simulator has everything it needs to simulate a problem
     * on a model. Implementations are registered in SimulationCheckerFactory
     * under the simulator name (lower case).
     */
    class opengeode_geosciences_simulation_api SimulationChecker
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( SimulationChecker );

    public:
        virtual ~SimulationChecker();

        /*!
         * Report every issue at once. Implementations start with
         * check_physical_problem_consistency and do not throw on data
         * issues.
         */
        [[nodiscard]] virtual SimulationReport check(
            const PhysicalProblem& problem, const BRep& model ) const = 0;

    protected:
        SimulationChecker();
    };

    using SimulationCheckerFactory = Factory< std::string, SimulationChecker >;

    /*!
     * Check that the given simulator can simulate the problem on the model.
     * The problem type (single phase or multiphase, isothermal or thermal) is
     * inferred from the problem.
     * @param[in] simulator Name of a simulator registered in
     * SimulationCheckerFactory (e.g. "geos"), case insensitive
     * @return Errors and warnings. An unknown simulator is an error.
     */
    [[nodiscard]] SimulationReport opengeode_geosciences_simulation_api
        check_physical_problem_simulation( const PhysicalProblem& problem,
            const BRep& model,
            std::string_view simulator );

    /// True if check_physical_problem_simulation reports no error
    [[nodiscard]] bool opengeode_geosciences_simulation_api
        is_physical_problem_simulable( const PhysicalProblem& problem,
            const BRep& model,
            std::string_view simulator );

    /*!
     * Simulator independent part of every check: validate_physical_problem
     * and validate_physical_problem_regions issues as errors, and a warning
     * for each material or initial condition covering no Block (items with
     * an invalid region are already reported as errors).
     */
    void opengeode_geosciences_simulation_api
        check_physical_problem_consistency( const PhysicalProblem& problem,
            const BRep& model,
            SimulationReport& report );

    /// Component type, name and uuid, for report messages
    [[nodiscard]] std::string opengeode_geosciences_simulation_api
        simulation_component_description( const Component3D& component );
} // namespace geode
