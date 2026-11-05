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

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include <absl/types/span.h>

#include <geode/basic/attribute_manager.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/component_type.hpp>
#include <geode/model/mixin/core/physical_properties.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/common.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace geode
{
    class PhysicalProblem;
} // namespace geode

// Helpers shared by the SimulationChecker implementation files and the
// simulator checkers
namespace geode::internal
{
    // Mesh elements of a component carrying a physical property
    struct ComponentElements
    {
        const geode::Component3D& component;
        const geode::AttributeManager& elements;
        const geode::AttributeManager& vertices;
        geode::index_t nb_elements;
        SIMULATION_ELEMENT_TYPE element_type;
    };

    template < typename Action >
    void for_each_component( const geode::BRep& model,
        const geode::ComponentType& type,
        const Action& action )
    {
        if( type == geode::Block3D::component_type_static() )
        {
            for( const auto& block : model.blocks() )
            {
                const auto& mesh = block.mesh();
                action( ComponentElements{ block,
                    mesh.polyhedron_attribute_manager(),
                    mesh.vertex_attribute_manager(), mesh.nb_polyhedra(),
                    SIMULATION_ELEMENT_TYPE::polyhedron } );
            }
            return;
        }
        for( const auto& surface : model.surfaces() )
        {
            const auto& mesh = surface.mesh();
            action(
                ComponentElements{ surface, mesh.polygon_attribute_manager(),
                    mesh.vertex_attribute_manager(), mesh.nb_polygons(),
                    SIMULATION_ELEMENT_TYPE::polygon } );
        }
    }

    [[nodiscard]] SimulationIssueLocation opengeode_geosciences_simulation_api
        component_location( const Component3D& component );

    // "2 Blocks", "1 Block"
    [[nodiscard]] std::string count_string(
        std::size_t count, std::string_view noun );

    [[nodiscard]] bool is_positive( double value );

    [[nodiscard]] bool is_non_negative( double value );

    [[nodiscard]] bool opengeode_geosciences_simulation_api is_multiphase(
        const PhysicalProblem& problem );

    [[nodiscard]] std::string opengeode_geosciences_simulation_api
        material_subject( std::string_view name );

    [[nodiscard]] std::string opengeode_geosciences_simulation_api
        initial_condition_subject( index_t condition );

    // Problems reading a physical property
    enum struct PROPERTY_READER : std::uint8_t
    {
        flow,
        multiphase_flow,
        solid_mechanics,
        solid_mechanics_without_flow,
        poromechanics,
        thermal_poromechanics
    };

    // e.g. "poromechanics", for "ignored" warnings
    [[nodiscard]] std::string_view property_reader_string(
        PROPERTY_READER reader );

    [[nodiscard]] bool reads(
        PROPERTY_READER reader, const PhysicalProblem& problem );

    enum struct PROPERTY_VALUE_TYPES : std::uint8_t
    {
        scalar,
        vector3,
        scalar_or_vector3
    };

    struct ValidRange
    {
        bool ( *contains )( double value );
        // Values the range does not contain, e.g. "values outside (0, 1]"
        std::string_view invalid_values;
    };

    // What simulators know about a physical property
    struct PhysicalPropertyDescription
    {
        PHYSICAL_PROPERTY_NAME property;
        std::string_view name;
        // polygon for boundary conditions (Surfaces), polyhedron for the
        // other properties (Blocks)
        SIMULATION_ELEMENT_TYPE element_type;
        PROPERTY_VALUE_TYPES value_types;
        ValidRange valid_range;
        PROPERTY_READER reader;
    };

    [[nodiscard]] const PhysicalPropertyDescription&
        physical_property_description( PHYSICAL_PROPERTY_NAME property );

    // Every physical property, in the enum order
    [[nodiscard]] absl::Span< const PhysicalPropertyDescription >
        physical_property_descriptions();
} // namespace geode::internal
