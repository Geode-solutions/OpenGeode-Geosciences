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

"""The long geosciences calls release the GIL: one sample per family (CRS,
CRS helpers, builder copy, fault blocks builder, load and save of each module,
implicit helpers).

Each check uses geode_test_env.assert_gil_released: with a huge interpreter
switch interval, a worker thread repeats the call until the main thread has
run. The repeated callable contains only the call under test: the objects it
needs are built before (a pool consumed with next() when the call can only be
made once per object)."""

import itertools
import os
import tempfile

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as explicit
import opengeode_geosciences_py_implicit as implicit
import opengeode_geosciences_py_simulation as simulation

DEADLINE = 60
# More empty models than the copies needed for the main thread to run
POOL_SIZE = 200
DATA = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../../tests/data")
)
LAMBERT1 = explicit.GeographicCoordinateSystemInfo("EPSG", "27571", "I")
LAMBERT2 = explicit.GeographicCoordinateSystemInfo("EPSG", "27572", "II")


def assert_gil_released(name, call):
    return geode_test_env.assert_gil_released(call, deadline=DEADLINE, name=name)


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def surface_model():
    model = explicit.StructuralModel()
    builder = explicit.StructuralModelBuilder(model)
    surface = model.surface(builder.add_surface())
    mesh_builder = builder.surface_mesh_builder(surface)
    for i in range(3):
        mesh_builder.create_point(
            opengeode.Point3D([600000.0 + 1000 * i, 2400000.0 + 1000 * i, 0.0])
        )
    mesh_builder.create_polygon([0, 1, 2])
    return model, builder


def load_vri2():
    path = os.path.join(DATA, "vri2.og_strm")
    return opengeode.load_one(explicit.load_structural_model(path), path)


def check_sanity():
    # A call holding the GIL must fail, else the checks below prove nothing
    try:
        geode_test_env.assert_gil_released(
            lambda: implicit.horizons_stack_from_top_to_bottom_names3D(
                ["h"], ["u1", "u2"]
            ),
            deadline=1.0,
            name="horizons_stack_from_top_to_bottom_names3D",
        )
    except ValueError as error:
        check("kept the GIL" in str(error), str(error))
        return
    raise ValueError("[Test] A call holding the GIL should be detected")


def check_crs():
    info = assert_gil_released(
        "GeographicCoordinateSystemInfo.from_authority_code",
        lambda: explicit.GeographicCoordinateSystemInfo.from_authority_code(
            "EPSG:27572"
        ),
    )
    check(info.code == "27572", "from_authority_code result")
    infos = assert_gil_released(
        "GeographicCoordinateSystem3D.geographic_coordinate_systems",
        explicit.GeographicCoordinateSystem3D.geographic_coordinate_systems,
    )
    check(len(infos) > 1000, "geographic_coordinate_systems result")


def check_crs_helpers():
    # Each call adds a new CRS to the same model: the names come from a
    # counter, so the calls can be repeated as long as needed
    model, builder = surface_model()
    names = ("crs" + str(index) for index in itertools.count())

    def assign():
        explicit.assign_brep_geographic_coordinate_system_info(
            model, builder, next(names), LAMBERT1
        )

    assert_gil_released("assign_brep_geographic_coordinate_system_info", assign)

    def convert():
        explicit.convert_brep_coordinate_reference_system(
            model, builder, next(names), LAMBERT2
        )

    assert_gil_released("convert_brep_coordinate_reference_system", convert)


def check_builder_copy():
    source = load_vri2()
    targets = []
    for _ in range(POOL_SIZE):
        target = explicit.StructuralModel()
        targets.append((target, explicit.StructuralModelBuilder(target)))
    to_copy = iter(targets)

    def copy():
        target, builder = next(to_copy)
        builder.copy(source)
        return target

    copied = assert_gil_released("StructuralModelBuilder.copy", copy)
    check(copied.nb_blocks() == source.nb_blocks(), "copied model")


def check_fault_blocks_builder():
    models = [load_vri2() for _ in range(POOL_SIZE)]
    to_build = iter(models)

    def build():
        model = next(to_build)
        explicit.build_structural_model_fault_blocks(model)
        return model

    built = assert_gil_released("build_structural_model_fault_blocks", build)
    check(built.nb_fault_blocks() == 1, "build_structural_model_fault_blocks result")


def check_load_save(directory):
    path = os.path.join(DATA, "vri2.og_strm")
    loaded = assert_gil_released(
        "load_structural_model", lambda: explicit.load_structural_model(path)
    )
    check(len(loaded) == 1, "load_structural_model result")
    model = loaded[0]
    output = os.path.join(directory, "model.og_strm")
    assert_gil_released(
        "save_structural_model", lambda: explicit.save_structural_model(model, output)
    )
    check(os.path.getsize(output) > 0, "save_structural_model output")
    stack_path = os.path.join(DATA, "test_HorizonStack_v0.og_hst3d")
    stacks = assert_gil_released(
        "load_horizons_stack3D", lambda: implicit.load_horizons_stack3D(stack_path)
    )
    check(stacks[0].nb_horizons() > 0, "load_horizons_stack3D result")
    problem = simulation.PhysicalProblem()
    problem_path = os.path.join(directory, "problem.og_phpb")
    assert_gil_released(
        "save_physical_problem",
        lambda: simulation.save_physical_problem(problem, problem_path),
    )
    problems = assert_gil_released(
        "load_physical_problem",
        lambda: simulation.load_physical_problem(problem_path),
    )
    check(problems[0].id().string() == problem.id().string(), "physical problem")


def check_implicit_helpers():
    model = implicit.ImplicitStructuralModel(load_vri2())
    block = model.blocks()[0]
    builder = implicit.ImplicitStructuralModelBuilder(model)
    builder.set_implicit_value(block, 0, 1.0)
    assert_gil_released(
        "rescale_model_implicit_value",
        lambda: implicit.rescale_model_implicit_value(model, 1.0),
    )
    check(
        model.implicit_value_from_vertex_id(block, 0) == 1.0,
        "rescale_model_implicit_value by 1 keeps the values",
    )


if __name__ == "__main__":
    explicit.OpenGeodeGeosciencesExplicitLibrary.initialize()
    implicit.OpenGeodeGeosciencesImplicitLibrary.initialize()
    simulation.OpenGeodeGeosciencesSimulationLibrary.initialize()
    check_sanity()
    check_crs()
    check_crs_helpers()
    check_builder_copy()
    check_fault_blocks_builder()
    with tempfile.TemporaryDirectory() as directory:
        check_load_save(directory)
    check_implicit_helpers()
