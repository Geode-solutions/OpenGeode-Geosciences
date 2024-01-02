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

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.h>
#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitCrossSection;
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStack );
    ALIAS_2D( HorizonsStack );
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStackBuilder );
    ALIAS_2D( HorizonsStackBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a ImplicitCrossSection
     */
    class opengeode_geosciences_implicit_api ImplicitCrossSectionBuilder
        : public CrossSectionBuilder
    {
    public:
        ImplicitCrossSectionBuilder( ImplicitCrossSection& implicit_section );

        ModelCopyMapping copy( const ImplicitCrossSection& implicit_model );

        void copy_implicit_information( ModelCopyMapping& mapping,
            const ImplicitCrossSection& other_model );

        void reinitialize_implicit_query_trees();

        void instantiate_implicit_attribute_on_surfaces();

        void set_implicit_value(
            const Surface2D& surface, index_t vertex_id, double value );

        void set_horizons_stack( HorizonsStack2D&& stack );

        void set_horizon_implicit_value(
            const Horizon2D& horizon, double isovalue );

        HorizonsStackBuilder2D horizons_stack_builder();

    private:
        ImplicitCrossSection& implicit_section_;
    };
} // namespace geode
