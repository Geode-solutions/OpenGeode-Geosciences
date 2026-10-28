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

#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/saturation_functions.hpp>

namespace geode
{
    void define_material( pybind11::module& module )
    {
        pybind11::class_< RockThermalProperties >(
            module, "RockThermalProperties" )
            .def( pybind11::init<>() )
            .def_readwrite( "volumetric_heat_capacity",
                &RockThermalProperties::volumetric_heat_capacity )
            .def_readwrite( "thermal_conductivity",
                &RockThermalProperties::thermal_conductivity );

        pybind11::class_< Material >( module, "Material" )
            .def( pybind11::init<>() )
            .def_readwrite( "name", &Material::name )
            .def_readwrite( "regions", &Material::regions )
            .def_readwrite(
                "pore_compressibility", &Material::pore_compressibility )
            .def_readwrite(
                "reference_pressure", &Material::reference_pressure )
            .def_readwrite( "porosity", &Material::porosity )
            .def_readwrite( "permeability", &Material::permeability )
            .def_readwrite(
                "relative_permeability", &Material::relative_permeability )
            .def_readwrite(
                "capillary_pressure", &Material::capillary_pressure )
            .def_readwrite( "thermal", &Material::thermal );
    }
} // namespace geode
