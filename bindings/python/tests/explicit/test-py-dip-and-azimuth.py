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


import math

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as geosciences

TOLERANCE = 1e-12


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def degrees(angle):
    return opengeode.Angle.create_from_degrees(angle)


def values(vector):
    return [vector.value(axis) for axis in range(3)]


def check_vector(vector, expected, message):
    check(type(vector) is opengeode.Vector3D, message + ": a Vector3D")
    check(
        all(abs(a - b) < TOLERANCE for a, b in zip(values(vector), expected)),
        message + ": get " + str(values(vector)) + " instead of " + str(expected),
    )


def test_dip_vector():
    horizontal = math.sqrt(3.0) / 2.0
    check_vector(
        geosciences.dip_vector(dip=degrees(30.0), dip_azimuth=degrees(90.0)),
        [horizontal, 0.0, -0.5],
        "Dip azimuth 90 dips toward the east",
    )
    check_vector(
        geosciences.dip_vector(degrees(30.0), degrees(180.0)),
        [0.0, -horizontal, -0.5],
        "Dip azimuth 180 dips toward the south",
    )
    check_vector(
        geosciences.dip_vector(degrees(90.0), degrees(17.0)),
        [0.0, 0.0, -1.0],
        "A vertical plane dips straight down",
    )


def test_strike_vector():
    check_vector(
        geosciences.strike_vector(dip_azimuth=degrees(90.0)),
        [0.0, 1.0, 0.0],
        "Dipping toward the east, the strike is north (right-hand rule)",
    )
    check_vector(
        geosciences.strike_vector(degrees(0.0)),
        [-1.0, 0.0, 0.0],
        "Dipping toward the north, the strike is west",
    )


def test_upward_plane_normal():
    dip = degrees(70.0)
    azimuth = degrees(45.0)
    normal = geosciences.upward_plane_normal(dip=dip, dip_azimuth=azimuth)
    down = geosciences.dip_vector(dip, azimuth)
    strike = geosciences.strike_vector(azimuth)
    check(normal.value(2) > 0, "The normal points upward")
    check(
        abs(normal.dot(down)) < TOLERANCE and abs(normal.dot(strike)) < TOLERANCE,
        "The normal is orthogonal to the dip and strike vectors",
    )
    check_vector(down.cross(strike), values(normal), "normal = dip x strike")
    check_vector(
        geosciences.upward_plane_normal(degrees(0.0), azimuth),
        [0.0, 0.0, 1.0],
        "A horizontal plane has a vertical normal",
    )


def test_wrong_arguments():
    for call in (
        lambda: geosciences.dip_vector(30.0, 90.0),
        lambda: geosciences.strike_vector(None),
        lambda: geosciences.upward_plane_normal(degrees(30.0)),
    ):
        try:
            call()
        except TypeError:
            continue
        raise ValueError("[Test] The functions take Angles: no TypeError")


if __name__ == "__main__":
    geosciences.OpenGeodeGeosciencesExplicitLibrary.initialize()
    test_dip_vector()
    test_strike_vector()
    test_upward_plane_normal()
    test_wrong_arguments()
