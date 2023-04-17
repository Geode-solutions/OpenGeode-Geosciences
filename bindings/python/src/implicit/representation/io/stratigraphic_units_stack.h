/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include "../../factory.h"

#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_input.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_output.h>

#define PYTHON_STRATIGRAPHIC_UNITS_STACK_IO( dimension )                       \
    const auto save##dimension =                                               \
        "save_stratigraphic_units_stack" + std::to_string( dimension ) + "D";  \
    module.def( save##dimension.c_str(),                                       \
        &save_stratigraphic_units_stack< dimension > );                        \
    const auto load##dimension =                                               \
        "load_stratigraphic_units_stack" + std::to_string( dimension ) + "D";  \
    module.def( load##dimension.c_str(),                                       \
        &load_stratigraphic_units_stack< dimension > );                        \
    PYTHON_FACTORY_CLASS( StratigraphicUnitsStackInputFactory##dimension##D ); \
    PYTHON_FACTORY_CLASS( StratigraphicUnitsStackOutputFactory##dimension##D )

namespace geode
{
    void define_stratigraphic_units_stack_io( pybind11::module& module )
    {
        PYTHON_STRATIGRAPHIC_UNITS_STACK_IO( 2 );
        PYTHON_STRATIGRAPHIC_UNITS_STACK_IO( 3 );
    }
} // namespace geode
