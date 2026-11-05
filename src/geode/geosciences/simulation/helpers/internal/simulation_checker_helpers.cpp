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

#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>

#include <array>
#include <cmath>
#include <cstddef>

#include <absl/strings/str_cat.h>

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/solid_mechanics.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

namespace
{
    using geode::PHYSICAL_PROPERTY_NAME;
    using geode::internal::PhysicalPropertyDescription;
    using geode::internal::PROPERTY_READER;
    using geode::internal::PROPERTY_VALUE_TYPES;
    using geode::internal::ValidRange;

    constexpr auto ON_BLOCKS = geode::SIMULATION_ELEMENT_TYPE::polyhedron;
    constexpr auto ON_SURFACES = geode::SIMULATION_ELEMENT_TYPE::polygon;

    constexpr ValidRange POSITIVE_FRACTION{ []( double value ) {
                                               return value > 0 && value <= 1;
                                           },
        "values outside (0, 1]" };
    constexpr ValidRange FRACTION{ []( double value ) {
                                      return value >= 0 && value <= 1;
                                  },
        "values outside [0, 1]" };
    constexpr ValidRange POSITIVE{ geode::internal::is_positive,
        "values outside (0, +inf)" };
    constexpr ValidRange NON_NEGATIVE{ geode::internal::is_non_negative,
        "values outside [0, +inf)" };
    constexpr ValidRange FINITE{ []( double value ) {
                                    return std::isfinite( value );
                                },
        "non-finite values" };
    // NaN components are free: only infinite values are invalid
    constexpr ValidRange FINITE_OR_FREE{ []( double value ) {
                                            return !std::isinf( value );
                                        },
        "infinite values" };

    // In the PHYSICAL_PROPERTY_NAME order
    constexpr std::array PROPERTY_DESCRIPTIONS{
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::porosity,
            "porosity", ON_BLOCKS, PROPERTY_VALUE_TYPES::scalar,
            POSITIVE_FRACTION, PROPERTY_READER::flow },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::permeability,
            "permeability", ON_BLOCKS, PROPERTY_VALUE_TYPES::scalar_or_vector3,
            NON_NEGATIVE, PROPERTY_READER::flow },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::bulk_modulus,
            "bulk_modulus", ON_BLOCKS, PROPERTY_VALUE_TYPES::scalar, POSITIVE,
            PROPERTY_READER::solid_mechanics },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::shear_modulus,
            "shear_modulus", ON_BLOCKS, PROPERTY_VALUE_TYPES::scalar, POSITIVE,
            PROPERTY_READER::solid_mechanics },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::density, "density",
            ON_BLOCKS, PROPERTY_VALUE_TYPES::scalar, NON_NEGATIVE,
            PROPERTY_READER::solid_mechanics_without_flow },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::biot_coefficient,
            "biot_coefficient", ON_BLOCKS, PROPERTY_VALUE_TYPES::scalar,
            POSITIVE_FRACTION, PROPERTY_READER::poromechanics },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::grain_density,
            "grain_density", ON_BLOCKS, PROPERTY_VALUE_TYPES::scalar,
            NON_NEGATIVE, PROPERTY_READER::poromechanics },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::thermal_expansion_coefficient,
            "thermal_expansion_coefficient", ON_BLOCKS,
            PROPERTY_VALUE_TYPES::scalar, NON_NEGATIVE,
            PROPERTY_READER::thermal_poromechanics },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::initial_stress,
            "initial_stress", ON_BLOCKS, PROPERTY_VALUE_TYPES::vector3, FINITE,
            PROPERTY_READER::solid_mechanics },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::initial_shear_stress,
            "initial_shear_stress", ON_BLOCKS, PROPERTY_VALUE_TYPES::vector3,
            FINITE, PROPERTY_READER::solid_mechanics },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::boundary_pressure,
            "boundary_pressure", ON_SURFACES, PROPERTY_VALUE_TYPES::scalar,
            FINITE, PROPERTY_READER::flow },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::boundary_temperature,
            "boundary_temperature", ON_SURFACES, PROPERTY_VALUE_TYPES::scalar,
            POSITIVE, PROPERTY_READER::flow },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::boundary_oil_fraction,
            "boundary_oil_fraction", ON_SURFACES, PROPERTY_VALUE_TYPES::scalar,
            FRACTION, PROPERTY_READER::multiphase_flow },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::boundary_gas_fraction,
            "boundary_gas_fraction", ON_SURFACES, PROPERTY_VALUE_TYPES::scalar,
            FRACTION, PROPERTY_READER::multiphase_flow },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::boundary_water_fraction,
            "boundary_water_fraction", ON_SURFACES,
            PROPERTY_VALUE_TYPES::scalar, FRACTION,
            PROPERTY_READER::multiphase_flow },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::boundary_displacement,
            "boundary_displacement", ON_SURFACES, PROPERTY_VALUE_TYPES::vector3,
            FINITE_OR_FREE, PROPERTY_READER::solid_mechanics },
        PhysicalPropertyDescription{ PHYSICAL_PROPERTY_NAME::boundary_traction,
            "boundary_traction", ON_SURFACES, PROPERTY_VALUE_TYPES::vector3,
            FINITE, PROPERTY_READER::solid_mechanics },
        PhysicalPropertyDescription{
            PHYSICAL_PROPERTY_NAME::boundary_normal_traction,
            "boundary_normal_traction", ON_SURFACES,
            PROPERTY_VALUE_TYPES::scalar, FINITE,
            PROPERTY_READER::solid_mechanics },
    };

    constexpr bool are_in_enum_order()
    {
        std::size_t position{ 0 };
        // Range-for rather than an algorithm: std::array iterators are not
        // pointers on MSVC
        for( const auto& description : PROPERTY_DESCRIPTIONS )
        {
            if( static_cast< std::size_t >( description.property )
                != position++ )
            {
                return false;
            }
        }
        return true;
    }

    static_assert( are_in_enum_order(),
        "PROPERTY_DESCRIPTIONS must follow the PHYSICAL_PROPERTY_NAME order" );
    static_assert( PROPERTY_DESCRIPTIONS.size()
                       == static_cast< std::size_t >(
                              PHYSICAL_PROPERTY_NAME::boundary_normal_traction )
                              + 1,
        "Every PHYSICAL_PROPERTY_NAME needs a description" );
} // namespace

namespace geode::internal
{
    geode::SimulationIssueLocation component_location(
        const geode::Component3D& component )
    {
        geode::SimulationIssueLocation location;
        location.component = component.component_id();
        if( component.name() )
        {
            location.name = component.name().value();
        }
        return location;
    }

    std::string count_string( std::size_t count, std::string_view noun )
    {
        return absl::StrCat( count, " ", noun, count > 1 ? "s" : "" );
    }

    bool is_positive( double value )
    {
        return std::isfinite( value ) && value > 0;
    }

    bool is_non_negative( double value )
    {
        return std::isfinite( value ) && value >= 0;
    }

    bool is_multiphase( const geode::PhysicalProblem& problem )
    {
        return problem.has_flow()
               && dynamic_cast< const geode::MultiphaseFlow* >(
                      &problem.flow() )
                      != nullptr;
    }

    std::string material_subject( std::string_view name )
    {
        return absl::StrCat( "Material \"", name, "\"" );
    }

    std::string initial_condition_subject( geode::index_t condition )
    {
        return absl::StrCat( "Initial condition #", condition );
    }

    std::string_view property_reader_string( PROPERTY_READER reader )
    {
        switch( reader )
        {
            case PROPERTY_READER::flow:
                return "flow";
            case PROPERTY_READER::multiphase_flow:
                return "multiphase flow";
            case PROPERTY_READER::solid_mechanics:
                return "solid mechanics";
            case PROPERTY_READER::solid_mechanics_without_flow:
                return "solid mechanics without flow";
            case PROPERTY_READER::poromechanics:
                return "poromechanics";
            case PROPERTY_READER::thermal_poromechanics:
                return "thermal poromechanics";
        }
        return "unknown";
    }

    bool reads( PROPERTY_READER reader, const PhysicalProblem& problem )
    {
        const auto has_mechanics = problem.solid_mechanics().has_value();
        switch( reader )
        {
            case PROPERTY_READER::flow:
                return problem.has_flow();
            case PROPERTY_READER::multiphase_flow:
                return is_multiphase( problem );
            case PROPERTY_READER::solid_mechanics:
                return has_mechanics;
            case PROPERTY_READER::solid_mechanics_without_flow:
                return has_mechanics && !problem.has_flow();
            case PROPERTY_READER::poromechanics:
                return has_mechanics && problem.has_flow();
            case PROPERTY_READER::thermal_poromechanics:
                return has_mechanics && problem.has_flow()
                       && problem.thermal().has_value();
        }
        return false;
    }

    const PhysicalPropertyDescription& physical_property_description(
        PHYSICAL_PROPERTY_NAME property )
    {
        const auto position = static_cast< std::size_t >( property );
        OpenGeodeGeosciencesSimulationException::check_exception(
            position < PROPERTY_DESCRIPTIONS.size(), nullptr,
            OpenGeodeException::TYPE::internal,
            "[physical_property_description] Physical property ", position,
            " has no description" );
        return PROPERTY_DESCRIPTIONS[position];
    }

    absl::Span< const PhysicalPropertyDescription >
        physical_property_descriptions()
    {
        return PROPERTY_DESCRIPTIONS;
    }
} // namespace geode::internal
