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

#include <pybind11/native_enum.h>

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>

namespace geode
{
    void define_flow_model( pybind11::module& module )
    {
        pybind11::native_enum< COMPOSITION_BASIS >( module, "CompositionBasis",
            "enum.Enum",
            "Basis of the compositions and rates of a multiphase flow: mole "
            "fractions (mol) or mass fractions (kg)" )
            .value( "mole", COMPOSITION_BASIS::mole )
            .value( "mass", COMPOSITION_BASIS::mass )
            .finalize();
        pybind11::class_< FlowModel, pybind11::smart_holder >( module,
            "FlowModel",
            "Flow process of a PhysicalProblem, with its fluid. The leaf "
            "classes are SinglePhaseFlow and MultiphaseFlow" )
            .def( "type_name", &FlowModel::type_name,
                "Return the name of the type of the flow model, written in "
                "the saved files" )
            .def( "has_fluid", &FlowModel::has_fluid,
                "Return True if the fluid of the flow model is set" )
            .def( "fluid", &FlowModel::shared_fluid,
                "Return the FluidModel of the flow model, shared with it: it "
                "stays valid after set_fluid or the destruction of the flow "
                "model. Raise an error if it has none" )
            .def( "set_fluid", &FlowModel::set_fluid, pybind11::arg( "fluid" ),
                "Set the FluidModel of the flow model, which is shared: do "
                "not modify it afterwards. Raise an error if the fluid is "
                "None" );
        pybind11::class_< SinglePhaseFlow, FlowModel, pybind11::smart_holder >(
            module, "SinglePhaseFlow", "Flow of a single fluid phase" )
            .def(
                pybind11::init<>(), "Build a single phase flow, without fluid" )
            .def_static( "type_name_static", &SinglePhaseFlow::type_name_static,
                "Return the name of the type (SinglePhaseFlow)" );
        pybind11::class_< MultiphaseFlow, FlowModel, pybind11::smart_holder >(
            module, "MultiphaseFlow",
            "Flow of several fluid phases and components" )
            .def( pybind11::init<>(),
                "Build a multiphase flow with a mole based composition, "
                "without fluid" )
            .def( pybind11::init< COMPOSITION_BASIS >(),
                pybind11::arg( "basis" ),
                "Build a multiphase flow, without fluid, whose compositions "
                "and rates use the CompositionBasis" )
            .def_static( "type_name_static", &MultiphaseFlow::type_name_static,
                "Return the name of the type (MultiphaseFlow)" )
            .def( "basis", &MultiphaseFlow::basis,
                "Return the CompositionBasis of the flow" );
    }
} // namespace geode
