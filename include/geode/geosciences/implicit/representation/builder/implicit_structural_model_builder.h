/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.h>
#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitStructuralModel;
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStack );
    ALIAS_3D( StratigraphicUnitsStack );
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
        ImplicitStructuralModelBuilder(
            ImplicitStructuralModel& implicit_model );

        void copy( const ImplicitStructuralModel& implicit_model );

        void instantiate_implicit_attribute_on_blocks();

        void set_implicit_value(
            const Block3D& block, index_t vertex_id, double value );

        void set_stratigraphic_units_stack( StratigraphicUnitsStack3D&& stack );

        void set_horizon_isovalue( const Horizon3D& horizon, double isovalue );

    private:
        ImplicitStructuralModel& implicit_model_;
    };
} // namespace geode
