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

#include <pybind11/native_enum.h>

#include <geode/geosciences/simulation/core/fluid.hpp>

namespace geode
{
    void define_fluid( pybind11::module& module )
    {
        pybind11::native_enum< PHASE >(
            module, "Phase", "enum.Enum", "Phase of a fluid" )
            .value( "oil", PHASE::oil )
            .value( "gas", PHASE::gas )
            .value( "water", PHASE::water )
            .finalize();
        module.def( "phase_name", &phase_name, pybind11::arg( "phase" ),
            "Return the name of the Phase: oil, gas or water" );

        pybind11::class_< FluidComponent > component_class( module,
            "FluidComponent", "Component of a fluid, with its molar weight" );
        component_class
            .def( pybind11::init<>(),
                "Build a component with an empty name and a null molar "
                "weight" )
            .def( pybind11::init( []( std::string name, double molar_weight ) {
                return FluidComponent{ std::move( name ), molar_weight };
            } ),
                pybind11::arg( "name" ), pybind11::arg( "molar_weight" ),
                "Build a component from its name and its molar weight in "
                "kg/mol" )
            .def(
                "__repr__",
                []( const FluidComponent& component ) {
                    return python_call_repr( "FluidComponent",
                        { "name=" + python_repr( component.name ),
                            "molar_weight="
                                + python_repr( component.molar_weight ) } );
                },
                "Return an unambiguous description of the component, which "
                "can be evaluated" );
        def_readwrite_named( component_class, "name", &FluidComponent::name,
            "Name of the component" );
        def_readwrite_named( component_class, "molar_weight",
            &FluidComponent::molar_weight, "Molar weight in kg/mol" );

        pybind11::class_< FluidThermalProperties > thermal_class(
            module, "FluidThermalProperties", "Thermal properties of a fluid" );
        thermal_class
            .def( pybind11::init<>(), "Build properties with null values" )
            .def(
                "__repr__",
                []( const FluidThermalProperties& thermal ) {
                    return "FluidThermalProperties(specific_heat_capacity="
                           + python_repr( thermal.specific_heat_capacity )
                           + ", thermal_expansion="
                           + python_repr( thermal.thermal_expansion )
                           + ", viscosity_temperature_coefficient="
                           + python_repr(
                               thermal.viscosity_temperature_coefficient )
                           + ")";
                },
                "Return an unambiguous description of the properties" );
        def_readwrite_named( thermal_class, "specific_heat_capacity",
            &FluidThermalProperties::specific_heat_capacity,
            "Specific heat capacity in J/(kg.K)" );
        def_readwrite_named( thermal_class, "thermal_expansion",
            &FluidThermalProperties::thermal_expansion,
            "Thermal expansion in 1/K" );
        def_readwrite_named( thermal_class, "viscosity_temperature_coefficient",
            &FluidThermalProperties::viscosity_temperature_coefficient,
            "Viscosity temperature coefficient in 1/K" );

        pybind11::class_< FluidModel, pybind11::smart_holder >( module,
            "FluidModel",
            "Fluid description shared by every simulator. The leaf classes "
            "are SinglePhaseFluid, DeadOilFluid and Co2BrineFluid" )
            .def( "type_name", &FluidModel::type_name,
                "Return the name of the type of the fluid, written in the "
                "saved files" )
            .def( "phases", &FluidModel::phases,
                "Return the list of the Phases, whose order gives the phase "
                "index of every per-phase vector" )
            .def(
                "components",
                []( const FluidModel& fluid ) {
                    const auto components = fluid.components();
                    return std::vector< FluidComponent >{ components.begin(),
                        components.end() };
                },
                "Return the list of the FluidComponents, whose order gives "
                "the component index of every composition vector" );
        module.def( "phase_component", &phase_component,
            pybind11::arg( "fluid" ), pybind11::arg( "phase" ),
            "Return the index of the component carried by the Phase, or None "
            "if the fluid has not one component per phase. The component "
            "index is the phase index: simulator writers use it to translate "
            "phase based data into component indices" );

        pybind11::class_< SinglePhaseFluid, FluidModel,
            pybind11::smart_holder >( module, "SinglePhaseFluid",
            "Fluid with a single phase, described by a reference density and "
            "viscosity and its compressibility" )
            .def( pybind11::init<>(), "Build an empty fluid" )
            .def(
                pybind11::init< std::vector< PHASE >,
                    std::vector< FluidComponent >, double, double, double,
                    double, double, std::optional< FluidThermalProperties > >(),
                pybind11::arg( "phases" ), pybind11::arg( "components" ),
                pybind11::arg( "reference_density" ),
                pybind11::arg( "reference_viscosity" ),
                pybind11::arg( "compressibility" ),
                pybind11::arg( "viscosibility" ),
                pybind11::arg( "reference_pressure" ),
                pybind11::arg( "thermal" ) = pybind11::none(),
                "Build a fluid from its phases and components, its reference "
                "density in kg/m3, reference viscosity in Pa.s, "
                "compressibility and viscosibility in 1/Pa, reference "
                "pressure in Pa and optional thermal properties. The values "
                "are not validated (see SimulationChecker)" )
            .def_static( "type_name_static",
                &SinglePhaseFluid::type_name_static,
                "Return the name of the type (SinglePhaseFluid)" )
            .def( "reference_density", &SinglePhaseFluid::reference_density,
                "Return the reference density in kg/m3" )
            .def( "reference_viscosity", &SinglePhaseFluid::reference_viscosity,
                "Return the reference viscosity in Pa.s" )
            .def( "compressibility", &SinglePhaseFluid::compressibility,
                "Return the compressibility in 1/Pa" )
            .def( "viscosibility", &SinglePhaseFluid::viscosibility,
                "Return the viscosibility in 1/Pa" )
            .def( "reference_pressure", &SinglePhaseFluid::reference_pressure,
                "Return the reference pressure in Pa" )
            .def( "thermal", &SinglePhaseFluid::thermal,
                "Return the FluidThermalProperties, or None if there are "
                "none" );

        pybind11::class_< HydrocarbonPvtTable > hydrocarbon_class( module,
            "HydrocarbonPvtTable",
            "Formation volume factor and viscosity of a hydrocarbon phase "
            "versus pressure (PVDO or PVDG like table)" );
        hydrocarbon_class
            .def( pybind11::init<>(), "Build an empty table of the oil" )
            .def(
                "__repr__",
                []( const HydrocarbonPvtTable& table ) {
                    return "HydrocarbonPvtTable(phase="
                           + python_repr( table.phase ) + ", nb_pressures="
                           + python_repr( table.pressures.size() ) + ")";
                },
                "Return an unambiguous description of the table: its phase "
                "and its number of pressures" );
        def_readwrite_named( hydrocarbon_class, "phase",
            &HydrocarbonPvtTable::phase, "Phase of the table (oil or gas)" );
        def_readwrite_named( hydrocarbon_class, "pressures",
            &HydrocarbonPvtTable::pressures,
            "Pressures in Pa, strictly increasing" );
        def_readwrite_named( hydrocarbon_class, "formation_volume_factors",
            &HydrocarbonPvtTable::formation_volume_factors,
            "Formation volume factor for each pressure" );
        def_readwrite_named( hydrocarbon_class, "viscosities",
            &HydrocarbonPvtTable::viscosities,
            "Viscosity in Pa.s for each pressure" );

        pybind11::class_< WaterPvt > water_class(
            module, "WaterPvt", "Water properties (PVTW like)" );
        water_class
            .def( pybind11::init<>(),
                "Build properties with a formation volume factor of 1 and "
                "the other values null" )
            .def(
                "__repr__",
                []( const WaterPvt& water ) {
                    return "WaterPvt(reference_pressure="
                           + python_repr( water.reference_pressure )
                           + ", formation_volume_factor="
                           + python_repr( water.formation_volume_factor )
                           + ", compressibility="
                           + python_repr( water.compressibility )
                           + ", viscosity=" + python_repr( water.viscosity )
                           + ")";
                },
                "Return an unambiguous description of the properties" );
        def_readwrite_named( water_class, "reference_pressure",
            &WaterPvt::reference_pressure, "Reference pressure in Pa" );
        def_readwrite_named( water_class, "formation_volume_factor",
            &WaterPvt::formation_volume_factor,
            "Formation volume factor at the reference pressure" );
        def_readwrite_named( water_class, "compressibility",
            &WaterPvt::compressibility, "Compressibility in 1/Pa" );
        def_readwrite_named( water_class, "viscosity", &WaterPvt::viscosity,
            "Viscosity in Pa.s" );

        pybind11::class_< DeadOilFluid, FluidModel, pybind11::smart_holder >(
            module, "DeadOilFluid",
            "Black oil fluid without dissolved gas: oil, gas and water "
            "described by PVT tables" )
            .def( pybind11::init<>(), "Build an empty fluid" )
            .def( pybind11::init< std::vector< PHASE >,
                      std::vector< FluidComponent >, std::vector< double >,
                      std::vector< HydrocarbonPvtTable >,
                      std::optional< WaterPvt > >(),
                pybind11::arg( "phases" ), pybind11::arg( "components" ),
                pybind11::arg( "surface_densities" ),
                pybind11::arg( "hydrocarbon_pvt" ),
                pybind11::arg( "water_pvt" ) = pybind11::none(),
                "Build a fluid from its phases and components, its surface "
                "densities in kg/m3 (one per phase), one HydrocarbonPvtTable "
                "per oil or gas phase, and the WaterPvt, required if and only "
                "if water is a phase. The values are not validated (see "
                "SimulationChecker)" )
            .def_static( "type_name_static", &DeadOilFluid::type_name_static,
                "Return the name of the type (DeadOilFluid)" )
            .def( "surface_densities", &DeadOilFluid::surface_densities,
                "Return the surface densities in kg/m3, one per phase" )
            .def(
                "hydrocarbon_pvt",
                []( const DeadOilFluid& fluid ) {
                    const auto tables = fluid.hydrocarbon_pvt();
                    return std::vector< HydrocarbonPvtTable >{ tables.begin(),
                        tables.end() };
                },
                "Return the HydrocarbonPvtTables, one per oil or gas phase" )
            .def( "water_pvt", &DeadOilFluid::water_pvt,
                "Return the WaterPvt, or None if water is not a phase" );

        pybind11::native_enum< CO2_BRINE_CORRELATION >( module,
            "Co2BrineCorrelation", "enum.Enum",
            "Correlation computing the properties of a CO2-brine fluid" )
            .value( "phillips", CO2_BRINE_CORRELATION::phillips )
            .value( "ezrokhi", CO2_BRINE_CORRELATION::ezrokhi )
            .finalize();
        pybind11::class_< ValidityRange > range_class( module, "ValidityRange",
            "Range of validity of a correlation, with the step of its table" );
        range_class.def( pybind11::init<>(), "Build a range with null values" )
            .def( pybind11::init( []( double min, double max, double step ) {
                return ValidityRange{ min, max, step };
            } ),
                pybind11::arg( "min" ), pybind11::arg( "max" ),
                pybind11::arg( "step" ),
                "Build a range from its bounds and its step" )
            .def(
                "__repr__",
                []( const ValidityRange& range ) {
                    return python_call_repr( "ValidityRange",
                        { "min=" + python_repr( range.min ),
                            "max=" + python_repr( range.max ),
                            "step=" + python_repr( range.step ) } );
                },
                "Return an unambiguous description of the range, which can be "
                "evaluated" );
        def_readwrite_named(
            range_class, "min", &ValidityRange::min, "Lower bound" );
        def_readwrite_named(
            range_class, "max", &ValidityRange::max, "Upper bound" );
        def_readwrite_named(
            range_class, "step", &ValidityRange::step, "Step of the table" );

        pybind11::class_< Co2BrineFluid, FluidModel, pybind11::smart_holder >(
            module, "Co2BrineFluid",
            "CO2-brine fluid computed from correlations. The thermal "
            "behavior is part of the correlation: the simulator writers "
            "select the thermal variant for a thermal problem" )
            .def( pybind11::init<>(), "Build an empty fluid" )
            .def( pybind11::init< std::vector< PHASE >,
                      std::vector< FluidComponent >, CO2_BRINE_CORRELATION,
                      double, ValidityRange, ValidityRange,
                      std::vector< double >, std::vector< double > >(),
                pybind11::arg( "phases" ), pybind11::arg( "components" ),
                pybind11::arg( "correlation" ), pybind11::arg( "salinity" ),
                pybind11::arg( "pressure_range" ),
                pybind11::arg( "temperature_range" ),
                pybind11::arg( "ezrokhi_density_coefficients" ) =
                    std::vector< double >{},
                pybind11::arg( "ezrokhi_viscosity_coefficients" ) =
                    std::vector< double >{},
                "Build a fluid from its phases and components, the "
                "Co2BrineCorrelation, the salinity in mol/kg, the validity "
                "ranges of the pressure (Pa) and of the temperature (K) and "
                "the coefficients of the ezrokhi correlation. The values are "
                "not validated (see SimulationChecker)" )
            .def_static( "type_name_static", &Co2BrineFluid::type_name_static,
                "Return the name of the type (Co2BrineFluid)" )
            .def( "correlation", &Co2BrineFluid::correlation,
                "Return the Co2BrineCorrelation" )
            .def( "salinity", &Co2BrineFluid::salinity,
                "Return the salinity in mol/kg" )
            .def( "pressure_range", &Co2BrineFluid::pressure_range,
                "Return the ValidityRange of the pressure in Pa" )
            .def( "temperature_range", &Co2BrineFluid::temperature_range,
                "Return the ValidityRange of the temperature in K" )
            .def( "ezrokhi_density_coefficients",
                &Co2BrineFluid::ezrokhi_density_coefficients,
                "Return the coefficients of the ezrokhi density correlation" )
            .def( "ezrokhi_viscosity_coefficients",
                &Co2BrineFluid::ezrokhi_viscosity_coefficients,
                "Return the coefficients of the ezrokhi viscosity "
                "correlation" );
    }
} // namespace geode
