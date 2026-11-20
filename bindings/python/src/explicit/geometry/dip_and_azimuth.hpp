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

#include <geode/geometry/angle.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/geosciences/explicit/geometry/dip_and_azimuth.hpp>

namespace geode
{
    void define_dip_and_azimuth( pybind11::module& module )
    {
        module.def( "dip_vector", &dip_vector, pybind11::arg( "dip" ),
            pybind11::arg( "dip_azimuth" ),
            "Return the unit Vector3D along the line of steepest descent of a "
            "plane of the given dip (Angle between the plane and the "
            "horizontal, from 0 to 90 degrees) and dip azimuth (Angle of the "
            "direction toward which the plane goes down, clockwise from the "
            "north +Y): (sin(dip_azimuth) cos(dip), cos(dip_azimuth) cos(dip), "
            "-sin(dip)). Moving a point of the plane by depth / sin(dip) along "
            "it lowers the point by depth and keeps it in the plane. Angles "
            "are not range checked" );
        module.def( "strike_vector", &strike_vector,
            pybind11::arg( "dip_azimuth" ),
            "Return the horizontal unit Vector3D along the strike of a plane "
            "of the given dip azimuth (Angle clockwise from the north +Y), "
            "following the right-hand rule: 90 degrees counterclockwise from "
            "the dip azimuth, so that the plane dips to its right: "
            "(-cos(dip_azimuth), sin(dip_azimuth), 0)" );
        module.def( "upward_plane_normal", &upward_plane_normal,
            pybind11::arg( "dip" ), pybind11::arg( "dip_azimuth" ),
            "Return the unit normal Vector3D of a plane of the given dip and "
            "dip azimuth (Angles, as in dip_vector), pointing upward and "
            "toward the dip azimuth (the hanging wall side of a fault): "
            "(sin(dip_azimuth) sin(dip), cos(dip_azimuth) sin(dip), "
            "cos(dip)), the cross product of the dip vector and the strike "
            "vector" );
    }
} // namespace geode
