# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2023 Geode-solutions
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

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_geosciences_py_explicit as geode_exp
import opengeode_geosciences_py_implicit as geode_imp

def test_model( model ):
    block = model.block(geode.uuid("00000000-c271-42e7-8000-00002c3147ed"))
    strati_pt1 = model.stratigraphic_coordinates_from_vertex_id(block, 59)
    if not strati_pt1.stratigraphic_coordinates().inexact_equal(geode.Point3D([-0.213112, -0.188148, 0.472047])):
        raise ValueError("[Test] Wrong stratigraphic coordinates for point 59 at position [", block.mesh().point(59).string(), "] with  stratigraphic coordinates [", strati_pt1.string(), "].")
    query2 = geode.Point3D([1, 0, 1])
    strati_pt2 = model.stratigraphic_coordinates_from_geometric_point(
        block, query2)
    if not strati_pt2.stratigraphic_coordinates().inexact_equal(geode.Point3D([0.386272, -0.109477, 0.])):
        raise ValueError("[Test] Wrong stratigraphic coordinates for point at position [", query2.string(), "] with  stratigraphic coordinates [", strati_pt2.string(), "].")
    query3 = geode.Point3D([0.480373621, 0.5420120955, 0.6765933633])
    strati_pt3 = model.stratigraphic_coordinates_from_geometric_point(
        block, query3)
    if not strati_pt3.stratigraphic_coordinates().inexact_equal(geode.Point3D([0.03380647978, -0.002759957825, 0.3080064376])):
        raise ValueError("[Test] Wrong stratigraphic coordinates for point at position [", query3.string(
        ), "] with  stratigraphic coordinates [", strati_pt3.string(), "].")

    geom_pt1 = model.geometric_coordinates_from_stratigraphic_point(
        block, strati_pt1)
    if not geom_pt1.inexact_equal(block.mesh().point(59)):
        raise ValueError("[Test] Wrong geometric coordinates for stratigraphic point at position [" +
                            strati_pt1.string() + "] with geometric coordinates [" + geom_pt1.string() + "].")
    geom_pt2 = model.geometric_coordinates_from_stratigraphic_point(
        block, strati_pt2)
    if not geom_pt2.inexact_equal(query2):
        raise ValueError("[Test] Wrong geometric coordinates for stratigraphic point at position [" + strati_pt2.string(
        ) + "] with geometric coordinates [" + geom_pt2.string() + "] instead of ["+query2.string()+"].")
    geom_pt3 = model.geometric_coordinates_from_stratigraphic_point(
        block, strati_pt3)
    if not geom_pt3.inexact_equal(query3):
        raise ValueError("[Test] Wrong geometric coordinates for stratigraphic point at position [" +
                            strati_pt3.string() + "] with geometric coordinates [" + geom_pt3.string() + "].")


def test_save_stratigraphic_surfaces( model ):
    counter = 0
    for model_block in model.blocks():
        for surface in model.boundary_surfaces(model_block):
            geode.save_triangulated_surface3D(
                surface.triangulated_mesh(), "mss_geom_surface_" + str(counter) + ".og_tsf3d")
            strati_surface = model.stratigraphic_surface(
                model_block, surface)
            geode.save_triangulated_surface3D(
                strati_surface[0], "mss_strati_surface_" + str(counter) + ".og_tsf3d")
            counter += 1
    counter = 0
    for model_block in model.blocks():
        for surface in model.internal_surfaces_of_block(model_block):
            geode.save_triangulated_surface3D(surface.triangulated_mesh(
            ), "mss_geom_int_surface_" + str(counter) + ".og_tsf3d")
            strati_surface = model.stratigraphic_surface(
                model_block, surface)
            geode.save_triangulated_surface3D(
                strati_surface[0], "mss_strati_int_surface_" + str(counter) + "_front.og_tsf3d")
            geode.save_triangulated_surface3D(
                strati_surface[1], "mss_strati_int_surface_" + str(counter) + "_back.og_tsf3d")
            counter += 1


if __name__ == '__main__':
    print("Starting test")
    geode_imp.GeosciencesImplicitLibrary.initialize()

    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    structural_model = geode_exp.load_structural_model( data_dir + "/vri2.og_strm")
    stratigraphic_model = geode_imp.StratigraphicModel(structural_model)

    test_model(stratigraphic_model)
    test_save_stratigraphic_surfaces(stratigraphic_model)
