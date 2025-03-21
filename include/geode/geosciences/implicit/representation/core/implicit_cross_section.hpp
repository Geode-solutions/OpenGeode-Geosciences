/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geosciences/explicit/representation/core/cross_section.hpp>
#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStack );
    ALIAS_2D( Point );
    ALIAS_2D( HorizonsStack );
    class ImplicitCrossSectionBuilder;
} // namespace geode

namespace geode
{
    /*!
     * An ImplicitCrossSection is a CrossSection where each surface has a
     * specific attribute to store the stratigraphic coordinates of its
     * vertices.
     */
    class opengeode_geosciences_implicit_api ImplicitCrossSection
        : public CrossSection
    {
        PASSKEY( ImplicitCrossSectionBuilder, ImplicitCrossSectionBuilderKey );

    public:
        static constexpr auto IMPLICIT_ATTRIBUTE_NAME =
            "geode_implicit_attribute";
        using implicit_attribute_type = double;
        ImplicitCrossSection();
        ImplicitCrossSection( ImplicitCrossSection&& implicit_model ) noexcept;
        explicit ImplicitCrossSection( CrossSection&& cross_section ) noexcept;
        ImplicitCrossSection( const ImplicitCrossSection& initial_model,
            Section&& section,
            const ModelGenericMapping& initial_to_section_mappings ) noexcept;
        virtual ~ImplicitCrossSection();

        [[nodiscard]] ImplicitCrossSection clone() const;

        [[nodiscard]] static constexpr std::string_view
            native_extension_static()
        {
            return "og_ixsctn";
        }

        [[nodiscard]] std::string_view native_extension() const
        {
            return native_extension_static();
        }

        /*!
         * Return the implicit value of the given vertex of the given surface.
         */
        [[nodiscard]] double implicit_value(
            const Surface2D& surface, index_t vertex_id ) const;

        /*!
         * Return the implicit value of the point, calculated in the polygon
         * containing the given point in the given surface, if there is any.
         */
        [[nodiscard]] std::optional< double > implicit_value(
            const Surface2D& surface, const Point2D& point ) const;

        /*!
         * Return the implicit value of the point, calculated in the given
         * polygon of the given surface.
         */
        [[nodiscard]] double implicit_value( const Surface2D& surface,
            const Point2D& point,
            index_t polygon_id ) const;

        /*!
         * Returns the surface polygon containing the given point, if there is
         * any.
         */
        [[nodiscard]] std::optional< index_t > containing_polygon(
            const Surface2D& surface, const Point2D& point ) const;

        [[nodiscard]] const HorizonsStack2D& horizons_stack() const;

        [[nodiscard]] std::optional< implicit_attribute_type >
            horizon_implicit_value( const Horizon2D& horizon ) const;

        [[nodiscard]] bool implicit_value_is_above_horizon(
            double implicit_function_value, const Horizon2D& horizon ) const;

        [[nodiscard]] std::optional< uuid > containing_stratigraphic_unit(
            implicit_attribute_type implicit_function_value ) const;

    public:
        void initialize_implicit_query_trees( ImplicitCrossSectionBuilderKey );

        void instantiate_implicit_attribute_on_surfaces(
            ImplicitCrossSectionBuilderKey );

        void set_implicit_value( const Surface2D& surface,
            index_t vertex_id,
            double value,
            ImplicitCrossSectionBuilderKey );

        void set_horizons_stack(
            HorizonsStack2D&& stack, ImplicitCrossSectionBuilderKey );

        void set_horizon_implicit_value( const Horizon2D& horizon,
            implicit_attribute_type isovalue,
            ImplicitCrossSectionBuilderKey );

        [[nodiscard]] HorizonsStack2D& modifiable_horizons_stack(
            ImplicitCrossSectionBuilderKey );

    protected:
        virtual void do_set_implicit_value(
            const Surface2D& surface, index_t vertex_id, double value );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
