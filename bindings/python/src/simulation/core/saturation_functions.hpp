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

#include <geode/geosciences/simulation/core/saturation_functions.hpp>

namespace geode
{
    void define_saturation_functions( pybind11::module& module )
    {
        pybind11::class_< SaturationTable > table_class( module,
            "SaturationTable",
            "Sampled curve f(S) of the saturation S, linearly interpolated" );
        table_class.def( pybind11::init<>(), "Build an empty table" )
            .def( pybind11::init( []( std::vector< double > saturations,
                                      std::vector< double > values ) {
                return SaturationTable{ std::move( saturations ),
                    std::move( values ) };
            } ),
                pybind11::arg( "saturations" ), pybind11::arg( "values" ),
                "Build a table from the saturations, strictly increasing in "
                "[0, 1], and the value of the curve for each of them" )
            .def(
                "__repr__",
                []( const SaturationTable& table ) {
                    return python_call_repr( "SaturationTable",
                        { "saturations=" + python_repr( table.saturations ),
                            "values=" + python_repr( table.values ) } );
                },
                "Return an unambiguous description of the table, which can "
                "be evaluated" );
        def_readwrite_named( table_class, "saturations",
            &SaturationTable::saturations,
            "Saturations, strictly increasing in [0, 1]" );
        def_readwrite_named( table_class, "values", &SaturationTable::values,
            "Value of the curve for each saturation" );

        pybind11::class_< RelativePermeability, pybind11::smart_holder >(
            module, "RelativePermeability",
            "Relative permeability law of a Material. The leaf classes are "
            "BrooksCoreyRelativePermeability and TableRelativePermeability" )
            .def( "type_name", &RelativePermeability::type_name,
                "Return the name of the type of the law, written in the saved "
                "files" )
            .def( "phases", &RelativePermeability::phases,
                "Return the list of the Phases, whose order gives the phase "
                "index of every per-phase vector" );
        pybind11::class_< BrooksCoreyRelativePermeability, RelativePermeability,
            pybind11::smart_holder >( module, "BrooksCoreyRelativePermeability",
            "Brooks-Corey relative permeability law" )
            .def( pybind11::init<>(), "Build an empty law" )
            .def( pybind11::init< std::vector< PHASE >, std::vector< double >,
                      std::vector< double >, std::vector< double > >(),
                pybind11::arg( "phases" ),
                pybind11::arg( "residual_saturations" ),
                pybind11::arg( "exponents" ), pybind11::arg( "max_values" ),
                "Build a law from its phases and one residual saturation, "
                "exponent and maximum value per phase, in the phase order. "
                "The values are not validated (see SimulationChecker)" )
            .def_static( "type_name_static",
                &BrooksCoreyRelativePermeability::type_name_static,
                "Return the name of the type "
                "(BrooksCoreyRelativePermeability)" )
            .def( "residual_saturations",
                &BrooksCoreyRelativePermeability::residual_saturations,
                "Return the residual saturation of each phase" )
            .def( "exponents", &BrooksCoreyRelativePermeability::exponents,
                "Return the exponent of each phase" )
            .def( "max_values", &BrooksCoreyRelativePermeability::max_values,
                "Return the maximum relative permeability of each phase" );

        pybind11::class_< TwoPhaseRelativePermeabilityTable > two_phase_kr(
            module, "TwoPhaseRelativePermeabilityTable",
            "Relative permeability curves of two phases (SWOF or SGOF like)" );
        two_phase_kr
            .def( pybind11::init<>(),
                "Build a table of water (wetting) and oil (non wetting) with "
                "empty curves" )
            .def(
                "__repr__",
                []( const TwoPhaseRelativePermeabilityTable& table ) {
                    return "TwoPhaseRelativePermeabilityTable(wetting_phase="
                           + python_repr( table.wetting_phase )
                           + ", non_wetting_phase="
                           + python_repr( table.non_wetting_phase )
                           + ", wetting_kr=" + python_repr( table.wetting_kr )
                           + ", non_wetting_kr="
                           + python_repr( table.non_wetting_kr ) + ")";
                },
                "Return an unambiguous description of the table" );
        def_readwrite_named( two_phase_kr, "wetting_phase",
            &TwoPhaseRelativePermeabilityTable::wetting_phase,
            "Wetting Phase" );
        def_readwrite_named( two_phase_kr, "non_wetting_phase",
            &TwoPhaseRelativePermeabilityTable::non_wetting_phase,
            "Non wetting Phase" );
        def_readwrite_named( two_phase_kr, "wetting_kr",
            &TwoPhaseRelativePermeabilityTable::wetting_kr,
            "Relative permeability of the wetting phase versus its "
            "saturation" );
        def_readwrite_named( two_phase_kr, "non_wetting_kr",
            &TwoPhaseRelativePermeabilityTable::non_wetting_kr,
            "Relative permeability of the non wetting phase versus its "
            "saturation" );
        pybind11::class_< TableRelativePermeability, RelativePermeability,
            pybind11::smart_holder >( module, "TableRelativePermeability",
            "Relative permeability law given by tables" )
            .def( pybind11::init<>(), "Build an empty law" )
            .def( pybind11::init< std::vector< PHASE >,
                      std::vector< TwoPhaseRelativePermeabilityTable > >(),
                pybind11::arg( "phases" ), pybind11::arg( "tables" ),
                "Build a law from its phases and one table for two phases, "
                "two (water-oil and gas-oil) for three. The values are not "
                "validated (see SimulationChecker)" )
            .def_static( "type_name_static",
                &TableRelativePermeability::type_name_static,
                "Return the name of the type (TableRelativePermeability)" )
            .def(
                "tables",
                []( const TableRelativePermeability& relperm ) {
                    const auto tables = relperm.tables();
                    return std::vector< TwoPhaseRelativePermeabilityTable >{
                        tables.begin(), tables.end()
                    };
                },
                "Return the TwoPhaseRelativePermeabilityTables" );

        pybind11::class_< CapillaryPressure, pybind11::smart_holder >( module,
            "CapillaryPressure",
            "Capillary pressure law of a Material. The leaf classes are "
            "BrooksCoreyCapillaryPressure and TableCapillaryPressure" )
            .def( "type_name", &CapillaryPressure::type_name,
                "Return the name of the type of the law, written in the saved "
                "files" )
            .def( "phases", &CapillaryPressure::phases,
                "Return the list of the Phases, whose order gives the phase "
                "index of every per-phase vector" );
        pybind11::class_< BrooksCoreyCapillaryPressure, CapillaryPressure,
            pybind11::smart_holder >( module, "BrooksCoreyCapillaryPressure",
            "Brooks-Corey capillary pressure law: Pc = entry_pressure * "
            "Se^(-1/lambda)" )
            .def( pybind11::init<>(), "Build an empty law" )
            .def( pybind11::init< std::vector< PHASE >, std::vector< double >,
                      std::vector< double >, std::vector< double > >(),
                pybind11::arg( "phases" ),
                pybind11::arg( "residual_saturations" ),
                pybind11::arg( "entry_pressures" ),
                pybind11::arg( "pore_size_distribution_indices" ),
                "Build a law from its phases and one residual saturation, "
                "entry pressure (Pa) and pore size distribution index "
                "(lambda) per phase, in the phase order. The values are not "
                "validated (see SimulationChecker)" )
            .def_static( "type_name_static",
                &BrooksCoreyCapillaryPressure::type_name_static,
                "Return the name of the type (BrooksCoreyCapillaryPressure)" )
            .def( "residual_saturations",
                &BrooksCoreyCapillaryPressure::residual_saturations,
                "Return the residual saturation of each phase" )
            .def( "entry_pressures",
                &BrooksCoreyCapillaryPressure::entry_pressures,
                "Return the entry pressure of each phase in Pa" )
            .def( "pore_size_distribution_indices",
                &BrooksCoreyCapillaryPressure::pore_size_distribution_indices,
                "Return the pore size distribution index (lambda) of each "
                "phase" );

        pybind11::class_< TwoPhaseCapillaryPressureTable > two_phase_pc( module,
            "TwoPhaseCapillaryPressureTable",
            "Capillary pressure curve of two phases" );
        two_phase_pc
            .def( pybind11::init<>(),
                "Build a table of water (wetting) and oil (non wetting) with "
                "an empty curve" )
            .def(
                "__repr__",
                []( const TwoPhaseCapillaryPressureTable& table ) {
                    return "TwoPhaseCapillaryPressureTable(wetting_phase="
                           + python_repr( table.wetting_phase )
                           + ", non_wetting_phase="
                           + python_repr( table.non_wetting_phase )
                           + ", pressure=" + python_repr( table.pressure )
                           + ")";
                },
                "Return an unambiguous description of the table" );
        def_readwrite_named( two_phase_pc, "wetting_phase",
            &TwoPhaseCapillaryPressureTable::wetting_phase, "Wetting Phase" );
        def_readwrite_named( two_phase_pc, "non_wetting_phase",
            &TwoPhaseCapillaryPressureTable::non_wetting_phase,
            "Non wetting Phase" );
        def_readwrite_named( two_phase_pc, "pressure",
            &TwoPhaseCapillaryPressureTable::pressure,
            "Capillary pressure in Pa versus the wetting phase saturation" );
        pybind11::class_< TableCapillaryPressure, CapillaryPressure,
            pybind11::smart_holder >( module, "TableCapillaryPressure",
            "Capillary pressure law given by tables" )
            .def( pybind11::init<>(), "Build an empty law" )
            .def( pybind11::init< std::vector< PHASE >,
                      std::vector< TwoPhaseCapillaryPressureTable > >(),
                pybind11::arg( "phases" ), pybind11::arg( "tables" ),
                "Build a law from its phases and its tables. The values are "
                "not validated (see SimulationChecker)" )
            .def_static( "type_name_static",
                &TableCapillaryPressure::type_name_static,
                "Return the name of the type (TableCapillaryPressure)" )
            .def(
                "tables",
                []( const TableCapillaryPressure& capillary_pressure ) {
                    const auto tables = capillary_pressure.tables();
                    return std::vector< TwoPhaseCapillaryPressureTable >{
                        tables.begin(), tables.end()
                    };
                },
                "Return the TwoPhaseCapillaryPressureTables" );
    }
} // namespace geode
