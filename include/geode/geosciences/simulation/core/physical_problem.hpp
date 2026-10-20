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

#include <memory>
#include <optional>
#include <string_view>

#include <absl/types/span.h>

#include <geode/basic/identifier.hpp>
#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/geometry/vector.hpp>

#include <geode/geosciences/simulation/common.hpp>
#include <geode/geosciences/simulation/core/thermal_parameters.hpp>

namespace geode
{
    class PhysicalProblemBuilder;
    class FlowModel;
    class FluidModel;
    class InitialCondition;
    class SolverOptions;
    struct Material;
    struct OutputSettings;
    struct Schedule;
    struct SourceTerm;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Simulator-independent description of a flow simulation on a model:
     * fluid, flow process, rock materials, initial state, well source terms,
     * time schedule, outputs and simulator-specific options.
     * Model regions are referenced by component uuid. Spatially varying data
     * (porosity, permeability, boundary conditions) stay in the model
     * PhysicalProperties.
     */
    class opengeode_geosciences_simulation_api PhysicalProblem
        : public Identifier
    {
        OPENGEODE_DISABLE_COPY( PhysicalProblem );
        PASSKEY( PhysicalProblemBuilder, PhysicalProblemBuilderKey );
        friend class bitsery::Access;

    public:
        using Builder = PhysicalProblemBuilder;

        PhysicalProblem();
        PhysicalProblem( PhysicalProblem&& other ) noexcept;
        PhysicalProblem& operator=( PhysicalProblem&& other ) noexcept;
        ~PhysicalProblem();

        /// Polymorphic parts (flow, fluid, laws, initial conditions) are
        /// shared with the clone
        [[nodiscard]] PhysicalProblem clone() const;

        [[nodiscard]] static std::string_view native_extension_static()
        {
            static const auto extension = "og_phpb";
            return extension;
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }

        /// Id of the model this problem was built for, if recorded
        [[nodiscard]] const std::optional< uuid >& model_id() const;

        [[nodiscard]] bool has_flow() const;

        [[nodiscard]] const FlowModel& flow() const;

        [[nodiscard]] const FluidModel& fluid() const;

        [[nodiscard]] const std::optional< ThermalParameters >& thermal() const;

        [[nodiscard]] absl::Span< const Material > materials() const;

        [[nodiscard]] index_t nb_initial_conditions() const;

        [[nodiscard]] const InitialCondition& initial_condition(
            index_t initial_condition_id ) const;

        [[nodiscard]] absl::Span< const SourceTerm > source_terms() const;

        [[nodiscard]] const Schedule& schedule() const;

        [[nodiscard]] const OutputSettings& output() const;

        /// m/s2, default is (0, 0, -9.81)
        [[nodiscard]] const Vector3D& gravity() const;

        [[nodiscard]] const SolverOptions& solver_options() const;

    public:
        void set_model_id( const uuid& model_id, PhysicalProblemBuilderKey );

        void set_flow(
            std::shared_ptr< FlowModel > flow, PhysicalProblemBuilderKey );

        void set_thermal(
            const ThermalParameters& thermal, PhysicalProblemBuilderKey );

        [[nodiscard]] index_t add_material(
            Material material, PhysicalProblemBuilderKey );

        void set_material(
            index_t material_id, Material material, PhysicalProblemBuilderKey );

        void remove_material( index_t material_id, PhysicalProblemBuilderKey );

        [[nodiscard]] index_t add_initial_condition(
            std::shared_ptr< InitialCondition > initial_condition,
            PhysicalProblemBuilderKey );

        void remove_initial_condition(
            index_t initial_condition_id, PhysicalProblemBuilderKey );

        [[nodiscard]] index_t add_source_term(
            SourceTerm source_term, PhysicalProblemBuilderKey );

        void remove_source_term(
            index_t source_term_id, PhysicalProblemBuilderKey );

        void set_schedule(
            const Schedule& schedule, PhysicalProblemBuilderKey );

        void set_output( OutputSettings output, PhysicalProblemBuilderKey );

        void set_gravity( const Vector3D& gravity, PhysicalProblemBuilderKey );

        void set_solver_option( std::string_view simulator,
            std::string_view key,
            std::string_view value,
            PhysicalProblemBuilderKey );

        void copy( const PhysicalProblem& other, PhysicalProblemBuilderKey );

    private:
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
