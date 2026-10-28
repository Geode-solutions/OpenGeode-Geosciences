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

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>

namespace geode
{
    void define_flow_model( pybind11::module& module )
    {
        pybind11::enum_< COMPOSITION_BASIS >( module, "COMPOSITION_BASIS" )
            .value( "mole", COMPOSITION_BASIS::mole )
            .value( "mass", COMPOSITION_BASIS::mass );

        pybind11::class_< FlowModel, pybind11::smart_holder >(
            module, "FlowModel" )
            .def( "type_name", &FlowModel::type_name )
            .def( "has_fluid", &FlowModel::has_fluid )
            .def( "fluid", &FlowModel::fluid,
                pybind11::return_value_policy::reference_internal )
            .def( "set_fluid", &FlowModel::set_fluid );

        pybind11::class_< SinglePhaseFlow, FlowModel, pybind11::smart_holder >(
            module, "SinglePhaseFlow" )
            .def( pybind11::init<>() )
            .def_static(
                "type_name_static", &SinglePhaseFlow::type_name_static );

        pybind11::class_< MultiphaseFlow, FlowModel, pybind11::smart_holder >(
            module, "MultiphaseFlow" )
            .def( pybind11::init<>() )
            .def( pybind11::init< COMPOSITION_BASIS >(),
                pybind11::arg( "basis" ) )
            .def_static( "type_name_static", &MultiphaseFlow::type_name_static )
            .def( "basis", &MultiphaseFlow::basis );
    }
} // namespace geode
