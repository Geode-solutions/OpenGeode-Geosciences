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

#include <geode/basic/identifier_builder.hpp>

#include <geode/geometry/vector.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace geode
{
    class FlowModel;
    class InitialCondition;
    class PhysicalProblem;
    struct Material;
    struct OutputSettings;
    struct Schedule;
    struct SourceTerm;
    struct ThermalParameters;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Edits a PhysicalProblem. Polymorphic objects (flow, fluid, laws,
     * initial conditions) are shared, not copied: do not modify them after
     * handing them to the builder.
     */
    class opengeode_geosciences_simulation_api PhysicalProblemBuilder
        : public IdentifierBuilder
    {
        OPENGEODE_DISABLE_COPY( PhysicalProblemBuilder );

    public:
        explicit PhysicalProblemBuilder( PhysicalProblem& problem );
        PhysicalProblemBuilder( PhysicalProblemBuilder&& ) noexcept = default;

        void copy( const PhysicalProblem& other );

        void set_model_id( const uuid& model_id );

        void set_flow( std::shared_ptr< FlowModel > flow );

        void set_thermal( std::optional< ThermalParameters > thermal );

        index_t add_material( Material material );

        void set_material( index_t material_id, Material material );

        void remove_material( index_t material_id );

        index_t add_initial_condition(
            std::shared_ptr< InitialCondition > initial_condition );

        void remove_initial_condition( index_t initial_condition_id );

        index_t add_source_term( SourceTerm source_term );

        void remove_source_term( index_t source_term_id );

        void set_schedule( const Schedule& schedule );

        void set_output( OutputSettings output );

        void set_gravity( const Vector3D& gravity );

        void set_solver_option( std::string_view simulator,
            std::string_view key,
            std::string_view value );

    private:
        PhysicalProblem& problem_;
    };
} // namespace geode
