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

#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/saturation_functions.hpp>

namespace geode
{
    void define_material( pybind11::module& module )
    {
        pybind11::class_< RockThermalProperties > thermal_class(
            module, "RockThermalProperties", "Thermal properties of a rock" );
        thermal_class
            .def( pybind11::init<>(), "Build properties with null values" )
            .def(
                "__repr__",
                []( const RockThermalProperties& thermal ) {
                    return "RockThermalProperties(volumetric_heat_capacity="
                           + python_repr( thermal.volumetric_heat_capacity )
                           + ", thermal_conductivity="
                           + python_repr( thermal.thermal_conductivity ) + ")";
                },
                "Return an unambiguous description of the properties" );
        def_readwrite_named( thermal_class, "volumetric_heat_capacity",
            &RockThermalProperties::volumetric_heat_capacity,
            "Volumetric heat capacity in J/(m3.K)" );
        def_readwrite_named( thermal_class, "thermal_conductivity",
            &RockThermalProperties::thermal_conductivity,
            "Thermal conductivity in W/(m.K) along x, y and z" );

        pybind11::class_< Material > material_class( module, "Material",
            "Rock constants of model regions. The porosity and the "
            "permeability varying in space are read from the "
            "PhysicalProperties of the model; the constants of the Material "
            "are used where there is none" );
        material_class.def( pybind11::init<>(), "Build an empty material" )
            .def(
                "__repr__",
                []( const Material& material ) {
                    return "Material(name=" + python_repr( material.name )
                           + ", nb_regions="
                           + python_repr( material.regions.size() ) + ")";
                },
                "Return an unambiguous description of the material: its name "
                "and its number of regions" );
        def_readwrite_named(
            material_class, "name", &Material::name, "Name of the material" );
        def_readwrite_named( material_class, "regions", &Material::regions,
            "Uuids of the model components or collections (Blocks, "
            "StratigraphicUnits...) of the material; empty means every "
            "region without another material" );
        def_readwrite_named( material_class, "pore_compressibility",
            &Material::pore_compressibility, "Pore compressibility in 1/Pa" );
        def_readwrite_named( material_class, "reference_pressure",
            &Material::reference_pressure, "Reference pressure in Pa" );
        def_readwrite_named( material_class, "porosity", &Material::porosity,
            "Constant porosity, or None" );
        def_readwrite_named( material_class, "permeability",
            &Material::permeability,
            "Constant permeability in m2 along x, y and z, or None" );
        def_readwrite_named( material_class, "relative_permeability",
            &Material::relative_permeability,
            "RelativePermeability law, shared: do not modify it once given to "
            "a PhysicalProblem" );
        def_readwrite_named( material_class, "capillary_pressure",
            &Material::capillary_pressure,
            "Optional CapillaryPressure law, shared: do not modify it once "
            "given to a PhysicalProblem" );
        def_readwrite_named( material_class, "thermal", &Material::thermal,
            "RockThermalProperties, None until properties are assigned" );
    }
} // namespace geode
