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

#include <geode/geosciences/simulation/builder/physical_problem_builder.hpp>

#include <utility>

#include <geode/basic/uuid.hpp>

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/solid_mechanics.hpp>
#include <geode/geosciences/simulation/core/source_term.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

namespace
{
    using BuilderKey = geode::PhysicalProblem::PhysicalProblemBuilderKey;
} // namespace

namespace geode
{
    PhysicalProblemBuilder::PhysicalProblemBuilder( PhysicalProblem& problem )
        : IdentifierBuilder( problem ), problem_( problem )
    {
    }

    void PhysicalProblemBuilder::copy( const PhysicalProblem& other )
    {
        copy_identifier( other );
        problem_.copy( other, BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_model_id( const uuid& model_id )
    {
        problem_.set_model_id( model_id, BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_flow( std::shared_ptr< FlowModel > flow )
    {
        OpenGeodeGeosciencesSimulationException::check_exception(
            static_cast< bool >( flow ), nullptr,
            OpenGeodeException::TYPE::data,
            "[PhysicalProblemBuilder::set_flow] Flow model is null" );
        problem_.set_flow( std::move( flow ), BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_thermal( const ThermalParameters& thermal )
    {
        problem_.set_thermal( thermal, BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_solid_mechanics(
        const SolidMechanics& mechanics )
    {
        problem_.set_solid_mechanics( mechanics, BuilderKey{} );
    }

    void PhysicalProblemBuilder::remove_solid_mechanics()
    {
        problem_.remove_solid_mechanics( BuilderKey{} );
    }

    index_t PhysicalProblemBuilder::add_material( Material material )
    {
        return problem_.add_material( std::move( material ), BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_material(
        index_t material_id, Material material )
    {
        problem_.set_material(
            material_id, std::move( material ), BuilderKey{} );
    }

    void PhysicalProblemBuilder::remove_material( index_t material_id )
    {
        problem_.remove_material( material_id, BuilderKey{} );
    }

    index_t PhysicalProblemBuilder::add_initial_condition(
        std::shared_ptr< InitialCondition > initial_condition )
    {
        OpenGeodeGeosciencesSimulationException::check_exception(
            static_cast< bool >( initial_condition ), nullptr,
            OpenGeodeException::TYPE::data,
            "[PhysicalProblemBuilder::add_initial_condition] Initial "
            "condition is null" );
        return problem_.add_initial_condition(
            std::move( initial_condition ), BuilderKey{} );
    }

    void PhysicalProblemBuilder::remove_initial_condition(
        index_t initial_condition_id )
    {
        problem_.remove_initial_condition( initial_condition_id, BuilderKey{} );
    }

    index_t PhysicalProblemBuilder::add_source_term( SourceTerm source_term )
    {
        return problem_.add_source_term(
            std::move( source_term ), BuilderKey{} );
    }

    void PhysicalProblemBuilder::remove_source_term( index_t source_term_id )
    {
        problem_.remove_source_term( source_term_id, BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_schedule( const Schedule& schedule )
    {
        problem_.set_schedule( schedule, BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_output( OutputSettings output )
    {
        problem_.set_output( std::move( output ), BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_gravity( const Vector3D& gravity )
    {
        problem_.set_gravity( gravity, BuilderKey{} );
    }

    void PhysicalProblemBuilder::set_solver_option( std::string_view simulator,
        std::string_view key,
        std::string_view value )
    {
        problem_.set_solver_option( simulator, key, value, BuilderKey{} );
    }
} // namespace geode
