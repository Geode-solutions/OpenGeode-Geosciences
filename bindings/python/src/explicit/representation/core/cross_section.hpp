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

#include <geode/geometry/bounding_box.hpp>

#include <geode/geosciences/explicit/representation/core/cross_section.hpp>

namespace geode
{
    void define_cross_section( pybind11::module& module )
    {
        pybind11::class_< CrossSection, Section, Faults2D, Horizons2D,
            FaultBlocks2D, StratigraphicUnits2D >( module, "CrossSection" )
            .def( pybind11::init<>() )
            .def( pybind11::init( []( Section& section ) {
                return CrossSection{ section.clone() };
            } ) )
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
                pybind11::return_value_policy::reference )
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
                pybind11::return_value_policy::reference )
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
                pybind11::return_value_policy::reference )
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
                pybind11::return_value_policy::reference )
            .def( "native_extension", &CrossSection::native_extension )
            .def( "cross_section_component", &CrossSection::component,
                pybind11::return_value_policy::reference );
    }
} // namespace geode
