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

#include <geode/basic/assert.h>
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>

#include <geode/geometry/point.h>

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.h>

int main()
{
    try
    {
        geode::GeosciencesExplicitLibrary::initialize();

        const auto infos = geode::GeographicCoordinateSystem3D::
            geographic_coordinate_systems();
        OPENGEODE_EXCEPTION(
            infos.size() == 13181, "[Test] Wrong number of supported CRS" );

        constexpr geode::index_t nb_points{ 4 };
        geode::AttributeManager manager;
        manager.resize( nb_points );

        geode::GeographicCoordinateSystem3D lambert1{ manager,
            { "EPSG", "27571", "I" } };
        for( const auto p : geode::TRange< double >{ nb_points } )
        {
            lambert1.set_point( p, { { p, p, p } } );
        }
        geode::GeographicCoordinateSystem3D lambert2{ manager,
            { "EPSG", "27572", "II" } };
        lambert2.import_coordinates( lambert1 );
        std::array< geode::Point3D, nb_points > answers{
            geode::Point3D{ { 4273.64251995017, 1302920.55457198, 0 } },
            geode::Point3D{ { 4274.63159306906, 1302921.55102308, 1 } },
            geode::Point3D{ { 4275.62066620018, 1302922.54747419, 2 } },
            geode::Point3D{ { 4276.60973934352, 1302923.5439253, 3 } },
        };
        for( const auto p : geode::Range{ nb_points } )
        {
            OPENGEODE_EXCEPTION(
                lambert2.point( p ).inexact_equal( answers[p] ),
                "[Test] Wrong coordinate conversion" );
        }

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
