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

#pragma once

#include <geode/geosciences/explicit/common.h>
#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( CoordinateReferenceSystemManager );
    class AttributeManager;
    class BRep;
    class BRepBuilder;
    class Section;
    class SectionBuilder;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    void convert_attribute_to_geographic_coordinate_reference_system(
        CoordinateReferenceSystemManager< dimension >& crs_manager,
        AttributeManager& attribute_manager,
        absl::string_view attribute_crs_name,
        absl::string_view geographic_crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info );

    void opengeode_geosciences_explicit_api
        convert_brep_coordinate_reference_system( const BRep& brep,
            BRepBuilder& builder,
            const GeographicCoordinateSystem3D::Info& info,
            absl::string_view crs_name );

    void opengeode_geosciences_explicit_api
        convert_section_coordinate_reference_system( const Section& section,
            SectionBuilder& builder,
            const GeographicCoordinateSystem2D::Info& info,
            absl::string_view crs_name );
} // namespace geode