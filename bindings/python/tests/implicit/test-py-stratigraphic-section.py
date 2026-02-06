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

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_geosciences_py_explicit as geode_exp
import opengeode_geosciences_py_implicit as geode_imp

def import_section_with_stratigraphy():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))
    cross_section = geode_exp.CrossSection(geode.load_section( data_dir + "/test_section.og_sctn"))
    stratigraphic_section = geode_imp.StratigraphicSection(cross_section)
    model_builder = geode_imp.StratigraphicSectionBuilder(stratigraphic_section)

    for surface in stratigraphic_section.surfaces():
        mesh = surface.mesh()
        scalar_attribute = mesh.vertex_attribute_manager().find_attribute_double( "curvature_min" )
        for vertex_id in range(mesh.nb_vertices()):
            model_builder.set_stratigraphic_coordinates( surface, vertex_id, geode_imp.StratigraphicPoint2D( [mesh.point( vertex_id ).value( 0 ), scalar_attribute.value( vertex_id )] ) )
    return stratigraphic_section

def test_section( stratigraphic_section ):
    surface0_id = geode.uuid( "00000000-2d28-4eeb-8000-000027dab659" )
    surface0 = stratigraphic_section.surface( surface0_id )
    strati_pt1 = stratigraphic_section.stratigraphic_coordinates_from_vertex_id( surface0, 1773 )
    if not strati_pt1.stratigraphic_coordinates().inexact_equal( geode.Point2D([ surface0.mesh().point( 1773 ).value( 0 ), -1.940964]) ):
        raise ValueError("[Test] Wrong stratigraphic coordinates for point 1773 at position [", surface0.mesh().point( 1773 ).string(), "] with stratigraphic coordinates [", strati_pt1.string(), "]." )
    query2 = surface0.mesh().point( 1832 )
    strati_pt2 = stratigraphic_section.stratigraphic_coordinates_from_geometric_point( surface0, query2 )
    if not strati_pt2.stratigraphic_coordinates().inexact_equal( stratigraphic_section.stratigraphic_coordinates_from_vertex_id( surface0, 1832 ).stratigraphic_coordinates() ):
        raise ValueError( "[Test] Wrong stratigraphic coordinates for point at position [", query2.string(), "] with stratigraphic coordinates [", strati_pt2.string(), "]." )
    query3 = geode.Point2D([3.32700324, 4.44508266])
    strati_pt3 = stratigraphic_section.stratigraphic_coordinates_from_geometric_point( surface0, query3 )
    if not strati_pt3.stratigraphic_coordinates().inexact_equal( geode.Point2D([ query3.value( 0 ), -1.889619 ]) ):
        raise ValueError( "[Test] Wrong stratigraphic coordinates for point at position [", query3.string(), "] with stratigraphic coordinates [", strati_pt3.string(), "]." )
    stratigraphic_bbox = stratigraphic_section.stratigraphic_bounding_box()
    if not stratigraphic_bbox.min().inexact_equal( geode.Point2D([ 0, -6.5798343 ]) ):
        raise ValueError( "[Test] Wrong stratigraphic coordinates bounding box minimum." )
    if not stratigraphic_bbox.max().inexact_equal( geode.Point2D([ 15, 10.4703907 ]) ):
        raise ValueError( "[Test] Wrong stratigraphic coordinates bounding box minimum." )

def test_save_stratigraphic_lines( stratigraphic_section ):
    counter = 0
    for model_surface in stratigraphic_section.surfaces():
        for line in stratigraphic_section.boundary_lines( model_surface ):
            geode.save_edged_curve2D( line.mesh(), "test_section_geom_line_" + str( counter ) + ".og_edc2d" )
            strati_line = stratigraphic_section.stratigraphic_line( model_surface, line )
            geode.save_edged_curve2D( strati_line[0], "test_section_strati_line_" + str( counter ) + ".og_edc2d" )
            counter += 1
    counter = 0
    for model_surface in stratigraphic_section.surfaces():
        for line in stratigraphic_section.internal_lines_of_surface( model_surface ):
            geode.save_edged_curve2D( line.mesh(), "test_section_geom_int_line_" + str(counter) + ".og_edc2d" )
            strati_lines = stratigraphic_section.stratigraphic_line( model_surface, line )
            geode.save_edged_curve2D( strati_lines[0], "test_section_strati_int_surface_" + str(counter) + "_front.og_edc2d" )
            geode.save_edged_curve2D( strati_lines[1], "test_section_strati_int_surface_" + str(counter) + "_back.og_edc2d" )
            counter+=1

if __name__ == '__main__':
    print("Starting test")
    geode_imp.GeosciencesImplicitLibrary.initialize()

    model = import_section_with_stratigraphy()
    test_section( model )
    test_save_stratigraphic_lines( model )
