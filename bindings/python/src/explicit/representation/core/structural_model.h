/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/geometry/bounding_box.h>

#include <geode/geosciences/explicit/representation/core/structural_model.h>

namespace geode
{
    void define_structural_model( pybind11::module& module )
    {
        pybind11::class_< StructuralModel, BRep, Faults3D, Horizons3D,
            FaultBlocks3D, StratigraphicUnits3D >( module, "StructuralModel" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( BRep& cross_section ) {
                return StructuralModel{ std::move( cross_section ) };
            } ) )
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
                pybind11::return_value_policy::reference )
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
                pybind11::return_value_policy::reference )
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
                pybind11::return_value_policy::reference )
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
                pybind11::return_value_policy::reference )
            .def( "native_extension", &StructuralModel::native_extension );
    }
} // namespace geode
