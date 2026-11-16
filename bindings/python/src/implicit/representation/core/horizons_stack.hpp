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

#include <geode/geometry/point.hpp>

#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>

#define PYTHON_HORIZONS_STACK_RANGE( dimension, function, type, what, order )  \
    .def(                                                                      \
        #function,                                                             \
        []( const HorizonsStack< dimension >& stack ) {                        \
            std::vector< const type< dimension >* > components;                \
            for( const auto& component : stack.function() )                    \
            {                                                                  \
                components.push_back( &component );                            \
            }                                                                  \
            return components;                                                 \
        },                                                                     \
        pybind11::return_value_policy::reference_internal,                     \
        "Return the " what " of the stack, ordered from " order ". The "       \
        "list is empty (with a warning) while the top and bottom horizons "    \
        "are not computed (compute_top_and_bottom_horizons of the builder)" )

#define PYTHON_HORIZONS_STACK( dimension )                                     \
    const auto name##dimension =                                               \
        "HorizonsStack" + std::to_string( dimension ) + "D";                   \
    pybind11::class_< HorizonsStack##dimension##D, StratigraphicRelationships, \
        Horizons##dimension##D, StratigraphicUnits##dimension##D, Identifier,  \
        pybind11::smart_holder >( module, name##dimension.c_str(),             \
        "Set of Horizons and StratigraphicUnits linked by the relations "      \
        "defining how they are stacked (which unit is above or under which "   \
        "horizon). It is modified through HorizonsStackBuilder; besides its "  \
        "own methods it inherits those of StratigraphicRelationships, "        \
        "Horizons and StratigraphicUnits" )                                    \
        .def( pybind11::init<>(), "Build an empty HorizonsStack" )             \
        .def( pybind11::init( []( const HorizonsStack##dimension##D& stack ) { \
            return stack.clone();                                              \
        } ),                                                                   \
            pybind11::arg( "stack" ),                                          \
            "Build a HorizonsStack by cloning the stack: a deep copy of its "  \
            "components, relations and identifier" )                           \
        .def( "clone", &HorizonsStack##dimension##D::clone,                    \
            "Return a deep copy of the stack" )                                \
        .def(                                                                  \
            "__repr__",                                                        \
            []( const HorizonsStack##dimension##D& stack ) {                   \
                return identifier_repr( stack,                                 \
                    "HorizonsStack" + std::to_string( dimension ) + "D",       \
                    { { "nb_horizons", python_repr( stack.nb_horizons() ) },   \
                        { "nb_stratigraphic_units",                            \
                            python_repr(                                       \
                                stack.nb_stratigraphic_units() ) } } );        \
            },                                                                 \
            "Return an unambiguous description of the stack: its type, its "   \
            "id, its name if it has one and its numbers of horizons and "      \
            "stratigraphic units" )                                            \
        .def( "string", &HorizonsStack##dimension##D::string,                  \
            "Return a readable description of the stack" )                     \
        .def( "native_extension",                                              \
            &HorizonsStack##dimension##D::native_extension,                    \
            "Return the extension of the native file format of the stack "     \
            "(og_hst2d or og_hst3d)" )                                         \
        .def( "top_horizon", &HorizonsStack##dimension##D::top_horizon,        \
            "Return the uuid of the top Horizon, or None if it is not "        \
            "computed (compute_top_and_bottom_horizons of the builder)" )      \
        .def( "bottom_horizon", &HorizonsStack##dimension##D::bottom_horizon,  \
            "Return the uuid of the bottom Horizon, or None if it is not "     \
            "computed (compute_top_and_bottom_horizons of the builder)" )      \
        .def( "is_conformal_above",                                            \
            &HorizonsStack##dimension##D::is_conformal_above,                  \
            pybind11::arg( "component" ),                                      \
            "Return True if the contact above the Horizon is conformal or "    \
            "an erosion, or if the Horizon above the StratigraphicUnit is "    \
            "conformal or a baselap. Raise an error if the component is not "  \
            "in the stack, or if the unit has no Horizon above" )              \
        .def( "is_conformal_under",                                            \
            &HorizonsStack##dimension##D::is_conformal_under,                  \
            pybind11::arg( "component" ),                                      \
            "Return True if the contact under the Horizon is conformal or "    \
            "a baselap, or if the Horizon under the StratigraphicUnit is "     \
            "conformal or an erosion. Raise an error if the component is "     \
            "not in the stack, or if the unit has no Horizon under" )          \
            PYTHON_HORIZONS_STACK_RANGE( dimension, bottom_to_top_horizons,    \
                Horizon, "Horizons", "the bottom to the top" )                 \
                PYTHON_HORIZONS_STACK_RANGE( dimension, bottom_to_top_units,   \
                    StratigraphicUnit, "StratigraphicUnits",                   \
                    "the bottom to the top" )                                  \
                    PYTHON_HORIZONS_STACK_RANGE( dimension,                    \
                        top_to_bottom_horizons, Horizon, "Horizons",           \
                        "the top to the bottom" )                              \
                        PYTHON_HORIZONS_STACK_RANGE( dimension,                \
                            top_to_bottom_units, StratigraphicUnit,            \
                            "StratigraphicUnits", "the top to the bottom" )    \
        .def( "horizon_stack_component",                                       \
            &HorizonsStack< dimension >::component, pybind11::arg( "id" ),     \
            pybind11::return_value_policy::reference_internal,                 \
            "Return the Horizon or StratigraphicUnit with the given uuid; "    \
            "raise an error if the stack has none" )

namespace geode
{
    void define_horizons_stack( pybind11::module& module )
    {
        PYTHON_HORIZONS_STACK( 2 );
        PYTHON_HORIZONS_STACK( 3 );
    }
} // namespace geode
