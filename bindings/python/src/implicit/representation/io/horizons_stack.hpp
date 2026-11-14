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

#include <geode/geosciences/implicit/representation/io/horizons_stack_input.hpp>
#include <geode/geosciences/implicit/representation/io/horizons_stack_output.hpp>

namespace geode
{
    void define_horizons_stack_io( pybind11::module& module )
    {
        module.def( "save_horizons_stack2D", &save_horizons_stack< 2 >,
            pybind11::arg( "horizons_stack" ), pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Save the HorizonsStack2D in the file, whose extension selects the "
            "format, "
            "and return the list of the written files; raise an error if the "
            "format is not supported" );
        module.def( "load_horizons_stack2D", &load_horizons_stack< 2 >,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Load the HorizonsStack2D objects of the file, with the default "
            "data "
            "structure, and return them as a list; raise an error if the file "
            "cannot be read" );
        module.def( "horizons_stack_object_priority2D",
            &horizons_stack_object_priority< 2 >, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the object priority of the input selected from the "
            "extension of the file; raise an error if no input handles the "
            "file" );
        module.def( "is_horizons_stack_loadable2D",
            &is_horizons_stack_loadable< 2 >, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a Percentage of how well the file can be loaded as a "
            "HorizonsStack2D, "
            "0 if it cannot" );
        module.def( "is_horizons_stack_saveable2D",
            &is_horizons_stack_saveable< 2 >, pybind11::arg( "horizons_stack" ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return True if the HorizonsStack2D can be saved in the file "
            "format given by "
            "the filename" );
        module.def( "horizons_stack_additional_files2D",
            &horizons_stack_additional_files< 2 >, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the AdditionalFiles (mandatory and optional) used when "
            "loading the file; raise an error if no loader handles the file" );
        PYTHON_FACTORY_CLASS( HorizonsStackInputFactory2D );
        PYTHON_FACTORY_CLASS( HorizonsStackOutputFactory2D );
        module.def( "save_horizons_stack3D", &save_horizons_stack< 3 >,
            pybind11::arg( "horizons_stack" ), pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Save the HorizonsStack3D in the file, whose extension selects the "
            "format, "
            "and return the list of the written files; raise an error if the "
            "format is not supported" );
        module.def( "load_horizons_stack3D", &load_horizons_stack< 3 >,
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Load the HorizonsStack3D objects of the file, with the default "
            "data "
            "structure, and return them as a list; raise an error if the file "
            "cannot be read" );
        module.def( "horizons_stack_object_priority3D",
            &horizons_stack_object_priority< 3 >, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the object priority of the input selected from the "
            "extension of the file; raise an error if no input handles the "
            "file" );
        module.def( "is_horizons_stack_loadable3D",
            &is_horizons_stack_loadable< 3 >, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return a Percentage of how well the file can be loaded as a "
            "HorizonsStack3D, "
            "0 if it cannot" );
        module.def( "is_horizons_stack_saveable3D",
            &is_horizons_stack_saveable< 3 >, pybind11::arg( "horizons_stack" ),
            pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return True if the HorizonsStack3D can be saved in the file "
            "format given by "
            "the filename" );
        module.def( "horizons_stack_additional_files3D",
            &horizons_stack_additional_files< 3 >, pybind11::arg( "filename" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Return the AdditionalFiles (mandatory and optional) used when "
            "loading the file; raise an error if no loader handles the file" );
        PYTHON_FACTORY_CLASS( HorizonsStackInputFactory3D );
        PYTHON_FACTORY_CLASS( HorizonsStackOutputFactory3D );
    }
} // namespace geode
