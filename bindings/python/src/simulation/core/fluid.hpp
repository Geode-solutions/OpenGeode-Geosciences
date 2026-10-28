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

#include <geode/geosciences/simulation/core/fluid.hpp>

namespace geode
{
    void define_fluid( pybind11::module& module )
    {
        pybind11::enum_< PHASE >( module, "PHASE" )
            .value( "oil", PHASE::oil )
            .value( "gas", PHASE::gas )
            .value( "water", PHASE::water );
        module.def( "phase_name", &phase_name );

        pybind11::class_< FluidComponent >( module, "FluidComponent" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( std::string name, double molar_weight ) {
                return FluidComponent{ std::move( name ), molar_weight };
            } ),
                pybind11::arg( "name" ), pybind11::arg( "molar_weight" ) )
            .def_readwrite( "name", &FluidComponent::name )
            .def_readwrite( "molar_weight", &FluidComponent::molar_weight );

        pybind11::class_< FluidThermalProperties >(
            module, "FluidThermalProperties" )
            .def( pybind11::init<>() )
            .def_readwrite( "specific_heat_capacity",
                &FluidThermalProperties::specific_heat_capacity )
            .def_readwrite( "thermal_expansion",
                &FluidThermalProperties::thermal_expansion )
            .def_readwrite( "viscosity_temperature_coefficient",
                &FluidThermalProperties::viscosity_temperature_coefficient );

        pybind11::class_< FluidModel, pybind11::smart_holder >(
            module, "FluidModel" )
            .def( "type_name", &FluidModel::type_name )
            .def( "phases", &FluidModel::phases )
            .def( "components", []( const FluidModel& fluid ) {
                const auto components = fluid.components();
                return std::vector< FluidComponent >{ components.begin(),
                    components.end() };
            } );
        module.def( "phase_component", &phase_component );

        pybind11::class_< SinglePhaseFluid, FluidModel,
            pybind11::smart_holder >( module, "SinglePhaseFluid" )
            .def( pybind11::init<>() )
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
                pybind11::arg( "thermal" ) = pybind11::none() )
            .def_static(
                "type_name_static", &SinglePhaseFluid::type_name_static )
            .def( "reference_density", &SinglePhaseFluid::reference_density )
            .def(
                "reference_viscosity", &SinglePhaseFluid::reference_viscosity )
            .def( "compressibility", &SinglePhaseFluid::compressibility )
            .def( "viscosibility", &SinglePhaseFluid::viscosibility )
            .def( "reference_pressure", &SinglePhaseFluid::reference_pressure )
            .def( "thermal", &SinglePhaseFluid::thermal );

        pybind11::class_< HydrocarbonPvtTable >( module, "HydrocarbonPvtTable" )
            .def( pybind11::init<>() )
            .def_readwrite( "phase", &HydrocarbonPvtTable::phase )
            .def_readwrite( "pressures", &HydrocarbonPvtTable::pressures )
            .def_readwrite( "formation_volume_factors",
                &HydrocarbonPvtTable::formation_volume_factors )
            .def_readwrite( "viscosities", &HydrocarbonPvtTable::viscosities );

        pybind11::class_< WaterPvt >( module, "WaterPvt" )
            .def( pybind11::init<>() )
            .def_readwrite(
                "reference_pressure", &WaterPvt::reference_pressure )
            .def_readwrite(
                "formation_volume_factor", &WaterPvt::formation_volume_factor )
            .def_readwrite( "compressibility", &WaterPvt::compressibility )
            .def_readwrite( "viscosity", &WaterPvt::viscosity );

        pybind11::class_< DeadOilFluid, FluidModel, pybind11::smart_holder >(
            module, "DeadOilFluid" )
            .def( pybind11::init<>() )
            .def( pybind11::init< std::vector< PHASE >,
                      std::vector< FluidComponent >, std::vector< double >,
                      std::vector< HydrocarbonPvtTable >,
                      std::optional< WaterPvt > >(),
                pybind11::arg( "phases" ), pybind11::arg( "components" ),
                pybind11::arg( "surface_densities" ),
                pybind11::arg( "hydrocarbon_pvt" ),
                pybind11::arg( "water_pvt" ) = pybind11::none() )
            .def_static( "type_name_static", &DeadOilFluid::type_name_static )
            .def( "surface_densities", &DeadOilFluid::surface_densities )
            .def( "hydrocarbon_pvt",
                []( const DeadOilFluid& fluid ) {
                    const auto tables = fluid.hydrocarbon_pvt();
                    return std::vector< HydrocarbonPvtTable >{ tables.begin(),
                        tables.end() };
                } )
            .def( "water_pvt", &DeadOilFluid::water_pvt );

        pybind11::enum_< CO2_BRINE_CORRELATION >(
            module, "CO2_BRINE_CORRELATION" )
            .value( "phillips", CO2_BRINE_CORRELATION::phillips )
            .value( "ezrokhi", CO2_BRINE_CORRELATION::ezrokhi );

        pybind11::class_< ValidityRange >( module, "ValidityRange" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( double min, double max, double step ) {
                return ValidityRange{ min, max, step };
            } ),
                pybind11::arg( "min" ), pybind11::arg( "max" ),
                pybind11::arg( "step" ) )
            .def_readwrite( "min", &ValidityRange::min )
            .def_readwrite( "max", &ValidityRange::max )
            .def_readwrite( "step", &ValidityRange::step );

        pybind11::class_< Co2BrineFluid, FluidModel, pybind11::smart_holder >(
            module, "Co2BrineFluid" )
            .def( pybind11::init<>() )
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
                    std::vector< double >{} )
            .def_static( "type_name_static", &Co2BrineFluid::type_name_static )
            .def( "correlation", &Co2BrineFluid::correlation )
            .def( "salinity", &Co2BrineFluid::salinity )
            .def( "pressure_range", &Co2BrineFluid::pressure_range )
            .def( "temperature_range", &Co2BrineFluid::temperature_range )
            .def( "ezrokhi_density_coefficients",
                &Co2BrineFluid::ezrokhi_density_coefficients )
            .def( "ezrokhi_viscosity_coefficients",
                &Co2BrineFluid::ezrokhi_viscosity_coefficients );
    }
} // namespace geode
