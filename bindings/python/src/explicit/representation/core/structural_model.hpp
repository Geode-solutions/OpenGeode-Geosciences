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

#include <geode/geosciences/explicit/representation/core/structural_model.hpp>

namespace geode
{
    void define_structural_model( pybind11::module& module )
    {
        pybind11::class_< StructuralModel, BRep, Faults3D, Horizons3D,
            FaultBlocks3D, StratigraphicUnits3D, pybind11::smart_holder >(
            module, "StructuralModel",
            "BRep composed of Faults and Horizons (as Surfaces) and "
            "FaultBlocks and StratigraphicUnits (as Blocks). It is modified "
            "through StructuralModelBuilder; besides its own methods it "
            "inherits those of BRep, Faults3D, Horizons3D, FaultBlocks3D and "
            "StratigraphicUnits3D" )
            .def( pybind11::init<>(), "Build an empty StructuralModel" )
            .def( pybind11::init( []( const StructuralModel& model ) {
                return model.clone();
            } ),
                pybind11::arg( "model" ),
                "Build a StructuralModel by cloning the model: a deep copy of "
                "its components, geometry, relationships, physical "
                "properties, geological components and identifier" )
            .def( pybind11::init( []( BRep& model ) {
                return StructuralModel{ model.clone() };
            } ),
                pybind11::arg( "brep" ),
                "Build a StructuralModel from a clone of the BRep, without "
                "geological component" )
            .def(
                "__repr__",
                []( const StructuralModel& model ) {
                    return identifier_repr( model, "StructuralModel",
                        { { "nb_corners", python_repr( model.nb_corners() ) },
                            { "nb_lines", python_repr( model.nb_lines() ) },
                            { "nb_surfaces",
                                python_repr( model.nb_surfaces() ) },
                            { "nb_blocks", python_repr( model.nb_blocks() ) },
                            { "nb_faults", python_repr( model.nb_faults() ) },
                            { "nb_horizons",
                                python_repr( model.nb_horizons() ) },
                            { "nb_fault_blocks",
                                python_repr( model.nb_fault_blocks() ) },
                            { "nb_stratigraphic_units",
                                python_repr(
                                    model.nb_stratigraphic_units() ) } } );
                },
                "Return an unambiguous description of the model: its type, "
                "its id, its name if it has one and its numbers of components "
                "and of geological components" )
            .def( "clone", &StructuralModel::clone,
                "Return a deep copy of the model, with its geological "
                "components" )
            .def(
                "horizon_items",
                []( const StructuralModel& structural_model,
                    const Horizon3D& horizon ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component :
                        structural_model.horizon_items( horizon ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "horizon" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces of the Horizon" )
            .def(
                "fault_items",
                []( const StructuralModel& structural_model,
                    const Fault3D& fault ) {
                    std::vector< const Surface3D* > components;
                    for( const auto& component :
                        structural_model.fault_items( fault ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "fault" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Surfaces of the Fault" )
            .def(
                "fault_block_items",
                []( const StructuralModel& structural_model,
                    const FaultBlock3D& fault_block ) {
                    std::vector< const Block3D* > components;
                    for( const auto& component :
                        structural_model.fault_block_items( fault_block ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "fault_block" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Blocks of the FaultBlock" )
            .def(
                "stratigraphic_unit_items",
                []( const StructuralModel& structural_model,
                    const StratigraphicUnit3D& stratigraphic_unit ) {
                    std::vector< const Block3D* > components;
                    for( const auto& component :
                        structural_model.stratigraphic_unit_items(
                            stratigraphic_unit ) )
                    {
                        components.push_back( &component );
                    }
                    return components;
                },
                pybind11::arg( "stratigraphic_unit" ),
                pybind11::return_value_policy::reference_internal,
                "Return the Blocks of the StratigraphicUnit" )
            .def( "native_extension", &StructuralModel::native_extension,
                "Return the extension of the native file format of the "
                "model (og_strm)" )
            .def( "structural_model_component", &StructuralModel::component,
                pybind11::arg( "id" ),
                pybind11::return_value_policy::reference_internal,
                "Return the component (mesh component, collection, fault, "
                "horizon, fault block or stratigraphic unit) with the given "
                "uuid; raise an error if the model has none" );
    }
} // namespace geode
