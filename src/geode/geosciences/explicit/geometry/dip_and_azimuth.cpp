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

#include <geode/geosciences/explicit/geometry/dip_and_azimuth.hpp>

#include <geode/geometry/angle.hpp>
#include <geode/geometry/vector.hpp>

namespace geode
{
    Vector3D dip_vector( const Angle& dip, const Angle& dip_azimuth )
    {
        return Vector3D{ { dip_azimuth.sin() * dip.cos(),
            dip_azimuth.cos() * dip.cos(), -dip.sin() } };
    }

    Vector3D strike_vector( const Angle& dip_azimuth )
    {
        return Vector3D{ { -dip_azimuth.cos(), dip_azimuth.sin(), 0 } };
    }

    Vector3D upward_plane_normal( const Angle& dip, const Angle& dip_azimuth )
    {
        return Vector3D{ { dip_azimuth.sin() * dip.sin(),
            dip_azimuth.cos() * dip.sin(), dip.cos() } };
    }
} // namespace geode
