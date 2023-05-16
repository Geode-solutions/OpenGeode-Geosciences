/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/core/coordinate_reference_system_manager.h>

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.h>
#include <geode/geosciences/explicit/geometry/geographic_coordinate_system_helper.h>

#define PYTHON_CRS_HELPER( dimension )                                         \
    const auto name##dimension =                                               \
        "convert_attribute_to_geographic_coordinate_reference_system"          \
        + std::to_string( dimension ) + "D";                                   \
    module.def( name##dimension.c_str(),                                       \
        &convert_attribute_to_geographic_coordinate_reference_system<          \
            dimension > )

namespace geode
{
    void define_crs_helper( pybind11::module& module )
    {
        PYTHON_CRS_HELPER( 2 );
        PYTHON_CRS_HELPER( 3 );
        module
            .def( "convert_brep_coordinate_reference_system",
                &convert_brep_coordinate_reference_system )
            .def( "convert_section_coordinate_reference_system",
                &convert_section_coordinate_reference_system );
    }
} // namespace geode
