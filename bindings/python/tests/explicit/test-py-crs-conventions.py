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

"""Conventions of the geographic coordinate systems and of the helpers
converting them: static methods, hash and repr of the infos, checked
arguments and keyword arguments."""

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as geosciences

WGS84 = ("EPSG", "4326", "WGS 84")
LAMBERT1 = ("EPSG", "27571", "I")
LAMBERT2 = ("EPSG", "27572", "II")


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    raise ValueError(f"[Test] {message} should raise {error.__name__}")


def info(values):
    return geosciences.GeographicCoordinateSystemInfo(*values)


def test_info_value_semantics():
    first = geosciences.GeographicCoordinateSystemInfo(
        authority="EPSG", code="4326", name="WGS 84"
    )
    second = info(WGS84)
    check(first == second, "Infos with the same fields should be equal")
    check(hash(first) == hash(second), "Equal infos should have the same hash")
    check(first != info(LAMBERT1), "Different infos should be different")
    check(len({first, second, info(LAMBERT1)}) == 2, "Infos should be hashable")
    check(
        repr(first)
        == "GeographicCoordinateSystemInfo('EPSG', '4326', 'WGS 84')",
        f"Wrong repr {first!r}",
    )
    check(eval(repr(first), vars(geosciences)) == first, "repr should be evaluable")
    check(str(first) == first.string() == "(EPSG:4326 -> WGS 84)", str(first))
    check(first.authority_code() == "EPSG:4326", "authority_code")
    first.name = "other"
    check(first.name == "other" and first != second, "Fields are writable")
    check(
        geosciences.GeographicCoordinateSystemInfo().authority_code() == ":",
        "default info",
    )
    parsed = geosciences.GeographicCoordinateSystemInfo.from_authority_code(
        authority_code="EPSG:27572"
    )
    check(parsed.code == "27572" and parsed.authority == "EPSG", "from_authority_code")
    check_raises(
        lambda: geosciences.GeographicCoordinateSystemInfo.from_authority_code("EPSG:0"),
        opengeode.OpenGeodeDataError,
        "from_authority_code of an unknown code",
    )


def test_static_methods():
    for dimension in (2, 3):
        crs_class = getattr(geosciences, f"GeographicCoordinateSystem{dimension}D")
        grid_class = getattr(geosciences, f"GeographicGridCoordinateSystem{dimension}D")
        check(
            crs_class.type_name_static().get() == "GeographicCoordinateSystem",
            "type_name_static",
        )
        check(
            grid_class.type_name_static().get() == "GeographicGridCoordinateSystem",
            "type_name_static of the grid system",
        )
        check(
            len(crs_class.geographic_coordinate_systems()) > 1000,
            "geographic_coordinate_systems",
        )


def test_system_keyword_arguments():
    manager = opengeode.AttributeManager()
    manager.resize(2)
    crs = geosciences.GeographicCoordinateSystem3D(
        manager=manager, info=info(LAMBERT1)
    )
    check(crs.info() == info(LAMBERT1), "info of the system")
    crs.set_point(0, opengeode.Point3D([1, 2, 3]))
    other = geosciences.GeographicCoordinateSystem3D(manager, info(LAMBERT2))
    other.import_coordinates(crs=crs)
    check(other.point(0).inexact_equal(other.point(0)), "converted point")
    grid_crs = geosciences.GeographicGridCoordinateSystem2D(
        coordinate_system=opengeode.CoordinateSystem2D(), info=info(WGS84)
    )
    check(grid_crs.info() == info(WGS84), "info of the grid system")


def test_system_keeps_manager_alive():
    import gc
    import weakref

    manager = opengeode.AttributeManager()
    reference = weakref.ref(manager)
    crs = geosciences.GeographicCoordinateSystem2D(manager, info(WGS84))
    del manager
    gc.collect()
    check(reference() is not None, "The system should keep its manager alive")
    del crs
    gc.collect()
    check(reference() is None, "The manager should be released with the system")


def test_import_coordinates_is_checked():
    small_manager = opengeode.AttributeManager()
    small_manager.resize(1)
    large_manager = opengeode.AttributeManager()
    large_manager.resize(3)
    small = geosciences.GeographicCoordinateSystem2D(small_manager, info(LAMBERT1))
    large = geosciences.GeographicCoordinateSystem2D(large_manager, info(LAMBERT2))
    check_raises(
        lambda: small.import_coordinates(large),
        IndexError,
        "import_coordinates of a larger system",
    )
    # A larger system can import the points of a smaller one
    large.import_coordinates(small)


def surface_mesh(dimension, with_points=True):
    mesh = (
        opengeode.TriangulatedSurface3D.create()
        if dimension == 3
        else opengeode.TriangulatedSurface2D.create()
    )
    builder = opengeode.TriangulatedSurfaceBuilder3D.create(mesh) if dimension == 3 else opengeode.TriangulatedSurfaceBuilder2D.create(mesh)
    if with_points:
        for i in range(3):
            coordinates = [600000.0 + 1000 * i, 2400000.0 + 1000 * i, 0.0][:dimension]
            builder.create_point(
                opengeode.Point3D(coordinates)
                if dimension == 3
                else opengeode.Point2D(coordinates)
            )
        builder.create_triangle([0, 1, 2])
    return mesh, builder


def test_mesh_helpers_keyword_arguments():
    for dimension in (2, 3):
        mesh, builder = surface_mesh(dimension)
        assign = getattr(
            geosciences,
            f"assign_surface_mesh_geographic_coordinate_system_info{dimension}D",
        )
        convert = getattr(
            geosciences,
            f"convert_surface_mesh_coordinate_reference_system{dimension}D",
        )
        # The mesh has a plain CRS stored in an attribute
        assign(mesh=mesh, builder=builder, crs_name="lambert1", info=info(LAMBERT1))
        manager = mesh.main_coordinate_reference_system_manager()
        check(
            manager.active_coordinate_reference_system_name() == "lambert1",
            "assign activates the system",
        )
        check(
            manager.active_coordinate_reference_system().info() == info(LAMBERT1),
            "assign gives the info",
        )
        convert(mesh=mesh, builder=builder, crs_name="lambert2", info=info(LAMBERT2))
        check(
            manager.active_coordinate_reference_system_name() == "lambert2",
            "convert activates the system",
        )
        check(
            manager.active_coordinate_reference_system().info() == info(LAMBERT2),
            "convert gives the info",
        )
        check_raises(
            lambda: convert(mesh, builder, "lambert2", info(LAMBERT2)),
            opengeode.OpenGeodeDataError,
            "convert with an existing CRS name",
        )


def test_other_meshes_helpers_exist():
    for name in [
        "edged_curve",
        "point_set",
        "surface_mesh",
    ]:
        for dimension in (2, 3):
            check(
                hasattr(
                    geosciences,
                    f"assign_{name}_geographic_coordinate_system_info{dimension}D",
                ),
                f"assign {name} {dimension}D",
            )
            check(
                hasattr(
                    geosciences, f"convert_{name}_coordinate_reference_system{dimension}D"
                ),
                f"convert {name} {dimension}D",
            )
    for helper in [
        "assign_solid_mesh_geographic_coordinate_system_info3D",
        "convert_solid_mesh_coordinate_reference_system3D",
    ]:
        check(hasattr(geosciences, helper), helper)


def test_grid_helpers_keyword_arguments():
    for dimension in (2, 3):
        grid = (
            opengeode.LightRegularGrid2D(
                opengeode.Point2D([600000, 2400000]), [4, 3], [1000.0, 2000.0]
            )
            if dimension == 2
            else opengeode.LightRegularGrid3D(
                opengeode.Point3D([600000, 2400000, 0]), [4, 3, 2], [1000.0, 2000.0, 10.0]
            )
        )
        assign = getattr(
            geosciences,
            f"assign_light_regular_grid_geographic_coordinate_system_info{dimension}D",
        )
        convert = getattr(
            geosciences,
            f"convert_light_regular_grid_coordinate_reference_system{dimension}D",
        )
        assign(grid=grid, crs_name="lambert1", info=info(LAMBERT1))
        convert(grid=grid, crs_name="lambert2", info=info(LAMBERT2))
        manager = grid.grid_coordinate_reference_system_manager()
        check(
            manager.active_coordinate_reference_system_name() == "lambert2",
            "grid conversion",
        )
        check_raises(
            lambda: convert(grid, "lambert2", info(LAMBERT2)),
            opengeode.OpenGeodeDataError,
            "convert a grid with an existing CRS name",
        )


def build_structural_model():
    model = geosciences.StructuralModel()
    builder = geosciences.StructuralModelBuilder(model)
    surface = model.surface(builder.add_surface())
    mesh_builder = builder.surface_mesh_builder(surface)
    for i in range(3):
        mesh_builder.create_point(
            opengeode.Point3D([600000.0 + 1000 * i, 2400000.0 + 1000 * i, 0.0])
        )
    mesh_builder.create_polygon([0, 1, 2])
    return model, builder, surface


def test_model_helpers():
    model, builder, surface = build_structural_model()
    geosciences.assign_brep_geographic_coordinate_system_info(
        brep=model, builder=builder, crs_name="lambert1", info=info(LAMBERT1)
    )
    geosciences.convert_brep_coordinate_reference_system(
        brep=model, builder=builder, crs_name="lambert2", info=info(LAMBERT2)
    )
    manager = surface.mesh().main_coordinate_reference_system_manager()
    check(
        manager.active_coordinate_reference_system().info() == info(LAMBERT2),
        "convert_brep_coordinate_reference_system",
    )
    check_raises(
        lambda: geosciences.convert_brep_coordinate_reference_system(
            model, builder, "lambert2", info(WGS84)
        ),
        opengeode.OpenGeodeDataError,
        "convert a model with an existing CRS name",
    )

    section = geosciences.CrossSection()
    section_builder = geosciences.CrossSectionBuilder(section)
    line = section.line(section_builder.add_line())
    line_builder = section_builder.line_mesh_builder(line)
    for i in range(2):
        line_builder.create_point(
            opengeode.Point2D([600000.0 + 1000 * i, 2400000.0 + 1000 * i])
        )
    geosciences.assign_section_geographic_coordinate_system_info(
        section=section, builder=section_builder, crs_name="lambert1", info=info(LAMBERT1)
    )
    geosciences.convert_section_coordinate_reference_system(
        section=section, builder=section_builder, crs_name="lambert2", info=info(LAMBERT2)
    )
    line_manager = line.mesh().main_coordinate_reference_system_manager()
    check(
        line_manager.active_coordinate_reference_system().info() == info(LAMBERT2),
        "convert_section_coordinate_reference_system",
    )


if __name__ == "__main__":
    geosciences.OpenGeodeGeosciencesExplicitLibrary.initialize()
    test_info_value_semantics()
    test_static_methods()
    test_system_keyword_arguments()
    test_system_keeps_manager_alive()
    test_import_coordinates_is_checked()
    test_mesh_helpers_keyword_arguments()
    test_other_meshes_helpers_exist()
    test_grid_helpers_keyword_arguments()
    test_model_helpers()
