/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/representation/core/detail/helpers.h>

#include <geode/model/representation/core/brep.h>

#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

namespace geode
{
    namespace detail
    {
        void define_implicit_model_helpers( pybind11::module& module )
        {
            module
                .def( "rescale_model_implicit_value",
                    ( void ( * )( ImplicitStructuralModel&, double ) )
                        & rescale_implicit_value )
                .def( "rescale_model_implicit_value_to_bbox_scale",
                    ( void ( * )( StratigraphicModel& ) )
                        & rescale_implicit_value_to_bbox_scale )
                .def( "rescale_section_implicit_value",
                    ( void ( * )( ImplicitCrossSection&, double ) )
                        & rescale_implicit_value )
                .def( "rescale_section_implicit_value_to_bbox_scale",
                    ( void ( * )( StratigraphicSection& ) )
                        & rescale_implicit_value_to_bbox_scale )
                .def( "save_stratigraphic_surfaces",
                    &save_stratigraphic_surfaces )
                .def( "save_stratigraphic_blocks", &save_stratigraphic_blocks )
                .def( "implicit_section_from_cross_section_scalar_field",
                    []( CrossSection& model,
                        absl::string_view attribute_name ) {
                        return implicit_section_from_cross_section_scalar_field(
                            std::move( model ), attribute_name );
                    } )
                .def( "implicit_model_from_structural_model_scalar_field",
                    []( StructuralModel& model,
                        absl::string_view attribute_name ) {
                        return implicit_model_from_structural_model_scalar_field(
                            std::move( model ), attribute_name );
                    } )
                .def( "stratigraphic_model_from_implicit_model_and_coords",
                    []( ImplicitStructuralModel& implicit_model,
                        local_index_t implicit_axis ) {
                        return stratigraphic_model_from_implicit_model_and_coords(
                            std::move( implicit_model ), implicit_axis );
                    } );
        }
    } // namespace detail
} // namespace geode
