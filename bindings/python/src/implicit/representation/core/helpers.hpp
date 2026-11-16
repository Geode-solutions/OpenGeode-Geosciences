/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/geosciences/implicit/representation/core/detail/helpers.hpp>

#include <geode/basic/uuid.hpp>

#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.hpp>

namespace geode
{
    namespace detail
    {
        constexpr auto top_to_bottom_names_doc =
            "Create a HorizonsStack from Horizon and StratigraphicUnit names, "
            "both ordered from top to bottom. N horizons always create N+1 "
            "units: [unit above the top horizon, units between horizons, unit "
            "below the bottom horizon]. units_names may contain N+1 names (all "
            "units named), N names (the bottom unit is unnamed) or N-1 names "
            "(only the units between horizons are named).";
        constexpr auto bottom_to_top_names_doc =
            "Create a HorizonsStack from Horizon and StratigraphicUnit names, "
            "both ordered from bottom to top. N horizons always create N+1 "
            "units: [unit below the bottom horizon, units between horizons, "
            "unit above the top horizon]. units_names may contain N+1 names "
            "(all units named), N names (the top unit is unnamed) or N-1 names "
            "(only the units between horizons are named).";
        constexpr auto repair_doc =
            "Add the missing StratigraphicUnits under the bottom Horizon and "
            "above the top Horizon. Raise an error if the above/under "
            "relations between Horizons and StratigraphicUnits are missing or "
            "wrong in between";
        constexpr auto horizon_id_doc =
            "Return the uuid of the Horizon with the given name, or None if "
            "there is none";
        constexpr auto unit_id_doc =
            "Return the uuid of the StratigraphicUnit with the given name, or "
            "None if there is none";
    } // namespace detail

    namespace detail
    {
        void define_implicit_model_helpers( pybind11::module& module )
        {
            const pybind11::call_guard< pybind11::gil_scoped_release > nogil;
            module
                .def( "rescale_model_implicit_value",
                    static_cast< void ( * )( ImplicitStructuralModel&,
                        double ) >( &rescale_implicit_value ),
                    pybind11::arg( "model" ), pybind11::arg( "scaling_factor" ),
                    nogil,
                    "Multiply by scaling_factor the implicit value of every "
                    "vertex of the meshes of the Blocks having the implicit "
                    "attribute" )
                .def( "rescale_model_implicit_value_to_bbox_scale",
                    static_cast< void ( * )( StratigraphicModel& ) >(
                        &rescale_implicit_value_to_bbox_scale ),
                    pybind11::arg( "model" ), nogil,
                    "Multiply the implicit values by (stratigraphic x size * "
                    "model z size) / (model x size * stratigraphic z size), "
                    "the sizes being those of the bounding boxes, so that the "
                    "implicit axis has the scale of the geometry. Log a "
                    "warning and keep the values if the bounding boxes are "
                    "degenerate" )
                .def( "rescale_section_implicit_value",
                    static_cast< void ( * )( ImplicitCrossSection&, double ) >(
                        &rescale_implicit_value ),
                    pybind11::arg( "section" ),
                    pybind11::arg( "scaling_factor" ), nogil,
                    "Multiply by scaling_factor the implicit value of every "
                    "vertex of the meshes of the Surfaces having the implicit "
                    "attribute" )
                .def( "rescale_section_implicit_value_to_bbox_scale",
                    static_cast< void ( * )( StratigraphicSection& ) >(
                        &rescale_implicit_value_to_bbox_scale ),
                    pybind11::arg( "section" ), nogil,
                    "Multiply the implicit values by (stratigraphic x size * "
                    "section y size) / (section x size * stratigraphic y "
                    "size), the sizes being those of the bounding boxes, so "
                    "that the implicit axis has the scale of the geometry. "
                    "Log a warning and keep the values if the bounding boxes "
                    "are degenerate" )
                .def( "save_stratigraphic_surfaces",
                    &save_stratigraphic_surfaces, pybind11::arg( "section" ),
                    pybind11::arg( "prefix" ), nogil,
                    "Save the Surfaces of the section in the stratigraphic "
                    "space: for each Surface, the clone of its mesh, whose "
                    "points are replaced by the stratigraphic coordinates and "
                    "the original ones kept in the vertex attribute "
                    "geode_xyz, is saved in the file prefix + index + "
                    ".og_tsf2d. The meshes must be TriangulatedSurface2D" )
                .def( "save_stratigraphic_blocks", &save_stratigraphic_blocks,
                    pybind11::arg( "model" ), pybind11::arg( "prefix" ), nogil,
                    "Save the Blocks of the model in the stratigraphic space: "
                    "for each Block, the clone of its mesh, whose points are "
                    "replaced by the stratigraphic coordinates and the "
                    "original ones kept in the vertex attribute geode_xyz, is "
                    "saved in the file prefix + index + .og_tso3d. The "
                    "meshes must be TetrahedralSolid3D" )
                .def( "horizons_stack_from_top_to_bottom_names2D",
                    &horizons_stack_from_top_to_bottom_names< 2 >,
                    pybind11::arg( "horizons_names" ),
                    pybind11::arg( "units_names" ), top_to_bottom_names_doc )
                .def( "horizons_stack_from_top_to_bottom_names3D",
                    &horizons_stack_from_top_to_bottom_names< 3 >,
                    pybind11::arg( "horizons_names" ),
                    pybind11::arg( "units_names" ), top_to_bottom_names_doc )
                .def( "horizons_stack_from_bottom_to_top_names2D",
                    &horizons_stack_from_bottom_to_top_names< 2 >,
                    pybind11::arg( "horizons_names" ),
                    pybind11::arg( "units_names" ), bottom_to_top_names_doc )
                .def( "horizons_stack_from_bottom_to_top_names3D",
                    &horizons_stack_from_bottom_to_top_names< 3 >,
                    pybind11::arg( "horizons_names" ),
                    pybind11::arg( "units_names" ), bottom_to_top_names_doc )
                .def( "repair_horizon_stack_if_possible2D",
                    &repair_horizon_stack_if_possible< 2 >,
                    pybind11::arg( "horizons_stack" ),
                    pybind11::arg( "builder" ), repair_doc )
                .def( "repair_horizon_stack_if_possible3D",
                    &repair_horizon_stack_if_possible< 3 >,
                    pybind11::arg( "horizons_stack" ),
                    pybind11::arg( "builder" ), repair_doc )
                .def( "horizon_id_from_name2D", &horizon_id_from_name< 2 >,
                    pybind11::arg( "horizons_stack" ),
                    pybind11::arg( "horizon_name" ), horizon_id_doc )
                .def( "horizon_id_from_name3D", &horizon_id_from_name< 3 >,
                    pybind11::arg( "horizons_stack" ),
                    pybind11::arg( "horizon_name" ), horizon_id_doc )
                .def( "stratigraphic_unit_id_from_name2D",
                    &stratigraphic_unit_id_from_name< 2 >,
                    pybind11::arg( "horizons_stack" ),
                    pybind11::arg( "unit_name" ), unit_id_doc )
                .def( "stratigraphic_unit_id_from_name3D",
                    &stratigraphic_unit_id_from_name< 3 >,
                    pybind11::arg( "horizons_stack" ),
                    pybind11::arg( "unit_name" ), unit_id_doc )
                .def(
                    "implicit_section_from_cross_section_scalar_field",
                    []( CrossSection& model, const uuid& attribute_id ) {
                        return implicit_section_from_cross_section_scalar_field(
                            model.clone(), attribute_id );
                    },
                    pybind11::arg( "cross_section" ),
                    pybind11::arg( "scalar_attribute_id" ), nogil,
                    "Build an ImplicitCrossSection from a clone of the "
                    "CrossSection, whose implicit values are the values of "
                    "the scalar (double) vertex attribute of its Surfaces with "
                    "this uuid" )
                .def(
                    "implicit_model_from_structural_model_scalar_field",
                    []( StructuralModel& model, const uuid& attribute_id ) {
                        return implicit_model_from_structural_model_scalar_field(
                            model.clone(), attribute_id );
                    },
                    pybind11::arg( "structural_model" ),
                    pybind11::arg( "scalar_attribute_id" ), nogil,
                    "Build an ImplicitStructuralModel from a clone of the "
                    "StructuralModel, whose implicit values are the values of "
                    "the scalar (double) vertex attribute of its Blocks with "
                    "this uuid" )
                .def(
                    "stratigraphic_model_from_implicit_model_and_coords",
                    []( ImplicitStructuralModel& implicit_model,
                        local_index_t implicit_axis ) {
                        return stratigraphic_model_from_implicit_model_and_coords(
                            implicit_model.clone(), implicit_axis );
                    },
                    pybind11::arg( "implicit_model" ),
                    pybind11::arg( "implicit_axis" ), nogil,
                    "Build a StratigraphicModel from a clone of the "
                    "ImplicitStructuralModel: the stratigraphic location of "
                    "each vertex of the Blocks is made of two of its "
                    "coordinates, the ones other than implicit_axis (0, 1 or "
                    "2; an error is raised otherwise)" );
        }
    } // namespace detail
} // namespace geode
