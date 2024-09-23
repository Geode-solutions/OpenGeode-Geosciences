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

#include <optional>

#include <geode/basic/pimpl.hpp>

#include <geode/geosciences/implicit/common.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_cross_section.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicPoint );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
    ALIAS_1D( Point );
    ALIAS_2D( Point );
    ALIAS_2D( StratigraphicPoint );
    ALIAS_2D( EdgedCurve );
    class StratigraphicSectionBuilder;
} // namespace geode

namespace geode
{
    /*!
     * A StratigraphicSection is an ImplicitCrossSection where each surface has
     * a specific attribute to store the stratigraphic coordinates of its
     * vertices.
     */
    class opengeode_geosciences_implicit_api StratigraphicSection
        : public ImplicitCrossSection
    {
        PASSKEY( StratigraphicSectionBuilder, StratigraphicSectionBuilderKey );

    public:
        static constexpr auto STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME =
            "geode_stratigraphic_location";
        static constexpr auto STRATIGRAPHIC_LINE_POLYGON_EDGE_ATTRIBUTE_NAME =
            "geode_associated_surface_polygon_edge";

        using stratigraphic_location_type = Point1D;
        StratigraphicSection();
        StratigraphicSection(
            StratigraphicSection&& stratigraphic_section ) noexcept;
        StratigraphicSection(
            ImplicitCrossSection&& implicit_section ) noexcept;
        StratigraphicSection( CrossSection&& cross_section ) noexcept;
        StratigraphicSection( const StratigraphicSection& initial_model,
            Section&& section,
            const ModelGenericMapping& initial_to_section_mappings ) noexcept;
        ~StratigraphicSection();

        [[nodiscard]] StratigraphicSection clone() const;

        [[nodiscard]] static constexpr std::string_view
            native_extension_static()
        {
            return "og_stgs";
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }

        /*!
         * Return the stratigraphic coordinates of the point at the given
         * vertex of the given surface.
         */
        [[nodiscard]] StratigraphicPoint2D stratigraphic_coordinates(
            const Surface2D& surface, index_t vertex_id ) const;

        /*!
         * Return the stratigraphic coordinates of the point, calculated
         * in the polygon containing the given point in the given surface, if
         * there is any.
         */
        [[nodiscard]] std::optional< StratigraphicPoint2D >
            stratigraphic_coordinates( const Surface2D& surface,
                const Point2D& geometric_point ) const;

        /*!
         * Return the stratigraphic coordinates of the point, calculated
         * in the given polygon of the given surface.
         */
        [[nodiscard]] StratigraphicPoint2D stratigraphic_coordinates(
            const Surface2D& surface,
            const Point2D& geometric_point,
            index_t polygon_id ) const;

        /*!
         * Return the geometric coordinates of the point, calculated from its
         * stratigraphic coordinates in the polygon containing the given
         * coordinates in the stratigraphic space in the given surface, if there
         * is any.
         */
        [[nodiscard]] std::optional< Point2D > geometric_coordinates(
            const Surface2D& surface,
            const StratigraphicPoint2D& stratigraphic_point ) const;

        /*!
         * Return the geometric coordinates of the point, calculated from
         * its stratigraphic coordinates in the given polygon of the given
         * surface.
         */
        [[nodiscard]] Point2D geometric_coordinates( const Surface2D& surface,
            const StratigraphicPoint2D& stratigraphic_point,
            index_t polygon_id ) const;

        /*!
         * Returns a surface polygon containing the given stratigraphic point,
         * if there is any.
         */
        [[nodiscard]] std::optional< index_t > stratigraphic_containing_polygon(
            const Surface2D& surface,
            const StratigraphicPoint2D& stratigraphic_point ) const;

        [[nodiscard]] absl::InlinedVector< std::unique_ptr< EdgedCurve2D >, 2 >
            stratigraphic_line(
                const Surface2D& surface, const Line2D& line ) const;

        [[nodiscard]] BoundingBox2D stratigraphic_bounding_box() const;

    public:
        void initialize_stratigraphic_query_trees(
            StratigraphicSectionBuilderKey );

        void instantiate_stratigraphic_location_on_surfaces(
            StratigraphicSectionBuilderKey );

        void set_stratigraphic_location( const Surface2D& surface,
            index_t vertex_id,
            Point1D value,
            StratigraphicSectionBuilderKey );

    private:
        void do_set_implicit_value( const Surface2D& surface,
            index_t vertex_id,
            double value ) override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
