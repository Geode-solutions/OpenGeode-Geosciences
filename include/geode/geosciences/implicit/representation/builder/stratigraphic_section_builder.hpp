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

#include <geode/geosciences/implicit/common.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.hpp>

namespace geode
{
    class StratigraphicSection;
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicPoint );
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    ALIAS_2D( StratigraphicPoint );
    ALIAS_1D( Point );
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StratigraphicSection
     */
    class opengeode_geosciences_implicit_api StratigraphicSectionBuilder
        : public ImplicitCrossSectionBuilder
    {
    public:
        explicit StratigraphicSectionBuilder(
            StratigraphicSection& stratigraphic_section );

        ModelCopyMapping copy(
            const StratigraphicSection& stratigraphic_section );

        void reinitialize_stratigraphic_query_trees();

        void instantiate_stratigraphic_attribute_on_surfaces();

        void set_stratigraphic_location(
            const Surface2D& surface, index_t vertex_id, Point1D value );

        void set_stratigraphic_coordinates( const Surface2D& surface,
            index_t vertex_id,
            const StratigraphicPoint2D& value );

    private:
        StratigraphicSection& stratigraphic_section_;
    };
} // namespace geode
