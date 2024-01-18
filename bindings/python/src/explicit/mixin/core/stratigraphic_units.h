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

#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>

#define PYTHON_STRATIGRAPHIC_UNITS( dimension )                                \
    const auto name##dimension =                                               \
        "StratigraphicUnits" + std::to_string( dimension ) + "D";              \
    pybind11::class_< StratigraphicUnits##dimension##D >(                      \
        module, name##dimension.c_str() )                                      \
        .def( "nb_stratigraphic_units",                                        \
            &StratigraphicUnits##dimension##D::nb_stratigraphic_units )        \
        .def( "stratigraphic_unit",                                            \
            &StratigraphicUnits##dimension##D::stratigraphic_unit,             \
            pybind11::return_value_policy::reference )                         \
        .def(                                                                  \
            "stratigraphic_units",                                             \
            []( const StratigraphicUnits##dimension##D& self ) {               \
                std::vector< const StratigraphicUnit##dimension##D* >          \
                    components;                                                \
                for( const auto& component : self.stratigraphic_units() )      \
                {                                                              \
                    components.push_back( &component );                        \
                }                                                              \
                return components;                                             \
            },                                                                 \
            pybind11::return_value_policy::reference )

namespace geode
{
    void define_stratigraphic_units( pybind11::module& module )
    {
        PYTHON_STRATIGRAPHIC_UNITS( 2 );
        PYTHON_STRATIGRAPHIC_UNITS( 3 );
    }
} // namespace geode
