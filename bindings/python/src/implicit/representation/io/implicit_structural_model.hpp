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

#include <geode/bindings/python/basic/factory.hpp>

#include <geode/geosciences/implicit/representation/io/implicit_structural_model_input.hpp>
#include <geode/geosciences/implicit/representation/io/implicit_structural_model_output.hpp>

namespace geode
{
    void define_implicit_structural_model_io( pybind11::module& module )
    {
        module.def( "save_implicit_structural_model",
            &save_implicit_structural_model,
            pybind11::arg( "implicit_structural_model" ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Save the ImplicitStructuralModel in the file, whose extension "
            "selects the format, "
            "and return the list of the written files; raise an error if the "
            "format is not supported" );
        module.def( "load_implicit_structural_model",
            &load_implicit_structural_model, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Load the ImplicitStructuralModel objects of the file, with the "
            "default data "
            "structure, and return them as a list; raise an error if the file "
            "cannot be read" );
        module.def( "implicit_structural_model_object_priority",
            &implicit_structural_model_object_priority,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the object priority of the input selected from the "
            "extension of the file; raise an error if no input handles the "
            "file" );
        module.def( "is_implicit_structural_model_loadable",
            &is_implicit_structural_model_loadable, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a Percentage of how well the file can be loaded as a "
            "ImplicitStructuralModel, "
            "0 if it cannot" );
        module.def( "is_implicit_structural_model_saveable",
            &is_implicit_structural_model_saveable,
            pybind11::arg( "implicit_structural_model" ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return True if the ImplicitStructuralModel can be saved in the "
            "file format given by "
            "the filename" );
        module.def( "implicit_structural_model_additional_files",
            &implicit_structural_model_additional_files,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the AdditionalFiles (mandatory and optional) used when "
            "loading the file; raise an error if no loader handles the file" );
        PYTHON_FACTORY_CLASS( ImplicitStructuralModelInputFactory );
        PYTHON_FACTORY_CLASS( ImplicitStructuralModelOutputFactory );
    }
} // namespace geode
