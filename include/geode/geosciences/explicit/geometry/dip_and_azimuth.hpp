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

#include <geode/geosciences/explicit/common.hpp>

namespace geode
{
    class Angle;
    FORWARD_DECLARATION_DIMENSION_CLASS( Vector );
    ALIAS_3D( Vector );
} // namespace geode

/*
 * Orientation of a plane (a fault, a bedding) given by its dip and its dip
 * azimuth, as measured by geologists. The dip is the angle between the plane
 * and the horizontal, from 0 (horizontal) to 90 degrees (vertical). The dip
 * azimuth (dip direction) is the azimuth of the horizontal direction toward
 * which the plane goes down, clockwise from the north (+Y axis): 0 north, 90
 * east. Any angle is accepted (no range check): an azimuth is taken modulo
 * 360 degrees.
 */
namespace geode
{
    /*!
     * Unit vector along the line of steepest descent of the plane:
     * (sin(dip_azimuth) cos(dip), cos(dip_azimuth) cos(dip), -sin(dip)).
     * Moving a point of the plane by depth / sin(dip) along it lowers the
     * point by depth and keeps it in the plane.
     */
    [[nodiscard]] Vector3D opengeode_geosciences_explicit_api dip_vector(
        const Angle& dip, const Angle& dip_azimuth );

    /*!
     * Horizontal unit vector along the strike of the plane, following the
     * right-hand rule: 90 degrees counterclockwise from the dip azimuth, so
     * that the plane dips to the right of it:
     * (-cos(dip_azimuth), sin(dip_azimuth), 0).
     */
    [[nodiscard]] Vector3D opengeode_geosciences_explicit_api strike_vector(
        const Angle& dip_azimuth );

    /*!
     * Unit normal of the plane pointing upward, toward the dip azimuth (the
     * hanging wall side of a fault):
     * (sin(dip_azimuth) sin(dip), cos(dip_azimuth) sin(dip), cos(dip)).
     * It is the cross product of the dip vector and the strike vector.
     */
    [[nodiscard]] Vector3D opengeode_geosciences_explicit_api
        upward_plane_normal( const Angle& dip, const Angle& dip_azimuth );
} // namespace geode
