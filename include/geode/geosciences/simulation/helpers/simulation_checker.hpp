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

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>
#include <absl/types/span.h>

#include <geode/basic/factory.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/mixin/core/component_type.hpp>
#include <geode/model/mixin/core/physical_properties.hpp>

#include <geode/geosciences/simulation/common.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace geode
{
    class BRep;
    class PhysicalProblem;
} // namespace geode

namespace geode
{
    enum struct PHYSICAL_PROPERTY_VALUE_TYPE : std::uint8_t
    {
        /// double
        scalar,
        /// std::array< double, 3 >
        vector3
    };

    [[nodiscard]] std::string_view opengeode_geosciences_simulation_api
        physical_property_string( PHYSICAL_PROPERTY_NAME property );

    /*!
     * Component type simulators read the property on: Blocks (polyhedra) for
     * rock and initial stress properties, Surfaces (polygons) for boundary
     * conditions
     */
    [[nodiscard]] ComponentType opengeode_geosciences_simulation_api
        physical_property_component_type( PHYSICAL_PROPERTY_NAME property );

    /*!
     * True if a simulator of the problem reads the property: flow reads
     * porosity, permeability and flow boundary conditions (phase fractions
     * for multiphase flow only), solid mechanics reads elastic moduli,
     * initial stresses and mechanical boundary conditions, plus density
     * without flow, or biot_coefficient and grain_density with flow, and
     * thermal_expansion_coefficient with flow and thermal parameters.
     */
    [[nodiscard]] bool opengeode_geosciences_simulation_api
        is_physical_property_read(
            const PhysicalProblem& problem, PHYSICAL_PROPERTY_NAME property );

    struct opengeode_geosciences_simulation_api PhysicalPropertyRequirement
    {
        /// Value types simulators accept for this property
        explicit PhysicalPropertyRequirement(
            PHYSICAL_PROPERTY_NAME property_in );

        PHYSICAL_PROPERTY_NAME property;
        absl::InlinedVector< PHYSICAL_PROPERTY_VALUE_TYPE, 2 > value_types;
        /*!
         * If not empty, the property is needed on every element of every
         * meshed component: what needs it, e.g. "poromechanics"
         */
        std::string required_by;
        /// If false, elements without value are counted, not reported
        bool value_on_every_element{ true };
        /*!
         * Component meshes are merged into one mesh: the attribute must be on
         * every meshed component, transferable, named and of one type
         */
        bool merged{ false };
    };

    struct opengeode_geosciences_simulation_api PhysicalPropertyCoverage
    {
        explicit PhysicalPropertyCoverage( PHYSICAL_PROPERTY_NAME property_in );

        /// The component has a readable attribute with at least one value
        [[nodiscard]] bool carries( const uuid& component ) const;

        /// The component carries a value on every element
        [[nodiscard]] bool covers( const uuid& component ) const;

        /// At least one component carries the property
        [[nodiscard]] bool is_carried() const;

        PHYSICAL_PROPERTY_NAME property;
        /// Registered on the expected component type
        bool is_registered{ false };
        /// Carrying component -> number of its elements without value
        absl::flat_hash_map< uuid, index_t > components;
        /// Minimum and maximum of the valid values (every item)
        std::optional< std::array< double, 2 > > value_bounds;
    };

    /*!
     * Checks that a simulator has everything it needs to simulate a problem
     * on a model. The problem type (flow, solid mechanics or both, single
     * phase or multiphase, isothermal or thermal) is inferred from the
     * problem. Implementations are registered in SimulationCheckerFactory
     * under the simulator name in lower case (e.g. "geos"). The checker
     * keeps references to the problem and the model: they must outlive it.
     */
    class opengeode_geosciences_simulation_api SimulationChecker
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( SimulationChecker );

    public:
        virtual ~SimulationChecker();

        /*!
         * Report every issue at once: the simulator independent checks
         * (check_physical_problem_definition, check_physical_problem_regions
         * and check_physical_problem_properties), then the simulator ones.
         * The report is moved out of the checker: each call starts a new one.
         */
        [[nodiscard]] SimulationReport check();

    protected:
        SimulationChecker( const PhysicalProblem& problem, const BRep& model );

        [[nodiscard]] const PhysicalProblem& problem() const
        {
            return problem_;
        }

        [[nodiscard]] const BRep& model() const
        {
            return model_;
        }

        /// The report the checks add their issues to
        [[nodiscard]] SimulationReport& report()
        {
            return report_;
        }

        /*!
         * Check the problem consistency, independently of any simulator and
         * model, as errors in the problem stage. Every issue is reported,
         * except those depending on a missing part (e.g. compositions without
         * fluid). The flow model and the fluid must match: SinglePhaseFluid
         * has one phase, DeadOilFluid and Co2BrineFluid are multiphase. Only
         * the content of the leaf classes defined in this library is checked:
         * leaf classes registered by other libraries are accepted as is.
         * Materials of solid mechanics without flow are reported as ignored.
         */
        void check_physical_problem_definition();

        /*!
         * Check, in the regions stage, that the recorded model id, if any,
         * matches and, with a flow model, that every region referenced by the
         * problem is a Block or a collection of Blocks of the model
         * (BlockCollection, StratigraphicUnit...) and that every Block is
         * covered by exactly one material and one initial condition. Coverage
         * is not checked without any material or initial condition:
         * check_physical_problem_definition reports it. Materials and initial
         * conditions covering no Block are warnings.
         */
        void check_physical_problem_regions();

        /*!
         * Check, in the properties stage, the model physical properties that
         * do not depend on the simulator. Properties the problem does not read
         * are ignored (warning), except boundary conditions of a missing
         * process and the bulk density with poromechanics (errors). For solid
         * mechanics: positive elastic moduli on every Block, non-negative
         * density (grain density with flow) on every Block when there is
         * gravity, finite initial stresses, displacements uniform on each
         * Surface and imposed along each axis, finite tractions. For
         * poromechanics: Biot coefficient in (0, 1] on every Block and not
         * below the porosity, and the thermal expansion coefficient on every
         * Block of thermal problems. Warns if the mean initial stress is a
         * tension in most cells, which suggests the opposite sign convention
         * than the one declared by the problem.
         */
        void check_physical_problem_properties();

        /*!
         * Every Block is meshed and its polyhedron vertices are unique
         * vertices. Issues are in the mesh stage.
         */
        void check_blocks();

        /// Every unique vertex is a Block vertex, as convert_brep_into_solid
        /// needs
        void check_unique_vertices_in_blocks();

        /*!
         * Check the property attribute on every component of the expected
         * type: registered component type, element (not vertex) attribute,
         * value type, values on every element if required, and value range
         * (e.g. porosity in (0, 1], positive elastic moduli, finite stresses
         * and pressures, fractions in [0, 1]). An unregistered property is
         * an issue only if required. Issues are in the properties stage.
         */
        PhysicalPropertyCoverage check_physical_property(
            const PhysicalPropertyRequirement& requirement );

        /*!
         * Error for each material lacking the porosity or permeability value
         * of each coverage property on a meshed Block the model does not
         * cover. The coverage properties must be porosity or permeability.
         */
        void check_rock_property_availability(
            absl::Span< const PhysicalPropertyCoverage > coverages );

        /// Warning, followed by the consequence, if no component carries it
        void check_physical_property_presence(
            const PhysicalPropertyCoverage& coverage,
            std::string_view consequence );

        /*!
         * Warning, in the simulator stage, for each registered property the
         * report simulator ignores although the problem reads it
         */
        void warn_ignored_physical_properties(
            absl::Span< const PHYSICAL_PROPERTY_NAME > properties );

    private:
        /*!
         * Set the report simulator name, then add the simulator issues,
         * without throwing on data issues
         */
        virtual void check_simulator() = 0;

        const PhysicalProblem& problem_;
        const BRep& model_;
        SimulationReport report_;
    };

    using SimulationCheckerFactory = Factory< std::string,
        SimulationChecker,
        const PhysicalProblem&,
        const BRep& >;
} // namespace geode
