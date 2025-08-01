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

#include <geode/geosciences/implicit/representation/core/detail/helpers.hpp>

#include <geode/basic/uuid.hpp>

#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.hpp>

namespace geode
{
    namespace detail
    {
        void define_implicit_model_helpers( pybind11::module& module )
        {
            module
                .def( "rescale_model_implicit_value",
                    static_cast< void ( * )( ImplicitStructuralModel&,
                        double ) >( &rescale_implicit_value ) )
                .def( "rescale_model_implicit_value_to_bbox_scale",
                    static_cast< void ( * )( StratigraphicModel& ) >(
                        &rescale_implicit_value_to_bbox_scale ) )
                .def( "rescale_section_implicit_value",
                    static_cast< void ( * )( ImplicitCrossSection&, double ) >(
                        &rescale_implicit_value ) )
                .def( "rescale_section_implicit_value_to_bbox_scale",
                    static_cast< void ( * )( StratigraphicSection& ) >(
                        &rescale_implicit_value_to_bbox_scale ) )
                .def( "save_stratigraphic_surfaces",
                    &save_stratigraphic_surfaces )
                .def( "save_stratigraphic_blocks", &save_stratigraphic_blocks )
                .def( "horizons_stack_from_top_to_bottom_names_2d",
                    &horizons_stack_from_top_to_bottom_names< 2 > )
                .def( "horizons_stack_from_top_to_bottom_names_3d",
                    &horizons_stack_from_top_to_bottom_names< 3 > )
                .def( "horizons_stack_from_bottom_to_top_names_2d",
                    &horizons_stack_from_bottom_to_top_names< 2 > )
                .def( "horizons_stack_from_bottom_to_top_names_3d",
                    &horizons_stack_from_bottom_to_top_names< 3 > )
                .def( "repair_horizon_stack_if_possible_2d",
                    &repair_horizon_stack_if_possible< 2 > )
                .def( "repair_horizon_stack_if_possible_3d",
                    &repair_horizon_stack_if_possible< 3 > )
                .def( "horizon_id_from_name_2d", &horizon_id_from_name< 2 > )
                .def( "horizon_id_from_name_3d", &horizon_id_from_name< 3 > )
                .def( "implicit_section_from_cross_section_scalar_field",
                    []( CrossSection& model, std::string_view attribute_name ) {
                        return implicit_section_from_cross_section_scalar_field(
                            model.clone(), attribute_name );
                    } )
                .def( "implicit_model_from_structural_model_scalar_field",
                    []( StructuralModel& model,
                        std::string_view attribute_name ) {
                        return implicit_model_from_structural_model_scalar_field(
                            model.clone(), attribute_name );
                    } )
                .def( "stratigraphic_model_from_implicit_model_and_coords",
                    []( ImplicitStructuralModel& implicit_model,
                        local_index_t implicit_axis ) {
                        return stratigraphic_model_from_implicit_model_and_coords(
                            implicit_model.clone(), implicit_axis );
                    } );
        }
    } // namespace detail
} // namespace geode
