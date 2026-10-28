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

#include <geode/geosciences/simulation/core/saturation_functions.hpp>

namespace geode
{
    void define_saturation_functions( pybind11::module& module )
    {
        pybind11::class_< SaturationTable >( module, "SaturationTable" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( std::vector< double > saturations,
                                      std::vector< double > values ) {
                return SaturationTable{ std::move( saturations ),
                    std::move( values ) };
            } ),
                pybind11::arg( "saturations" ), pybind11::arg( "values" ) )
            .def_readwrite( "saturations", &SaturationTable::saturations )
            .def_readwrite( "values", &SaturationTable::values );

        pybind11::class_< RelativePermeability, pybind11::smart_holder >(
            module, "RelativePermeability" )
            .def( "type_name", &RelativePermeability::type_name )
            .def( "phases", &RelativePermeability::phases );

        pybind11::class_< BrooksCoreyRelativePermeability, RelativePermeability,
            pybind11::smart_holder >(
            module, "BrooksCoreyRelativePermeability" )
            .def( pybind11::init<>() )
            .def( pybind11::init< std::vector< PHASE >, std::vector< double >,
                      std::vector< double >, std::vector< double > >(),
                pybind11::arg( "phases" ),
                pybind11::arg( "residual_saturations" ),
                pybind11::arg( "exponents" ), pybind11::arg( "max_values" ) )
            .def_static( "type_name_static",
                &BrooksCoreyRelativePermeability::type_name_static )
            .def( "residual_saturations",
                &BrooksCoreyRelativePermeability::residual_saturations )
            .def( "exponents", &BrooksCoreyRelativePermeability::exponents )
            .def( "max_values", &BrooksCoreyRelativePermeability::max_values );

        pybind11::class_< TwoPhaseRelativePermeabilityTable >(
            module, "TwoPhaseRelativePermeabilityTable" )
            .def( pybind11::init<>() )
            .def_readwrite( "wetting_phase",
                &TwoPhaseRelativePermeabilityTable::wetting_phase )
            .def_readwrite( "non_wetting_phase",
                &TwoPhaseRelativePermeabilityTable::non_wetting_phase )
            .def_readwrite(
                "wetting_kr", &TwoPhaseRelativePermeabilityTable::wetting_kr )
            .def_readwrite( "non_wetting_kr",
                &TwoPhaseRelativePermeabilityTable::non_wetting_kr );

        pybind11::class_< TableRelativePermeability, RelativePermeability,
            pybind11::smart_holder >( module, "TableRelativePermeability" )
            .def( pybind11::init<>() )
            .def( pybind11::init< std::vector< PHASE >,
                      std::vector< TwoPhaseRelativePermeabilityTable > >(),
                pybind11::arg( "phases" ), pybind11::arg( "tables" ) )
            .def_static( "type_name_static",
                &TableRelativePermeability::type_name_static )
            .def( "tables", []( const TableRelativePermeability& relperm ) {
                const auto tables = relperm.tables();
                return std::vector< TwoPhaseRelativePermeabilityTable >{
                    tables.begin(), tables.end()
                };
            } );

        pybind11::class_< CapillaryPressure, pybind11::smart_holder >(
            module, "CapillaryPressure" )
            .def( "type_name", &CapillaryPressure::type_name )
            .def( "phases", &CapillaryPressure::phases );

        pybind11::class_< BrooksCoreyCapillaryPressure, CapillaryPressure,
            pybind11::smart_holder >( module, "BrooksCoreyCapillaryPressure" )
            .def( pybind11::init<>() )
            .def( pybind11::init< std::vector< PHASE >, std::vector< double >,
                      std::vector< double >, std::vector< double > >(),
                pybind11::arg( "phases" ),
                pybind11::arg( "residual_saturations" ),
                pybind11::arg( "entry_pressures" ),
                pybind11::arg( "pore_size_distribution_indices" ) )
            .def_static( "type_name_static",
                &BrooksCoreyCapillaryPressure::type_name_static )
            .def( "residual_saturations",
                &BrooksCoreyCapillaryPressure::residual_saturations )
            .def( "entry_pressures",
                &BrooksCoreyCapillaryPressure::entry_pressures )
            .def( "pore_size_distribution_indices",
                &BrooksCoreyCapillaryPressure::pore_size_distribution_indices );

        pybind11::class_< TwoPhaseCapillaryPressureTable >(
            module, "TwoPhaseCapillaryPressureTable" )
            .def( pybind11::init<>() )
            .def_readwrite( "wetting_phase",
                &TwoPhaseCapillaryPressureTable::wetting_phase )
            .def_readwrite( "non_wetting_phase",
                &TwoPhaseCapillaryPressureTable::non_wetting_phase )
            .def_readwrite(
                "pressure", &TwoPhaseCapillaryPressureTable::pressure );

        pybind11::class_< TableCapillaryPressure, CapillaryPressure,
            pybind11::smart_holder >( module, "TableCapillaryPressure" )
            .def( pybind11::init<>() )
            .def( pybind11::init< std::vector< PHASE >,
                      std::vector< TwoPhaseCapillaryPressureTable > >(),
                pybind11::arg( "phases" ), pybind11::arg( "tables" ) )
            .def_static(
                "type_name_static", &TableCapillaryPressure::type_name_static )
            .def( "tables",
                []( const TableCapillaryPressure& capillary_pressure ) {
                    const auto tables = capillary_pressure.tables();
                    return std::vector< TwoPhaseCapillaryPressureTable >{
                        tables.begin(), tables.end()
                    };
                } );
    }
} // namespace geode
