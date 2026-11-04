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

#include <cmath>

#include <absl/strings/str_cat.h>

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/solid_mechanics.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

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

    PropertyReader property_reader( PHYSICAL_PROPERTY_NAME property )
    {
        switch( property )
        {
            case PHYSICAL_PROPERTY_NAME::porosity:
            case PHYSICAL_PROPERTY_NAME::permeability:
            case PHYSICAL_PROPERTY_NAME::boundary_pressure:
            case PHYSICAL_PROPERTY_NAME::boundary_temperature:
                return { "flow", []( const PhysicalProblem& problem ) {
                            return problem.has_flow();
                        } };
            case PHYSICAL_PROPERTY_NAME::boundary_oil_fraction:
            case PHYSICAL_PROPERTY_NAME::boundary_gas_fraction:
            case PHYSICAL_PROPERTY_NAME::boundary_water_fraction:
                return { "multiphase flow", is_multiphase };
            case PHYSICAL_PROPERTY_NAME::density:
                return { "solid mechanics without flow",
                    []( const PhysicalProblem& problem ) {
                        return problem.solid_mechanics().has_value()
                               && !problem.has_flow();
                    } };
            case PHYSICAL_PROPERTY_NAME::biot_coefficient:
            case PHYSICAL_PROPERTY_NAME::grain_density:
                return { "poromechanics", []( const PhysicalProblem& problem ) {
                            return problem.solid_mechanics().has_value()
                                   && problem.has_flow();
                        } };
            case PHYSICAL_PROPERTY_NAME::thermal_expansion_coefficient:
                return { "thermal poromechanics",
                    []( const PhysicalProblem& problem ) {
                        return problem.solid_mechanics().has_value()
                               && problem.has_flow()
                               && problem.thermal().has_value();
                    } };
            default:
                return { "solid mechanics",
                    []( const PhysicalProblem& problem ) {
                        return problem.solid_mechanics().has_value();
                    } };
        }
    }
} // namespace geode::internal
