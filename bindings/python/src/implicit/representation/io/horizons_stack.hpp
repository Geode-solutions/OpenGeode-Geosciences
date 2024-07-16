/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/implicit/representation/io/horizons_stack_input.hpp>
#include <geode/geosciences/implicit/representation/io/horizons_stack_output.hpp>

#define PYTHON_HORIZONS_STACK_IO( dimension )                                  \
    const auto save##dimension =                                               \
        "save_horizons_stack" + std::to_string( dimension ) + "D";             \
    module.def( save##dimension.c_str(), &save_horizons_stack< dimension > );  \
    const auto load##dimension =                                               \
        "load_horizons_stack" + std::to_string( dimension ) + "D";             \
    module.def( load##dimension.c_str(), &load_horizons_stack< dimension > );  \
    const auto check##dimension = "check_horizons_stack_missing_files"         \
                                  + std::to_string( dimension ) + "D";         \
    module.def( check##dimension.c_str(),                                      \
        &check_horizons_stack_missing_files< dimension > );                    \
    const auto loadable##dimension =                                           \
        "is_horizons_stack_loadable" + std::to_string( dimension ) + "D";      \
    module.def( loadable##dimension.c_str(),                                   \
        &is_horizons_stack_loadable< dimension > );                            \
    PYTHON_INPUT_CLASS( HorizonsStack< dimension >,                            \
        "HorizonsStack" + std::to_string( dimension ) + "D" );                 \
    PYTHON_FACTORY_CLASS( HorizonsStackInputFactory##dimension##D );           \
    PYTHON_FACTORY_CLASS( HorizonsStackOutputFactory##dimension##D )

namespace geode
{
    void define_horizons_stack_io( pybind11::module& module )
    {
        PYTHON_HORIZONS_STACK_IO( 2 );
        PYTHON_HORIZONS_STACK_IO( 3 );
    }
} // namespace geode
