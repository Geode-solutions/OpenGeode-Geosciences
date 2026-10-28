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

#include <geode/basic/uuid.hpp>

#include <geode/geosciences/simulation/core/initial_condition.hpp>

namespace geode
{
    void define_initial_condition( pybind11::module& module )
    {
        pybind11::class_< InitialCondition, pybind11::smart_holder >(
            module, "InitialCondition" )
            .def( "type_name", &InitialCondition::type_name )
            .def( "regions", &InitialCondition::regions );

        pybind11::class_< UniformInitialCondition, InitialCondition,
            pybind11::smart_holder >( module, "UniformInitialCondition" )
            .def( pybind11::init<>() )
            .def( pybind11::init< std::vector< uuid >, double,
                      std::optional< double >, std::vector< double > >(),
                pybind11::arg( "regions" ), pybind11::arg( "pressure" ),
                pybind11::arg( "temperature" ) = pybind11::none(),
                pybind11::arg( "composition" ) = std::vector< double >{} )
            .def_static(
                "type_name_static", &UniformInitialCondition::type_name_static )
            .def( "pressure", &UniformInitialCondition::pressure )
            .def( "temperature", &UniformInitialCondition::temperature )
            .def( "composition", &UniformInitialCondition::composition );
    }
} // namespace geode
