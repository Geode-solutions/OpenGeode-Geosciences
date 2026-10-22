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

#include <geode/geosciences/simulation/core/physical_problem.hpp>

#include <array>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>
#include <utility>
#include <vector>

#include <bitsery/ext/std_optional.h>
#include <bitsery/ext/std_smart_ptr.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geosciences/simulation/builder/physical_problem_builder.hpp>
#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/saturation_functions.hpp>
#include <geode/geosciences/simulation/core/schedule.hpp>
#include <geode/geosciences/simulation/core/solver_options.hpp>
#include <geode/geosciences/simulation/core/source_term.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

namespace geode
{
    class PhysicalProblem::Impl
    {
        friend class bitsery::Access;

    public:
        [[nodiscard]] const std::optional< uuid >& model_id() const
        {
            return model_id_;
        }

        void set_model_id( const uuid& model_id )
        {
            model_id_ = model_id;
        }

        [[nodiscard]] bool has_flow() const
        {
            return static_cast< bool >( flow_ );
        }

        [[nodiscard]] const FlowModel& flow() const
        {
            OpenGeodeGeosciencesSimulationException::check_exception(
                has_flow(), nullptr, OpenGeodeException::TYPE::data,
                "[PhysicalProblem::flow] No flow model is defined" );
            return *flow_;
        }

        [[nodiscard]] const FluidModel& fluid() const
        {
            return flow().fluid();
        }

        void set_flow( std::shared_ptr< FlowModel > flow )
        {
            flow_ = std::move( flow );
        }

        [[nodiscard]] const std::optional< ThermalParameters >& thermal() const
        {
            return thermal_;
        }

        void set_thermal( const ThermalParameters& thermal )
        {
            thermal_ = thermal;
        }

        [[nodiscard]] absl::Span< const Material > materials() const
        {
            return materials_;
        }

        [[nodiscard]] index_t add_material( Material material )
        {
            materials_.emplace_back( std::move( material ) );
            return static_cast< index_t >( materials_.size() - 1 );
        }

        void set_material( index_t material_id, Material material )
        {
            check_material_id( material_id );
            materials_[material_id] = std::move( material );
        }

        void remove_material( index_t material_id )
        {
            check_material_id( material_id );
            materials_.erase( materials_.begin() + material_id );
        }

        [[nodiscard]] index_t nb_initial_conditions() const
        {
            return static_cast< index_t >( initial_conditions_.size() );
        }

        [[nodiscard]] const InitialCondition& initial_condition(
            index_t initial_condition_id ) const
        {
            check_initial_condition_id( initial_condition_id );
            return *initial_conditions_[initial_condition_id];
        }

        [[nodiscard]] index_t add_initial_condition(
            std::shared_ptr< InitialCondition > initial_condition )
        {
            initial_conditions_.emplace_back( std::move( initial_condition ) );
            return nb_initial_conditions() - 1;
        }

        void remove_initial_condition( index_t initial_condition_id )
        {
            check_initial_condition_id( initial_condition_id );
            initial_conditions_.erase(
                initial_conditions_.begin() + initial_condition_id );
        }

        [[nodiscard]] absl::Span< const SourceTerm > source_terms() const
        {
            return source_terms_;
        }

        [[nodiscard]] index_t add_source_term( SourceTerm source_term )
        {
            source_terms_.emplace_back( std::move( source_term ) );
            return static_cast< index_t >( source_terms_.size() - 1 );
        }

        void remove_source_term( index_t source_term_id )
        {
            OpenGeodeGeosciencesSimulationException::check_exception(
                source_term_id < source_terms_.size(), nullptr,
                OpenGeodeException::TYPE::data,
                "[PhysicalProblem] Source term index ", source_term_id,
                " is out of range" );
            source_terms_.erase( source_terms_.begin() + source_term_id );
        }

        [[nodiscard]] const Schedule& schedule() const
        {
            return schedule_;
        }

        void set_schedule( const Schedule& schedule )
        {
            schedule_ = schedule;
        }

        [[nodiscard]] const OutputSettings& output() const
        {
            return output_;
        }

        void set_output( OutputSettings output )
        {
            output_ = std::move( output );
        }

        [[nodiscard]] const Vector3D& gravity() const
        {
            return gravity_;
        }

        void set_gravity( const Vector3D& gravity )
        {
            gravity_ = gravity;
        }

        [[nodiscard]] const SolverOptions& solver_options() const
        {
            return solver_options_;
        }

        void set_solver_option( std::string_view simulator,
            std::string_view key,
            std::string_view value )
        {
            solver_options_.set( simulator, key, value );
        }

    private:
        void check_material_id( index_t material_id ) const
        {
            OpenGeodeGeosciencesSimulationException::check_exception(
                material_id < materials_.size(), nullptr,
                OpenGeodeException::TYPE::data,
                "[PhysicalProblem] Material index ", material_id,
                " is out of range" );
        }

        void check_initial_condition_id( index_t initial_condition_id ) const
        {
            OpenGeodeGeosciencesSimulationException::check_exception(
                initial_condition_id < initial_conditions_.size(), nullptr,
                OpenGeodeException::TYPE::data,
                "[PhysicalProblem] Initial condition index ",
                initial_condition_id, " is out of range" );
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{
                                    { []( Archive& local_archive, Impl& impl ) {
                                        impl.serialize_members( local_archive );
                                    } } } );
        }

        template < typename Archive >
        void serialize_members( Archive& local_archive )
        {
            local_archive.ext( model_id_, bitsery::ext::StdOptional{},
                []( Archive& id_archive, uuid& recorded_model_id ) {
                    id_archive.object( recorded_model_id );
                } );
            local_archive.ext( flow_, bitsery::ext::StdSmartPtr{} );
            local_archive.ext( thermal_, bitsery::ext::StdOptional{},
                []( Archive& thermal_archive,
                    ThermalParameters& thermal_parameters ) {
                    thermal_archive.object( thermal_parameters );
                } );
            local_archive.container( materials_, materials_.max_size(),
                []( Archive& material_archive, Material& material ) {
                    material_archive.object( material );
                } );
            local_archive.container( initial_conditions_,
                initial_conditions_.max_size(),
                []( Archive& condition_archive,
                    std::shared_ptr< InitialCondition >& condition ) {
                    condition_archive.ext(
                        condition, bitsery::ext::StdSmartPtr{} );
                } );
            local_archive.object( schedule_ );
            local_archive.object( output_ );
            std::array< double, 3 > gravity{ gravity_.value( 0 ),
                gravity_.value( 1 ), gravity_.value( 2 ) };
            local_archive.container8b( gravity );
            gravity_ = Vector3D{ gravity };
            local_archive.object( solver_options_ );
            local_archive.container( source_terms_, source_terms_.max_size(),
                []( Archive& source_archive, SourceTerm& source_term ) {
                    source_archive.object( source_term );
                } );
        }

    private:
        std::optional< uuid > model_id_;
        std::shared_ptr< FlowModel > flow_;
        std::optional< ThermalParameters > thermal_;
        std::vector< Material > materials_;
        std::vector< std::shared_ptr< InitialCondition > > initial_conditions_;
        std::vector< SourceTerm > source_terms_;
        Schedule schedule_;
        OutputSettings output_;
        Vector3D gravity_{ { 0., 0., -9.81 } };
        SolverOptions solver_options_;
    };

    PhysicalProblem::PhysicalProblem() = default;
    PhysicalProblem::PhysicalProblem( PhysicalProblem&& ) noexcept = default;
    PhysicalProblem& PhysicalProblem::operator=(
        PhysicalProblem&& ) noexcept = default;
    PhysicalProblem::~PhysicalProblem() = default;

    PhysicalProblem PhysicalProblem::clone() const
    {
        PhysicalProblem problem;
        PhysicalProblemBuilder{ problem }.copy( *this );
        return problem;
    }

    const std::optional< uuid >& PhysicalProblem::model_id() const
    {
        return impl_->model_id();
    }

    bool PhysicalProblem::has_flow() const
    {
        return impl_->has_flow();
    }

    const FlowModel& PhysicalProblem::flow() const
    {
        return impl_->flow();
    }

    const FluidModel& PhysicalProblem::fluid() const
    {
        return impl_->fluid();
    }

    const std::optional< ThermalParameters >& PhysicalProblem::thermal() const
    {
        return impl_->thermal();
    }

    absl::Span< const Material > PhysicalProblem::materials() const
    {
        return impl_->materials();
    }

    index_t PhysicalProblem::nb_initial_conditions() const
    {
        return impl_->nb_initial_conditions();
    }

    const InitialCondition& PhysicalProblem::initial_condition(
        index_t initial_condition_id ) const
    {
        return impl_->initial_condition( initial_condition_id );
    }

    absl::Span< const SourceTerm > PhysicalProblem::source_terms() const
    {
        return impl_->source_terms();
    }

    const Schedule& PhysicalProblem::schedule() const
    {
        return impl_->schedule();
    }

    const OutputSettings& PhysicalProblem::output() const
    {
        return impl_->output();
    }

    const Vector3D& PhysicalProblem::gravity() const
    {
        return impl_->gravity();
    }

    const SolverOptions& PhysicalProblem::solver_options() const
    {
        return impl_->solver_options();
    }

    void PhysicalProblem::set_model_id(
        const uuid& model_id, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_model_id( model_id );
    }

    void PhysicalProblem::set_flow( std::shared_ptr< FlowModel > flow,
        PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_flow( std::move( flow ) );
    }

    void PhysicalProblem::set_thermal(
        const ThermalParameters& thermal, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_thermal( thermal );
    }

    index_t PhysicalProblem::add_material(
        Material material, PhysicalProblemBuilderKey /*unused*/ )
    {
        return impl_->add_material( std::move( material ) );
    }

    void PhysicalProblem::set_material( index_t material_id,
        Material material,
        PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_material( material_id, std::move( material ) );
    }

    void PhysicalProblem::remove_material(
        index_t material_id, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->remove_material( material_id );
    }

    index_t PhysicalProblem::add_initial_condition(
        std::shared_ptr< InitialCondition > initial_condition,
        PhysicalProblemBuilderKey /*unused*/ )
    {
        return impl_->add_initial_condition( std::move( initial_condition ) );
    }

    void PhysicalProblem::remove_initial_condition(
        index_t initial_condition_id, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->remove_initial_condition( initial_condition_id );
    }

    index_t PhysicalProblem::add_source_term(
        SourceTerm source_term, PhysicalProblemBuilderKey /*unused*/ )
    {
        return impl_->add_source_term( std::move( source_term ) );
    }

    void PhysicalProblem::remove_source_term(
        index_t source_term_id, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->remove_source_term( source_term_id );
    }

    void PhysicalProblem::set_schedule(
        const Schedule& schedule, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_schedule( schedule );
    }

    void PhysicalProblem::set_output(
        OutputSettings output, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_output( std::move( output ) );
    }

    void PhysicalProblem::set_gravity(
        const Vector3D& gravity, PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_gravity( gravity );
    }

    void PhysicalProblem::set_solver_option( std::string_view simulator,
        std::string_view key,
        std::string_view value,
        PhysicalProblemBuilderKey /*unused*/ )
    {
        impl_->set_solver_option( simulator, key, value );
    }

    void PhysicalProblem::copy(
        const PhysicalProblem& other, PhysicalProblemBuilderKey /*unused*/ )
    {
        *impl_ = *other.impl_;
    }

    template < typename Archive >
    void PhysicalProblem::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, PhysicalProblem >{
                { []( Archive& local_archive, PhysicalProblem& problem ) {
                    local_archive.ext(
                        problem, bitsery::ext::BaseClass< Identifier >{} );
                    local_archive.object( problem.impl_ );
                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, PhysicalProblem );
} // namespace geode
