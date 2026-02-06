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

#include "../../../factory.hpp"

#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_output.hpp>

namespace geode
{
    void define_structural_model_io( pybind11::module& module )
    {
        module.def( "save_structural_model", &save_structural_model );
        module.def( "load_structural_model", &load_structural_model );
        module.def( "structural_model_object_priority",
            &structural_model_object_priority );
        module.def(
            "is_structural_model_loadable", &is_structural_model_loadable );
        module.def(
            "is_structural_model_saveable", &is_structural_model_saveable );
        module.def( "structural_model_additional_files",
            &structural_model_additional_files );
        PYTHON_FACTORY_CLASS( StructuralModelInputFactory );
        PYTHON_FACTORY_CLASS( StructuralModelOutputFactory );
    }
} // namespace geode
