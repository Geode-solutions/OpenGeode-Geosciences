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

#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.h>

#include <geode/geometry/point.h>

namespace geode
{
    void define_implicit_cross_section_builder( pybind11::module& module )
    {
        pybind11::class_< ImplicitCrossSectionBuilder, CrossSectionBuilder >(
            module, "ImplicitCrossSectionBuilder" )
            .def( pybind11::init< ImplicitCrossSection& >() )
            .def( "copy", ( void( ImplicitCrossSectionBuilder::* )(
                              const ImplicitCrossSection& implicit_section ) )
                              & ImplicitCrossSectionBuilder::copy )
            .def( "instantiate_implicit_attribute_on_surfaces",
                &ImplicitCrossSectionBuilder::
                    instantiate_implicit_attribute_on_surfaces )
            .def( "set_implicit_value",
                &ImplicitCrossSectionBuilder::set_implicit_value )
            .def( "set_stratigraphic_units_stack",
                []( ImplicitCrossSectionBuilder& builder,
                    StratigraphicUnitsStack2D& su_stack ) {
                    builder.set_stratigraphic_units_stack(
                        std::move( su_stack ) );
                } )
            .def( "set_horizon_isovalue",
                &ImplicitCrossSectionBuilder::set_horizon_isovalue );
    }
} // namespace geode