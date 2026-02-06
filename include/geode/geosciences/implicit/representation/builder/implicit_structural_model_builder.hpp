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

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    class ImplicitStructuralModel;
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStack );
    ALIAS_3D( HorizonsStack );
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStackBuilder );
    ALIAS_3D( HorizonsStackBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a ImplicitStructuralModel
     */
    class opengeode_geosciences_implicit_api ImplicitStructuralModelBuilder
        : public StructuralModelBuilder
    {
    public:
        explicit ImplicitStructuralModelBuilder(
            ImplicitStructuralModel& implicit_model );

        ModelCopyMapping copy( const ImplicitStructuralModel& implicit_model );

        void copy_implicit_information( ModelCopyMapping& mapping,
            const ImplicitStructuralModel& other_model );

        void reinitialize_implicit_query_trees();

        void instantiate_implicit_attribute_on_blocks();

        void set_implicit_value(
            const Block3D& block, index_t vertex_id, double value );

        void set_horizons_stack( HorizonsStack3D&& stack );

        void set_horizon_implicit_value(
            const Horizon3D& horizon, double isovalue );

        [[nodiscard]] HorizonsStackBuilder3D horizons_stack_builder();

    private:
        ImplicitStructuralModel& implicit_model_;
    };
} // namespace geode
