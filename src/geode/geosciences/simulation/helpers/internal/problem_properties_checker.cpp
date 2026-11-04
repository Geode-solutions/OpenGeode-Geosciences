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

#include <geode/geosciences/simulation/helpers/internal/problem_properties_checker.hpp>

#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <string_view>
#include <typeinfo>
#include <utility>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/solid_mechanics.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>
#include <geode/geosciences/simulation/helpers/internal/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/internal/property_attribute_checker.hpp>
#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>
#include <geode/geosciences/simulation/helpers/simulation_checker.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace
{
    using geode::PHYSICAL_PROPERTY_NAME;
    using geode::SIMULATION_CHECK_STAGE;
    using geode::SIMULATION_ISSUE_SEVERITY;
    using geode::internal::component_location;
    using geode::internal::count_string;

    constexpr std::array< PHYSICAL_PROPERTY_NAME, 5 > FLOW_BOUNDARY_PROPERTIES{
        PHYSICAL_PROPERTY_NAME::boundary_pressure,
        PHYSICAL_PROPERTY_NAME::boundary_temperature,
        PHYSICAL_PROPERTY_NAME::boundary_oil_fraction,
        PHYSICAL_PROPERTY_NAME::boundary_gas_fraction,
        PHYSICAL_PROPERTY_NAME::boundary_water_fraction
    };

    constexpr std::array< PHYSICAL_PROPERTY_NAME, 3 >
        MECHANICS_BOUNDARY_PROPERTIES{
            PHYSICAL_PROPERTY_NAME::boundary_displacement,
            PHYSICAL_PROPERTY_NAME::boundary_traction,
            PHYSICAL_PROPERTY_NAME::boundary_normal_traction
        };

    constexpr std::array< PHYSICAL_PROPERTY_NAME, 18 > ALL_PROPERTIES{
        PHYSICAL_PROPERTY_NAME::porosity, PHYSICAL_PROPERTY_NAME::permeability,
        PHYSICAL_PROPERTY_NAME::bulk_modulus,
        PHYSICAL_PROPERTY_NAME::shear_modulus, PHYSICAL_PROPERTY_NAME::density,
        PHYSICAL_PROPERTY_NAME::biot_coefficient,
        PHYSICAL_PROPERTY_NAME::grain_density,
        PHYSICAL_PROPERTY_NAME::thermal_expansion_coefficient,
        PHYSICAL_PROPERTY_NAME::initial_stress,
        PHYSICAL_PROPERTY_NAME::initial_shear_stress,
        PHYSICAL_PROPERTY_NAME::boundary_pressure,
        PHYSICAL_PROPERTY_NAME::boundary_temperature,
        PHYSICAL_PROPERTY_NAME::boundary_oil_fraction,
        PHYSICAL_PROPERTY_NAME::boundary_gas_fraction,
        PHYSICAL_PROPERTY_NAME::boundary_water_fraction,
        PHYSICAL_PROPERTY_NAME::boundary_displacement,
        PHYSICAL_PROPERTY_NAME::boundary_traction,
        PHYSICAL_PROPERTY_NAME::boundary_normal_traction
    };

    constexpr std::array< std::string_view, 3 > AXIS_NAMES{ "x", "y", "z" };

    // Free displacement components are NaN, which equals no value
    bool same_components( const std::array< double, 3 >& first,
        const std::array< double, 3 >& second )
    {
        for( const auto axis : geode::LRange{ 3 } )
        {
            if( first[axis] != second[axis]
                && !(
                    std::isnan( first[axis] ) && std::isnan( second[axis] ) ) )
            {
                return false;
            }
        }
        return true;
    }

    std::vector< geode::index_t > polygons_differing_from_first(
        const geode::ReadOnlyAttribute< std::array< double, 3 > >& values,
        geode::index_t nb_polygons )
    {
        std::vector< geode::index_t > different;
        const auto& first = values.value( 0 );
        for( const auto polygon : geode::Range{ nb_polygons } )
        {
            if( !same_components( values.value( polygon ), first ) )
            {
                different.push_back( polygon );
            }
        }
        return different;
    }

    geode::SimulationIssueLocation non_uniform_location(
        const geode::Surface3D& surface,
        std::vector< geode::index_t > different_polygons )
    {
        auto location = component_location( surface );
        location.element_type = geode::SIMULATION_ELEMENT_TYPE::polygon;
        location.elements = std::move( different_polygons );
        location.nb_elements = surface.mesh().nb_polygons();
        location.detail = "polygons differing from the first one";
        return location;
    }

    void add_displaced_axes( const std::array< double, 3 >& displacement,
        std::array< bool, 3 >& displaced_axes )
    {
        for( const auto axis : geode::LRange{ 3 } )
        {
            displaced_axes[axis] =
                displaced_axes[axis] || !std::isnan( displacement[axis] );
        }
    }

    // Model physical properties read by solid mechanics, and properties of
    // the other processes
    class PhysicalPropertyChecker
    {
    public:
        PhysicalPropertyChecker( const geode::PhysicalProblem& problem,
            const geode::BRep& model,
            geode::SimulationReport& report )
            : problem_( problem ), model_( model ), report_( report )
        {
        }

        void run()
        {
            check_unread_properties();
            if( !problem_.solid_mechanics() )
            {
                return;
            }
            check_elastic_properties();
            check_density();
            check_initial_stresses();
            check_displacements();
            check_tractions();
            if( problem_.has_flow() )
            {
                check_poromechanics_properties();
            }
        }

    private:
        [[nodiscard]] std::string_view process() const
        {
            return problem_.has_flow() ? "poromechanics" : "solid mechanics";
        }

        geode::PhysicalPropertyCoverage check_property(
            PHYSICAL_PROPERTY_NAME property, bool is_required )
        {
            geode::PhysicalPropertyRequirement requirement{ property };
            if( is_required )
            {
                requirement.required_by = std::string{ process() };
            }
            return geode::internal::check_physical_property(
                model_, requirement, report_ );
        }

        // Boundary conditions of a missing process are errors: the problem
        // probably lacks this process. Other properties are ignored.
        void check_unread_properties()
        {
            for( const auto property : ALL_PROPERTIES )
            {
                if( !model_.has_physical_property( property )
                    || geode::is_physical_property_read( problem_, property ) )
                {
                    continue;
                }
                report_unread_property( property );
            }
        }

        void add_unread_property_error(
            std::string_view name, std::string message, std::string fix )
        {
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::properties, std::string{ name },
                std::move( message ) };
            issue.fix = std::move( fix );
            report_.add_issue( std::move( issue ) );
        }

        void report_unread_property( PHYSICAL_PROPERTY_NAME property )
        {
            const auto name = geode::physical_property_string( property );
            if( !problem_.has_flow()
                && absl::c_linear_search( FLOW_BOUNDARY_PROPERTIES, property ) )
            {
                add_unread_property_error( name,
                    "flow boundary condition without flow model",
                    "define a flow model or remove the property" );
            }
            else if( !problem_.solid_mechanics()
                     && absl::c_linear_search(
                         MECHANICS_BOUNDARY_PROPERTIES, property ) )
            {
                add_unread_property_error( name,
                    "mechanical boundary condition without solid mechanics",
                    "define solid mechanics or remove the property" );
            }
            else if( property == PHYSICAL_PROPERTY_NAME::density
                     && problem_.solid_mechanics() )
            {
                add_unread_property_error( name,
                    "bulk density of solid mechanics without flow, "
                    "poromechanics reads the grain density",
                    "give the grain_density instead" );
            }
            else
            {
                report_.add_warning( SIMULATION_CHECK_STAGE::properties,
                    std::string{ name }, "ignored, only read by ",
                    geode::internal::property_reader( property ).name );
            }
        }

        void check_elastic_properties()
        {
            check_property( PHYSICAL_PROPERTY_NAME::bulk_modulus, true );
            check_property( PHYSICAL_PROPERTY_NAME::shear_modulus, true );
        }

        // With flow, the weight of the rock comes from its grains and fluid
        void check_density()
        {
            check_property( problem_.has_flow()
                                ? PHYSICAL_PROPERTY_NAME::grain_density
                                : PHYSICAL_PROPERTY_NAME::density,
                problem_.gravity().length() > 0 );
        }

        void check_poromechanics_properties()
        {
            check_property( PHYSICAL_PROPERTY_NAME::biot_coefficient, true );
            check_biot_coefficients_above_porosity();
            if( problem_.thermal() )
            {
                check_property(
                    PHYSICAL_PROPERTY_NAME::thermal_expansion_coefficient,
                    true );
            }
        }

        // Double attribute of a Block property, null if the Block has none
        [[nodiscard]] std::shared_ptr< geode::ReadOnlyAttribute< double > >
            block_double_attribute( const geode::Block3D& block,
                PHYSICAL_PROPERTY_NAME property ) const
        {
            if( !model_.has_physical_property( property ) )
            {
                return nullptr;
            }
            const auto& info = model_.physical_property_info( property );
            const auto& manager = block.mesh().polyhedron_attribute_manager();
            if( info.component_type != geode::Block3D::component_type_static()
                || !manager.attribute_exists( info.attribute_id )
                || manager.attribute_type( info.attribute_id )
                       != typeid( double ).name() )
            {
                return nullptr;
            }
            return manager.find_read_only_attribute< double >(
                info.attribute_id );
        }

        // Polyhedra whose Biot coefficient is below the porosity property, or
        // else below the material porosity
        [[nodiscard]] std::vector< geode::index_t >
            polyhedra_with_biot_below_porosity( const geode::Block3D& block,
                std::optional< double > material_porosity ) const
        {
            std::vector< geode::index_t > invalid;
            const auto biot = block_double_attribute(
                block, PHYSICAL_PROPERTY_NAME::biot_coefficient );
            if( !biot )
            {
                return invalid;
            }
            const auto porosity = block_double_attribute(
                block, PHYSICAL_PROPERTY_NAME::porosity );
            for( const auto polyhedron :
                geode::Range{ block.mesh().nb_polyhedra() } )
            {
                auto cell_porosity = material_porosity;
                if( porosity && porosity->has_value( polyhedron ) )
                {
                    cell_porosity = porosity->value( polyhedron );
                }
                if( biot->has_value( polyhedron ) && cell_porosity
                    && biot->value( polyhedron ) < cell_porosity.value() )
                {
                    invalid.push_back( polyhedron );
                }
            }
            return invalid;
        }

        // The pore compressibility (b - porosity) / Ks must not be negative.
        // The porosity is the porosity property, or else the one of the
        // Block Material.
        void check_biot_coefficients_above_porosity()
        {
            const auto materials = problem_.materials();
            const auto materials_of_blocks =
                geode::internal::physical_problem_block_materials(
                    problem_, model_ );
            std::vector< geode::SimulationIssueLocation > locations;
            for( const auto& block : model_.blocks() )
            {
                std::optional< double > material_porosity;
                if( const auto material =
                        materials_of_blocks.find( block.id() );
                    material != materials_of_blocks.end() )
                {
                    material_porosity = materials[material->second].porosity;
                }
                auto invalid = polyhedra_with_biot_below_porosity(
                    block, material_porosity );
                if( !invalid.empty() )
                {
                    auto& location =
                        locations.emplace_back( component_location( block ) );
                    location.element_type =
                        geode::SIMULATION_ELEMENT_TYPE::polyhedron;
                    location.elements = std::move( invalid );
                    location.nb_elements = block.mesh().nb_polyhedra();
                }
            }
            if( locations.empty() )
            {
                return;
            }
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::properties, "biot_coefficient",
                absl::StrCat( "smaller than the porosity on ",
                    count_string( locations.size(), "Block" ),
                    ": the pore compressibility (biot - porosity) / Ks would "
                    "be negative" ) };
            issue.locations = std::move( locations );
            issue.fix = "the Biot coefficient must be at least the porosity";
            report_.add_issue( std::move( issue ) );
        }

        void check_initial_stresses()
        {
            check_property( PHYSICAL_PROPERTY_NAME::initial_stress, false );
            check_property(
                PHYSICAL_PROPERTY_NAME::initial_shear_stress, false );
        }

        // Displacements are uniform on each Surface, and fixed along each
        // axis on at least one Surface
        void check_displacements()
        {
            const auto coverage = check_property(
                PHYSICAL_PROPERTY_NAME::boundary_displacement, false );
            std::array< bool, 3 > displaced_axes{ false, false, false };
            if( coverage.is_registered )
            {
                check_uniform_displacements( coverage, displaced_axes );
            }
            report_free_axes( displaced_axes );
        }

        // Updates the axes displaced by uniform Surface displacements
        void check_uniform_displacements(
            const geode::PhysicalPropertyCoverage& coverage,
            std::array< bool, 3 >& displaced_axes )
        {
            const auto& attribute_id =
                model_
                    .physical_property_info(
                        PHYSICAL_PROPERTY_NAME::boundary_displacement )
                    .attribute_id;
            std::vector< geode::SimulationIssueLocation > not_uniform;
            for( const auto& surface : model_.surfaces() )
            {
                if( !coverage.covers( surface.id() ) )
                {
                    continue;
                }
                const auto& mesh = surface.mesh();
                const auto values =
                    mesh.polygon_attribute_manager()
                        .find_read_only_attribute< std::array< double, 3 > >(
                            attribute_id );
                auto different = polygons_differing_from_first(
                    *values, mesh.nb_polygons() );
                if( !different.empty() )
                {
                    not_uniform.push_back( non_uniform_location(
                        surface, std::move( different ) ) );
                    continue;
                }
                add_displaced_axes( values->value( 0 ), displaced_axes );
            }
            if( not_uniform.empty() )
            {
                return;
            }
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::properties, "boundary_displacement",
                absl::StrCat(
                    "not uniform on ", count_string( not_uniform.size(), "Surface" ) ) };
            issue.locations = std::move( not_uniform );
            issue.fix = "impose one displacement per Surface";
            report_.add_issue( std::move( issue ) );
        }

        void report_free_axes( const std::array< bool, 3 >& displaced_axes )
        {
            std::vector< std::string_view > free_axes;
            for( const auto axis : geode::LRange{ 3 } )
            {
                if( !displaced_axes[axis] )
                {
                    free_axes.push_back( AXIS_NAMES[axis] );
                }
            }
            if( free_axes.empty() )
            {
                return;
            }
            const auto axes = absl::StrJoin( free_axes, ", " );
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::properties, "boundary_displacement",
                absl::StrCat(
                    "no Surface has an imposed displacement along ", axes ) };
            issue.fix = absl::StrCat( "fix the displacement along ", axes,
                " on at least one Surface to prevent rigid body motions" );
            report_.add_issue( std::move( issue ) );
        }

        void check_tractions()
        {
            check_property( PHYSICAL_PROPERTY_NAME::boundary_traction, false );
            check_property(
                PHYSICAL_PROPERTY_NAME::boundary_normal_traction, false );
        }

    private:
        const geode::PhysicalProblem& problem_;
        const geode::BRep& model_;
        geode::SimulationReport& report_;
    };

    struct MeanStressSigns
    {
        geode::index_t nb_positives{ 0 };
        geode::index_t nb_negatives{ 0 };
    };

    // Sign of the mean normal stress of each cell, uniform values included
    MeanStressSigns mean_stress_signs(
        const geode::BRep& model, const geode::uuid& attribute_id )
    {
        MeanStressSigns signs;
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            const auto& manager = mesh.polyhedron_attribute_manager();
            if( !manager.attribute_exists( attribute_id )
                || manager.attribute_type( attribute_id )
                       != typeid( std::array< double, 3 > ).name() )
            {
                continue;
            }
            const auto stress =
                manager.find_read_only_attribute< std::array< double, 3 > >(
                    attribute_id );
            for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
            {
                const auto& value = stress->value( polyhedron );
                const auto trace = value[0] + value[1] + value[2];
                if( trace > 0 )
                {
                    signs.nb_positives++;
                }
                else if( trace < 0 )
                {
                    signs.nb_negatives++;
                }
            }
        }
        return signs;
    }

    void warn_stress_sign_convention( const geode::PhysicalProblem& problem,
        const geode::BRep& model,
        geode::SimulationReport& report )
    {
        if( !model.has_physical_property(
                PHYSICAL_PROPERTY_NAME::initial_stress ) )
        {
            return;
        }
        const auto signs = mean_stress_signs(
            model, model
                       .physical_property_info(
                           PHYSICAL_PROPERTY_NAME::initial_stress )
                       .attribute_id );
        const auto compression_positive =
            problem.solid_mechanics()->sign_convention
            == geode::STRESS_SIGN_CONVENTION::compression_positive;
        const auto nb_compressions =
            compression_positive ? signs.nb_positives : signs.nb_negatives;
        const auto nb_tensions =
            compression_positive ? signs.nb_negatives : signs.nb_positives;
        if( nb_tensions > nb_compressions )
        {
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::warning,
                SIMULATION_CHECK_STAGE::properties, "initial_stress",
                absl::StrCat( "read as ",
                    compression_positive ? "compression positive"
                                         : "tension positive",
                    " values, the mean stress is a tension in ", nb_tensions,
                    " of ", nb_tensions + nb_compressions, " cells" ) };
            issue.fix = "check the problem stress sign convention: the model "
                        "may use the opposite one";
            report.add_issue( std::move( issue ) );
        }
    }
} // namespace

namespace geode::internal
{
    void check_physical_problem_properties( const PhysicalProblem& problem,
        const BRep& model,
        SimulationReport& report )
    {
        PhysicalPropertyChecker{ problem, model, report }.run();
        if( problem.solid_mechanics() )
        {
            warn_stress_sign_convention( problem, model, report );
        }
    }
} // namespace geode::internal
