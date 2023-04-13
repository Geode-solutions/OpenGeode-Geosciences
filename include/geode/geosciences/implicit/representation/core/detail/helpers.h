/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class CrossSection;
    class StructuralModel;
    class ImplicitCrossSection;
    class ImplicitStructuralModel;
    class StratigraphicSection;
    class StratigraphicModel;
    struct MeshElement;
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
            const StratigraphicSection& section, absl::string_view prefix );

        void opengeode_geosciences_implicit_api save_stratigraphic_blocks(
            const StratigraphicModel& model, absl::string_view prefix );

        ImplicitCrossSection opengeode_geosciences_implicit_api
            implicit_section_from_cross_section_scalar_field(
                CrossSection&& section,
                absl::string_view scalar_attribute_name );

        ImplicitStructuralModel opengeode_geosciences_implicit_api
            implicit_model_from_structural_model_scalar_field(
                StructuralModel&& model,
                absl::string_view scalar_attribute_name );

        StratigraphicModel opengeode_geosciences_implicit_api
            stratigraphic_model_from_implicit_model_and_coords(
                ImplicitStructuralModel&& implicit_model,
                local_index_t implicit_axis );

        std::vector< MeshElement > opengeode_geosciences_implicit_api
            invalid_stratigraphic_tetrahedra( const StratigraphicModel& model );
    } // namespace detail
} // namespace geode
