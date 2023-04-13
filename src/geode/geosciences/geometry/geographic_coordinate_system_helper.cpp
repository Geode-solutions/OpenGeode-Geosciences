/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/geosciences/geometry/geographic_coordinate_system_helper.h>

#include <geode/basic/attribute_manager.h>

#include <geode/mesh/builder/coordinate_reference_system_manager_builder.h>
#include <geode/mesh/core/attribute_coordinate_reference_system.h>
#include <geode/mesh/core/coordinate_reference_system_manager.h>

namespace geode
{
    template < index_t dimension >
    void convert_attribute_to_geographic_coordinate_reference_system(
        CoordinateReferenceSystemManager< dimension >& crs_manager,
        AttributeManager& attribute_manager,
        absl::string_view attribute_crs_name,
        absl::string_view geographic_crs_name,
        typename GeographicCoordinateSystem< dimension >::Info info )
    {
        const auto is_active =
            crs_manager.active_coordinate_reference_system_name()
            == attribute_crs_name;
        const auto& attribute_crs = dynamic_cast<
            const AttributeCoordinateReferenceSystem< dimension >& >(
            crs_manager.find_coordinate_reference_system(
                attribute_crs_name ) );
        attribute_manager.rename_attribute(
            attribute_crs.attribute_name(), info.name );
        CoordinateReferenceSystemManagerBuilder< dimension > crs_builder{
            crs_manager
        };
        crs_builder.register_coordinate_reference_system( geographic_crs_name,
            std::make_shared< GeographicCoordinateSystem< dimension > >(
                attribute_manager, std::move( info ) ) );
        crs_builder.delete_coordinate_reference_system( attribute_crs_name );
        if( is_active )
        {
            crs_builder.set_active_coordinate_reference_system(
                geographic_crs_name );
        }
    }

    template void opengeode_geosciences_geosciences_api
        convert_attribute_to_geographic_coordinate_reference_system(
            CoordinateReferenceSystemManager< 2 >&,
            AttributeManager&,
            absl::string_view,
            absl::string_view,
            typename GeographicCoordinateSystem< 2 >::Info );
    template void opengeode_geosciences_geosciences_api
        convert_attribute_to_geographic_coordinate_reference_system(
            CoordinateReferenceSystemManager< 3 >&,
            AttributeManager&,
            absl::string_view,
            absl::string_view,
            typename GeographicCoordinateSystem< 3 >::Info );
} // namespace geode