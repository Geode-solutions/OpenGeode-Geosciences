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

#include <geode/geometry/bounding_box.hpp>

#include <geode/geosciences/explicit/representation/core/cross_section.hpp>

namespace geode
{
    void define_cross_section( pybind11::module& module )
    {
        pybind11::class_< CrossSection, Section, Faults2D, Horizons2D,
            FaultBlocks2D, StratigraphicUnits2D, pybind11::smart_holder >(
            module, "CrossSection",
            "Section composed of Faults and Horizons (as Lines) and "
            "FaultBlocks and StratigraphicUnits (as Surfaces). It is "
            "modified through CrossSectionBuilder; besides its own methods "
            "it inherits those of Section, Faults2D, Horizons2D, "
            "FaultBlocks2D and StratigraphicUnits2D" )
            .def( pybind11::init<>(), "Build an empty CrossSection" )
            .def( pybind11::init( []( const CrossSection& cross_section ) {
                return cross_section.clone();
            } ),
                pybind11::arg( "cross_section" ),
                "Build a CrossSection by cloning the cross_section: a deep "
                "copy of its components, geometry, relationships, geological "
                "components and identifier" )
            .def( pybind11::init( []( Section& section ) {
                return CrossSection{ section.clone() };
            } ),
                pybind11::arg( "section" ),
                "Build a CrossSection from a clone of the Section, without "
                "geological component" )
            .def(
                "__repr__",
                []( const CrossSection& model ) {
                    return identifier_repr( model, "CrossSection",
                        { { "nb_corners", python_repr( model.nb_corners() ) },
                            { "nb_lines", python_repr( model.nb_lines() ) },
                            { "nb_surfaces",
                                python_repr( model.nb_surfaces() ) },
                            { "nb_faults", python_repr( model.nb_faults() ) },
                            { "nb_horizons",
                                python_repr( model.nb_horizons() ) },
                            { "nb_fault_blocks",
                                python_repr( model.nb_fault_blocks() ) },
                            { "nb_stratigraphic_units",
                                python_repr(
                                    model.nb_stratigraphic_units() ) } } );
                },
                "Return an unambiguous description of the cross section: its "
                "type, its id, its name if it has one and its numbers of "
                "components and of geological components" )
            .def( "clone", &CrossSection::clone,
                "Return a deep copy of the cross section, with its "
                "geological components" )
            .def(
                "horizon_items",
                []( const CrossSection& cross_section,
                    const Horizon2D& horizon ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component :
                        cross_section.horizon_items( horizon ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "horizon" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines of the Horizon" )
            .def(
                "fault_items",
                []( const CrossSection& cross_section, const Fault2D& fault ) {
                    std::vector< const Line2D* > components;
                    for( const auto& component :
                        cross_section.fault_items( fault ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "fault" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Lines of the Fault" )
            .def(
                "fault_block_items",
                []( const CrossSection& cross_section,
                    const FaultBlock2D& fault_block ) {
                    std::vector< const Surface2D* > components;
                    for( const auto& component :
                        cross_section.fault_block_items( fault_block ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "fault_block" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces of the FaultBlock" )
            .def(
                "stratigraphic_unit_items",
                []( const CrossSection& cross_section,
                    const StratigraphicUnit2D& stratigraphic_unit ) {
                    std::vector< const Surface2D* > components;
                    for( const auto& component :
                        cross_section.stratigraphic_unit_items(
                            stratigraphic_unit ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "stratigraphic_unit" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces of the StratigraphicUnit" )
            .def( "native_extension", &CrossSection::native_extension,
                "Return the extension of the native file format of the "
                "cross section (og_xsctn)" )
            .def( "cross_section_component", &CrossSection::component,
                pybind11::arg( "id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component (mesh component, collection, fault, "
                "horizon, fault block or stratigraphic unit) with the given "
                "uuid; raise an error if the cross section has none" );
    }
} // namespace geode
