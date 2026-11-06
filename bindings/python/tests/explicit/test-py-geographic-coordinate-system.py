# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as geosciences

if __name__ == "__main__":
    geosciences.OpenGeodeGeosciencesExplicitLibrary.initialize()

    infos = geosciences.GeographicCoordinateSystem3D.geographic_coordinate_systems()
    if len(infos) != 13181:
        raise ValueError("[Test] Wrong number of supported CRS")

    nb_points = 4
    manager = opengeode.AttributeManager()
    manager.resize(nb_points)

    lambert1 = geosciences.GeographicCoordinateSystem3D(
        manager, geosciences.GeographicCoordinateSystemInfo("EPSG", "27571", "I")
    )
    for p in range(nb_points):
        lambert1.set_point(p, opengeode.Point3D([p, p, p]))
    lambert2 = geosciences.GeographicCoordinateSystem3D(
        manager, geosciences.GeographicCoordinateSystemInfo("EPSG", "27572", "II")
    )
    lambert2.import_coordinates(lambert1)
    answers = [
        opengeode.Point3D([4273.64251995017, 1302920.55457198, 0]),
        opengeode.Point3D([4274.63159306906, 1302921.55102308, 1]),
        opengeode.Point3D([4275.62066620018, 1302922.54747419, 2]),
        opengeode.Point3D([4276.60973934352, 1302923.5439253, 3]),
    ]
    for p in range(nb_points):
        if not lambert2.point(p).inexact_equal(answers[p]):
            raise ValueError("[Test] Wrong coordinate conversion")

    wgs84 = geosciences.GeographicCoordinateSystemInfo.from_authority_code("4326")
    if wgs84.authority_code() != "EPSG:4326" or wgs84.name != "WGS 84":
        raise ValueError("[Test] Wrong info from authority code")

    grid = opengeode.LightRegularGrid2D(
        opengeode.Point2D([600000, 2400000]), [4, 3], [1000.0, 2000.0]
    )
    lambert1_info = geosciences.GeographicCoordinateSystemInfo("EPSG", "27571", "I")
    geosciences.assign_light_regular_grid_geographic_coordinate_system_info2D(
        grid, "lambert1", lambert1_info
    )
    lambert2_info = geosciences.GeographicCoordinateSystemInfo("EPSG", "27572", "II")
    geosciences.convert_light_regular_grid_coordinate_reference_system2D(
        grid, "lambert2", lambert2_info
    )
    crs_manager = grid.grid_coordinate_reference_system_manager()
    if crs_manager.active_coordinate_reference_system_name() != "lambert2":
        raise ValueError("[Test] Wrong active grid CRS")
    if crs_manager.active_coordinate_reference_system().info().code != "27572":
        raise ValueError("[Test] Wrong active grid CRS info")
