/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.h>

namespace geode
{
    class StratigraphicModel;
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicPoint );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_3D( StratigraphicPoint );
    ALIAS_2D( Point );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StratigraphicModel
     */
    class opengeode_geosciences_implicit_api StratigraphicModelBuilder
        : public ImplicitStructuralModelBuilder
    {
    public:
        StratigraphicModelBuilder( StratigraphicModel& stratigraphic_model_ );

        ModelCopyMapping copy( const StratigraphicModel& implicit_model );

        void reinitialize_stratigraphic_query_trees();

        void instantiate_stratigraphic_attribute_on_blocks();

        void set_stratigraphic_location(
            const Block3D& block, index_t vertex_id, Point2D value );

        void set_stratigraphic_coordinates( const Block3D& block,
            index_t vertex_id,
            const StratigraphicPoint3D& value );

    private:
        StratigraphicModel& stratigraphic_model_;
    };
} // namespace geode
