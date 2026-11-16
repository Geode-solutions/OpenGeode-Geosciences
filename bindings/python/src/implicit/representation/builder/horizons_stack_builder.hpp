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

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>

#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

#define PYTHON_HORIZONS_STACK_BUILDER( dimension )                             \
    using Builder##dimension = HorizonsStackBuilder##dimension##D;             \
    using Stack##dimension = HorizonsStack##dimension##D;                      \
    using Info##dimension = Builder##dimension::InsertedHorizonInfo;           \
    const auto info_name##dimension =                                          \
        "InsertedHorizonInfo" + std::to_string( dimension ) + "D";             \
    pybind11::class_< Info##dimension > info_class##dimension( module,         \
        info_name##dimension.c_str(),                                          \
        "Uuids of what add_horizon_in_stratigraphic_unit created: the new "    \
        "Horizon and the StratigraphicUnits above and under it" );             \
    def_readwrite_named( info_class##dimension, "new_horizon_id",              \
        &Info##dimension::new_horizon_id, "Uuid of the new Horizon" );         \
    def_readwrite_named( info_class##dimension, "strati_unit_above_id",        \
        &Info##dimension::strati_unit_above_id,                                \
        "Uuid of the new StratigraphicUnit above the new Horizon" );           \
    def_readwrite_named( info_class##dimension, "strati_unit_under_id",        \
        &Info##dimension::strati_unit_under_id,                                \
        "Uuid of the new StratigraphicUnit under the new Horizon" );           \
    info_class##dimension.def(                                                 \
        "__repr__",                                                            \
        []( const Info##dimension& info ) {                                    \
            return python_call_repr(                                           \
                "InsertedHorizonInfo" + std::to_string( dimension ) + "D",     \
                { "new_horizon_id=" + python_repr( info.new_horizon_id ),      \
                    "strati_unit_above_id="                                    \
                        + python_repr( info.strati_unit_above_id ),            \
                    "strati_unit_under_id="                                    \
                        + python_repr( info.strati_unit_under_id ) } );        \
        },                                                                     \
        "Return a description of the three uuids" );                           \
    const auto name##dimension =                                               \
        "HorizonsStackBuilder" + std::to_string( dimension ) + "D";            \
    pybind11::class_< Builder##dimension, StratigraphicRelationshipsBuilder,   \
        HorizonsBuilder##dimension##D,                                         \
        StratigraphicUnitsBuilder##dimension##D, IdentifierBuilder >( module,  \
        name##dimension.c_str(),                                               \
        "Builder of a HorizonsStack: the only way to modify it. It also "      \
        "inherits the methods of StratigraphicRelationshipsBuilder, "          \
        "HorizonsBuilder, StratigraphicUnitsBuilder and IdentifierBuilder" )   \
        .def( pybind11::init< Stack##dimension& >(),                           \
            pybind11::arg( "horizons_stack" ), pybind11::keep_alive< 1, 2 >(), \
            "Build a builder of the stack, which is kept alive by the "        \
            "builder" )                                                        \
        .def( "copy",                                                          \
            static_cast< ModelCopyMapping ( Builder##dimension::* )(           \
                const Stack##dimension& ) >( &Builder##dimension::copy ),      \
            pybind11::arg( "horizons_stack" ),                                 \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            "Copy the name, components, relations and top and bottom "         \
            "horizons of horizons_stack into the empty stack and return the "  \
            "ModelCopyMapping from its uuids to the new ones. Raise an "       \
            "error if the stack is not empty" )                                \
        .def( "copy",                                                          \
            static_cast< void ( Builder##dimension::* )( ModelCopyMapping&,    \
                const Stack##dimension& ) >( &Builder##dimension::copy ),      \
            pybind11::arg( "mapping" ), pybind11::arg( "horizons_stack" ),     \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            "Same as copy(horizons_stack), but the components keep the "       \
            "uuids given by the mapping: the components without mapping get "  \
            "a new uuid and are added to the mapping. Raise an error if the "  \
            "stack is not empty" )                                             \
        .def( "copy_components",                                               \
            static_cast< ModelCopyMapping ( Builder##dimension::* )(           \
                const Stack##dimension& ) >(                                   \
                &Builder##dimension::copy_components ),                        \
            pybind11::arg( "horizons_stack" ),                                 \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            "Copy the components, without their relations, of "                \
            "horizons_stack and return the ModelCopyMapping from its uuids "   \
            "to the new ones" )                                                \
        .def( "copy_components",                                               \
            static_cast< void ( Builder##dimension::* )(                       \
                ModelCopyMapping&, const Stack##dimension& ) >(                \
                &Builder##dimension::copy_components ),                        \
            pybind11::arg( "mapping" ), pybind11::arg( "horizons_stack" ),     \
            pybind11::call_guard< pybind11::gil_scoped_release >(),            \
            "Same as copy_components(horizons_stack), but the components "     \
            "keep the uuids given by the mapping: the components without "     \
            "mapping get a new uuid and are added to the mapping" )            \
        .def( "add_horizon",                                                   \
            static_cast< const uuid& (Builder##dimension::*) () >(             \
                &Builder##dimension::add_horizon ),                            \
            "Create a conformal Horizon, add it to the stack and return its "  \
            "uuid" )                                                           \
        .def( "add_horizon",                                                   \
            static_cast< void ( Builder##dimension::* )( uuid ) >(             \
                &Builder##dimension::add_horizon ),                            \
            pybind11::arg( "horizon_id" ),                                     \
            "Create a conformal Horizon with the given uuid and add it to "    \
            "the stack" )                                                      \
        .def( "add_stratigraphic_unit",                                        \
            static_cast< const uuid& (Builder##dimension::*) () >(             \
                &Builder##dimension::add_stratigraphic_unit ),                 \
            "Create a StratigraphicUnit, add it to the stack and return its "  \
            "uuid" )                                                           \
        .def( "add_stratigraphic_unit",                                        \
            static_cast< void ( Builder##dimension::* )( uuid ) >(             \
                &Builder##dimension::add_stratigraphic_unit ),                 \
            pybind11::arg( "stratigraphic_unit_id" ),                          \
            "Create a StratigraphicUnit with the given uuid and add it to "    \
            "the stack" )                                                      \
        .def( "set_horizon_above", &Builder##dimension::set_horizon_above,     \
            pybind11::arg( "horizon_above" ),                                  \
            pybind11::arg( "strati_unit_under" ),                              \
            "Set the Horizon directly above the StratigraphicUnit. Each "      \
            "component has at most one component directly above and one "      \
            "under: a relation conflicting with an existing one is ignored "   \
            "with a warning. Raise an error if the Horizon or the unit is "    \
            "not in the stack" )                                               \
        .def( "set_horizon_under", &Builder##dimension::set_horizon_under,     \
            pybind11::arg( "horizon_under" ),                                  \
            pybind11::arg( "strati_unit_above" ),                              \
            "Set the Horizon directly under the StratigraphicUnit. Each "      \
            "component has at most one component directly above and one "      \
            "under: a relation conflicting with an existing one is ignored "   \
            "with a warning. Raise an error if the Horizon or the unit is "    \
            "not in the stack" )                                               \
        .def( "remove_horizon", &Builder##dimension::remove_horizon,           \
            pybind11::arg( "horizon" ),                                        \
            "Remove the Horizon and its relations; the top and bottom "        \
            "horizons are recomputed if they were. The object passed, and "    \
            "every other Python object of this Horizon (from horizon(), "      \
            "horizons()...), refer to freed memory afterwards and must not "   \
            "be used: keep its uuid instead (has_horizon)" )                   \
        .def( "remove_stratigraphic_unit",                                     \
            &Builder##dimension::remove_stratigraphic_unit,                    \
            pybind11::arg( "stratigraphic_unit" ),                             \
            "Remove the StratigraphicUnit and its relations; the top and "     \
            "bottom horizons are recomputed if they were. The object passed, " \
            "and every other Python object of this StratigraphicUnit (from "   \
            "stratigraphic_unit(), stratigraphic_units()...), refer to freed " \
            "memory afterwards and must not be used: keep its uuid instead "   \
            "(has_stratigraphic_unit)" )                                       \
        .def( "add_horizon_in_stratigraphic_unit",                             \
            &Builder##dimension::add_horizon_in_stratigraphic_unit,            \
            pybind11::arg( "strati_unit" ),                                    \
            "Split the StratigraphicUnit with a new Horizon: the unit is "     \
            "replaced by two new units, one above and one under the new "      \
            "Horizon, which take its relations with the Horizons above and "   \
            "under it. Return the uuids of the new Horizon and units. Raise "  \
            "an error if the unit is not in the stack. The unit passed is "    \
            "removed: it, and every other Python object of this "              \
            "StratigraphicUnit, refer to freed memory afterwards and must "    \
            "not be used" )                                                    \
        .def( "compute_top_and_bottom_horizons",                               \
            &Builder##dimension::compute_top_and_bottom_horizons,              \
            "Compute the top and bottom Horizons from the relations, which "   \
            "are needed to iterate on the stack (they are reset, with a "      \
            "warning, if the stack has no Horizon)" )

namespace geode
{
    void define_horizons_stack_builder( pybind11::module& module )
    {
        PYTHON_HORIZONS_STACK_BUILDER( 2 );
        PYTHON_HORIZONS_STACK_BUILDER( 3 );
    }
} // namespace geode
