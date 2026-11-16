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

"""stratigraphic_model_from_implicit_model_and_coords gives to each vertex a
stratigraphic location made of the two coordinates other than the implicit
axis."""

import os

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as explicit
import opengeode_geosciences_py_implicit as implicit

DATA = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../../../tests/data")
)
# The coordinates kept in the stratigraphic location for each implicit axis
LOCATION_AXES = {0: (1, 2), 1: (0, 2), 2: (0, 1)}


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def test_location_of_each_axis():
    path = DATA + "/vri2.og_strm"
    structural = opengeode.load_one(explicit.load_structural_model(path), path)
    implicit_model = implicit.ImplicitStructuralModel(structural)
    for axis, (first, second) in LOCATION_AXES.items():
        model = implicit.stratigraphic_model_from_implicit_model_and_coords(
            implicit_model=implicit_model, implicit_axis=axis
        )
        checked = 0
        for block in model.blocks():
            mesh = block.mesh()
            for vertex_id in range(mesh.nb_vertices()):
                point = mesh.point(vertex_id)
                location = model.stratigraphic_coordinates_from_vertex_id(
                    block, vertex_id
                ).stratigraphic_location()
                expected = opengeode.Point2D([point.value(first), point.value(second)])
                check(
                    location.inexact_equal(expected),
                    f"axis {axis}: the location of vertex {vertex_id} is "
                    f"{location.string()}, expected {expected.string()}",
                )
                checked += 1
        check(checked > 0, f"axis {axis}: no vertex checked")


if __name__ == "__main__":
    implicit.OpenGeodeGeosciencesImplicitLibrary.initialize()
    test_location_of_each_axis()
