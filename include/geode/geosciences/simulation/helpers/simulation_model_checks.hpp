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
#include <optional>
#include <string_view>

#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>
#include <absl/types/span.h>

#include <geode/basic/uuid.hpp>

#include <geode/model/mixin/core/component_type.hpp>
#include <geode/model/mixin/core/physical_properties.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace geode
{
    class BRep;
    class PhysicalProblem;
    struct SimulationReport;
} // namespace geode

namespace geode
{
    /// Every Block is meshed and its polyhedron vertices are unique vertices
    void opengeode_geosciences_simulation_api check_simulation_blocks(
        const BRep& model, SimulationReport& report );

    /// Every polygon vertex of the given Surfaces is a Block polyhedron vertex
    void opengeode_geosciences_simulation_api
        check_simulation_surfaces_on_blocks( const BRep& model,
            absl::Span< const uuid > surfaces,
            SimulationReport& report );

    /// Every unique vertex is a Block vertex, as convert_brep_into_solid needs
    void opengeode_geosciences_simulation_api
        check_simulation_unique_vertices_in_blocks(
            const BRep& model, SimulationReport& report );

    enum struct PHYSICAL_PROPERTY_VALUE_TYPE
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
     * porosity and permeability, Surfaces (polygons) for boundary conditions
     */
    [[nodiscard]] ComponentType opengeode_geosciences_simulation_api
        physical_property_component_type( PHYSICAL_PROPERTY_NAME property );

    struct opengeode_geosciences_simulation_api PhysicalPropertyRequirement
    {
        /// Value types accepted for this property
        explicit PhysicalPropertyRequirement(
            PHYSICAL_PROPERTY_NAME property_in );

        PHYSICAL_PROPERTY_NAME property;
        absl::InlinedVector< PHYSICAL_PROPERTY_VALUE_TYPE, 2 > value_types;
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
     * Check the property attribute on every component of the expected type:
     * registered component type, element (not vertex) attribute, value type,
     * values on every element if required, and value range (porosity in
     * (0, 1], non-negative permeability, finite pressure, positive
     * temperature, fractions in [0, 1]). An unregistered property is not an
     * issue.
     */
    [[nodiscard]] PhysicalPropertyCoverage opengeode_geosciences_simulation_api
        check_physical_property( const BRep& model,
            const PhysicalPropertyRequirement& requirement,
            SimulationReport& report );

    /*!
     * Error for each material lacking the porosity or permeability value of
     * the coverage property on a meshed Block the model does not cover
     */
    void opengeode_geosciences_simulation_api check_rock_property_availability(
        const PhysicalProblem& problem,
        const BRep& model,
        const PhysicalPropertyCoverage& coverage,
        SimulationReport& report );

    /// Error for each component carrying some but not all of the properties
    void opengeode_geosciences_simulation_api
        check_coinciding_physical_properties( const BRep& model,
            absl::Span< const PhysicalPropertyCoverage > coverages,
            SimulationReport& report );

    /// Warning, followed by the consequence, if no component carries it
    void opengeode_geosciences_simulation_api check_physical_property_presence(
        const BRep& model,
        const PhysicalPropertyCoverage& coverage,
        std::string_view consequence,
        SimulationReport& report );

    /// Warning for each registered property the simulator ignores
    void opengeode_geosciences_simulation_api warn_ignored_physical_properties(
        const BRep& model,
        absl::Span< const PHYSICAL_PROPERTY_NAME > properties,
        std::string_view simulator,
        SimulationReport& report );
} // namespace geode
