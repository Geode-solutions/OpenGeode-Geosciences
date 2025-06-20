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
#include <string_view>

#include <absl/types/span.h>

#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStack );
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStackBuilder );
    ALIAS_2D_AND_3D( HorizonsStack );
    ALIAS_2D_AND_3D( HorizonsStackBuilder );
    class CrossSection;
    class StructuralModel;
    class ImplicitCrossSection;
    class ImplicitStructuralModel;
    class StratigraphicSection;
    class StratigraphicModel;
    struct MeshElement;
    struct uuid;
} // namespace geode

namespace geode
{
    namespace detail
    {
        void opengeode_geosciences_implicit_api rescale_implicit_value(
            ImplicitCrossSection& section, double scaling_factor );

        void opengeode_geosciences_implicit_api
            rescale_implicit_value_to_bbox_scale(
                StratigraphicSection& section );

        void opengeode_geosciences_implicit_api rescale_implicit_value(
            ImplicitStructuralModel& model, double scaling_factor );

        void opengeode_geosciences_implicit_api
            rescale_implicit_value_to_bbox_scale( StratigraphicModel& model );

        void opengeode_geosciences_implicit_api save_stratigraphic_surfaces(
            const StratigraphicSection& section, std::string_view prefix );

        void opengeode_geosciences_implicit_api save_stratigraphic_blocks(
            const StratigraphicModel& model, std::string_view prefix );

        [[nodiscard]] ImplicitCrossSection opengeode_geosciences_implicit_api
            implicit_section_from_cross_section_scalar_field(
                CrossSection&& section,
                std::string_view scalar_attribute_name );

        [[nodiscard]] ImplicitStructuralModel opengeode_geosciences_implicit_api
            implicit_model_from_structural_model_scalar_field(
                StructuralModel&& model,
                std::string_view scalar_attribute_name );

        [[nodiscard]] StratigraphicModel opengeode_geosciences_implicit_api
            stratigraphic_model_from_implicit_model_and_coords(
                ImplicitStructuralModel&& implicit_model,
                local_index_t implicit_axis );

        /*!
         * Creates a HorizonsStack from a list of names of Horizons and
         * StratigraphicUnits. The top horizon and stratigraphic unit
         * will be the first in the lists, respectively.
         */
        template < index_t dimension >
        [[nodiscard]] HorizonsStack< dimension >
            horizons_stack_from_top_to_bottom_names(
                absl::Span< const std::string > horizons_names,
                absl::Span< const std::string > units_names );

        /*!
         * Creates a HorizonsStack from a list of names of Horizons and
         * StratigraphicUnits.The bottom horizon and stratigraphic unit
         * will be the first in the lists, respectively.
         */
        template < index_t dimension >
        [[nodiscard]] HorizonsStack< dimension >
            horizons_stack_from_bottom_to_top_names(
                absl::Span< const std::string > horizons_names,
                absl::Span< const std::string > units_names );

        template < index_t dimension >
        void repair_horizon_stack_if_possible(
            const HorizonsStack< dimension >& horizon_stack,
            HorizonsStackBuilder< dimension >& builder );

        template < index_t dimension >
        [[nodiscard]] std::optional< uuid > horizon_id_from_name(
            const HorizonsStack< dimension >& horizon_stack,
            std::string_view horizon_name );

        template < index_t dimension >
        [[nodiscard]] std::optional< uuid > stratigraphic_unit_id_from_name(
            const HorizonsStack< dimension >& horizon_stack,
            std::string_view unit_name );

        [[nodiscard]] std::vector< MeshElement >
            opengeode_geosciences_implicit_api invalid_stratigraphic_tetrahedra(
                const StratigraphicModel& model );
    } // namespace detail
} // namespace geode
