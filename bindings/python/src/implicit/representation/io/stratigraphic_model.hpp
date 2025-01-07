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

#include "../../../factory.hpp"
#include "../../../input.hpp"

#include <geode/geosciences/implicit/representation/io/stratigraphic_model_input.hpp>
#include <geode/geosciences/implicit/representation/io/stratigraphic_model_output.hpp>

namespace geode
{
    void define_stratigraphic_model_io( pybind11::module& module )
    {
        module.def( "save_stratigraphic_model", &save_stratigraphic_model );
        module.def( "load_stratigraphic_model", &load_stratigraphic_model );
        module.def( "check_stratigraphic_model_missing_files",
            &check_stratigraphic_model_missing_files );
        module.def( "is_stratigraphic_model_loadable",
            &is_stratigraphic_model_loadable );
        module.def( "is_stratigraphic_model_saveable",
            &is_stratigraphic_model_saveable );
        PYTHON_INPUT_CLASS( StratigraphicModel, "StratigraphicModel" );
        PYTHON_FACTORY_CLASS( StratigraphicModelInputFactory );
        PYTHON_FACTORY_CLASS( StratigraphicModelOutputFactory );
    }
} // namespace geode
