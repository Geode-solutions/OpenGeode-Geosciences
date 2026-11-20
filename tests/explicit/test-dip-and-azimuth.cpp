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

#include <cmath>
#include <string>

#include <absl/strings/str_cat.h>

#include <geode/basic/logger.hpp>

#include <geode/geometry/angle.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/geosciences/explicit/common.hpp>
#include <geode/geosciences/explicit/geometry/dip_and_azimuth.hpp>

// NOLINTBEGIN(*-magic-numbers)
namespace
{
    constexpr double TOLERANCE = 1e-12;

    geode::Angle degrees( double angle )
    {
        return geode::Angle::create_from_degrees( angle );
    }

    void check_vector( const geode::Vector3D& vector,
        const geode::Vector3D& expected,
        const std::string& message )
    {
        geode::OpenGeodeGeosciencesExplicitException::test(
            ( vector - expected ).length() < TOLERANCE, message, ": get ",
            vector.string(), " instead of ", expected.string() );
    }

    void test_horizontal_and_vertical_planes()
    {
        check_vector( geode::dip_vector( degrees( 0 ), degrees( 90 ) ),
            geode::Vector3D{ { 1, 0, 0 } },
            "A horizontal plane has a horizontal dip vector" );
        check_vector( geode::upward_plane_normal( degrees( 0 ), degrees( 90 ) ),
            geode::Vector3D{ { 0, 0, 1 } },
            "A horizontal plane has a vertical normal" );
        check_vector( geode::dip_vector( degrees( 90 ), degrees( 30 ) ),
            geode::Vector3D{ { 0, 0, -1 } },
            "A vertical plane dips straight down" );
        check_vector(
            geode::upward_plane_normal( degrees( 90 ), degrees( 90 ) ),
            geode::Vector3D{ { 1, 0, 0 } },
            "The normal of a vertical plane points toward its dip azimuth" );
    }

    void test_dip_azimuth_directions()
    {
        const auto horizontal = std::sqrt( 3. ) / 2.;
        check_vector( geode::dip_vector( degrees( 30 ), degrees( 0 ) ),
            geode::Vector3D{ { 0, horizontal, -0.5 } },
            "Dip azimuth 0 dips toward the north (+Y)" );
        check_vector( geode::dip_vector( degrees( 30 ), degrees( 90 ) ),
            geode::Vector3D{ { horizontal, 0, -0.5 } },
            "Dip azimuth 90 dips toward the east (+X)" );
        check_vector( geode::dip_vector( degrees( 30 ), degrees( 180 ) ),
            geode::Vector3D{ { 0, -horizontal, -0.5 } },
            "Dip azimuth 180 dips toward the south (-Y)" );
        check_vector( geode::dip_vector( degrees( 30 ), degrees( 270 ) ),
            geode::Vector3D{ { -horizontal, 0, -0.5 } },
            "Dip azimuth 270 dips toward the west (-X)" );
        const auto component = horizontal / std::sqrt( 2. );
        check_vector( geode::dip_vector( degrees( 30 ), degrees( 45 ) ),
            geode::Vector3D{ { component, component, -0.5 } },
            "Dip azimuth 45 dips toward the north-east" );
        check_vector( geode::dip_vector( degrees( 30 ), degrees( -90 ) ),
            geode::dip_vector( degrees( 30 ), degrees( 270 ) ),
            "A dip azimuth is taken modulo 360" );
        check_vector( geode::dip_vector( degrees( 30 ), degrees( 450 ) ),
            geode::dip_vector( degrees( 30 ), degrees( 90 ) ),
            "A dip azimuth is taken modulo 360" );
    }

    void test_strike_follows_the_right_hand_rule()
    {
        check_vector( geode::strike_vector( degrees( 90 ) ),
            geode::Vector3D{ { 0, 1, 0 } },
            "Dipping toward the east, the strike is north" );
        check_vector( geode::strike_vector( degrees( 0 ) ),
            geode::Vector3D{ { -1, 0, 0 } },
            "Dipping toward the north, the strike is west" );
        check_vector( geode::strike_vector( degrees( 180 ) ),
            geode::Vector3D{ { 1, 0, 0 } },
            "Dipping toward the south, the strike is east" );
        check_vector( geode::strike_vector( degrees( 270 ) ),
            geode::Vector3D{ { 0, -1, 0 } },
            "Dipping toward the west, the strike is south" );
    }

    void test_orthonormal_frame()
    {
        for( const auto dip : { 0., 10., 45., 70., 89., 90. } )
        {
            for( const auto azimuth : { 0., 30., 135., 200., 315., 359. } )
            {
                const auto name = absl::StrCat(
                    "Dip ", dip, ", dip azimuth ", azimuth, ": " );
                const auto down =
                    geode::dip_vector( degrees( dip ), degrees( azimuth ) );
                const auto strike = geode::strike_vector( degrees( azimuth ) );
                const auto normal = geode::upward_plane_normal(
                    degrees( dip ), degrees( azimuth ) );
                for( const auto& vector : { down, strike, normal } )
                {
                    geode::OpenGeodeGeosciencesExplicitException::test(
                        std::fabs( vector.length() - 1 ) < TOLERANCE, name,
                        "the vectors should be unit vectors" );
                }
                geode::OpenGeodeGeosciencesExplicitException::test(
                    std::fabs( down.dot( strike ) ) < TOLERANCE
                        && std::fabs( down.dot( normal ) ) < TOLERANCE
                        && std::fabs( strike.dot( normal ) ) < TOLERANCE,
                    name, "the vectors should be orthogonal" );
                check_vector( down.cross( strike ), normal,
                    absl::StrCat( name, "normal = dip x strike" ) );
                geode::OpenGeodeGeosciencesExplicitException::test(
                    std::fabs( std::asin( -down.value( 2 ) )
                               - degrees( dip ).radians() )
                        < TOLERANCE,
                    name, "the dip vector should plunge by the dip" );
                geode::OpenGeodeGeosciencesExplicitException::test(
                    normal.value( 2 ) >= 0, name,
                    "the normal should point upward" );
            }
        }
    }

    void test_point_moved_along_the_dip_stays_in_the_plane()
    {
        const auto dip = degrees( 60 );
        const auto azimuth = degrees( 135 );
        const auto depth = 100.;
        const auto offset =
            geode::dip_vector( dip, azimuth ) * ( depth / dip.sin() );
        check_vector( offset,
            geode::Vector3D{ { depth / dip.tan() / std::sqrt( 2. ),
                -depth / dip.tan() / std::sqrt( 2. ), -depth } },
            "Lowered by the depth, moved by depth / tan(dip) toward the dip "
            "azimuth" );
        geode::OpenGeodeGeosciencesExplicitException::test(
            std::fabs(
                offset.dot( geode::upward_plane_normal( dip, azimuth ) ) )
                < TOLERANCE,
            "The moved point stays in the plane" );
    }
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesExplicitLibrary::initialize();
        test_horizontal_and_vertical_planes();
        test_dip_azimuth_directions();
        test_strike_follows_the_right_hand_rule();
        test_orthonormal_frame();
        test_point_moved_along_the_dip_stays_in_the_plane();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
// NOLINTEND(*-magic-numbers)
