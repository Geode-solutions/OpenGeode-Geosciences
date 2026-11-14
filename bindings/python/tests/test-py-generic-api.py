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

"""The types of OpenGeode-Geosciences are registered in the generic functions
of opengeode (load, save, builder...): every type is saved and loaded back."""

import os
import tempfile

import geode_test_env

import opengeode
import opengeode_geosciences as geosciences

explicit = implicit = simulation = geosciences

DATA = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../../tests/data")
)


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    raise ValueError(f"[Test] {message} should raise {error.__name__}")


def structural_model():
    path = DATA + "/vri2.og_strm"
    return opengeode.load_one(explicit.load_structural_model(path), path)


def small_structural_model():
    model = explicit.StructuralModel()
    builder = explicit.StructuralModelBuilder(model)
    builder.set_name("small")
    fault = builder.add_fault(explicit.Fault3D.FaultType.normal)
    horizon = builder.add_horizon(explicit.Horizon3D.ContactType.erosion)
    surface = model.surface(builder.add_surface())
    builder.add_surface_in_fault(surface, model.fault(fault))
    builder.add_surface_in_horizon(surface, model.horizon(horizon))
    return model


def small_cross_section():
    section = explicit.CrossSection()
    builder = explicit.CrossSectionBuilder(section)
    builder.set_name("small")
    fault = builder.add_fault(explicit.Fault2D.FaultType.reverse)
    line = section.line(builder.add_line())
    builder.add_line_in_fault(line, section.fault(fault))
    builder.add_horizon()
    return section


def horizons_stack(dimension):
    return getattr(implicit, "horizons_stack_from_top_to_bottom_names" + dimension)(
        ["H2", "H1"], ["U3", "U2", "U1"]
    )


def physical_problem():
    problem = simulation.PhysicalProblem()
    builder = simulation.PhysicalProblemBuilder(problem)
    flow = simulation.SinglePhaseFlow()
    flow.set_fluid(
        simulation.SinglePhaseFluid(
            [simulation.Phase.water],
            [simulation.FluidComponent("water", 0.018)],
            1000.0,
            1e-3,
            1e-9,
            0.0,
            1e5,
        )
    )
    builder.set_flow(flow)
    builder.set_schedule(simulation.Schedule(0.0, 10.0, 1.0))
    builder.add_material(simulation.Material())
    return problem


def check_round_trip(directory, name, cls, obj, extension, builder_class, same):
    filename = os.path.join(directory, name + "." + extension)
    check(
        extension in opengeode.input_extensions(cls),
        f"{name}: {extension} is an input extension",
    )
    check(
        extension in opengeode.output_extensions(cls),
        f"{name}: {extension} is an output extension",
    )
    check(opengeode.is_saveable(cls, filename), f"{name}: is_saveable(class)")
    check(opengeode.is_saveable(obj, filename), f"{name}: is_saveable(object)")
    check(
        not opengeode.is_saveable(obj, os.path.join(directory, name + ".unknown")),
        f"{name}: unknown extension is not saveable",
    )
    written = opengeode.save(obj, filename)
    check(filename in written, f"{name}: save returns the written files {written}")
    check(os.path.exists(filename), f"{name}: the file is written")
    check(
        opengeode.is_loadable(cls, filename).value() > 0,
        f"{name}: the file is loadable",
    )
    check(
        opengeode.is_loadable(cls, os.path.join(directory, "x.unknown")).value() == 0,
        f"{name}: an unknown file is not loadable",
    )
    loaded = opengeode.load(cls, filename)
    check(isinstance(loaded, cls), f"{name}: load returns a {cls.__name__}")
    same(obj, loaded)
    all_loaded = opengeode.load_all(cls, filename)
    check(len(all_loaded) == 1, f"{name}: load_all")
    same(obj, all_loaded[0])
    files = opengeode.additional_files(cls, filename)
    check(files is not None, f"{name}: additional_files")
    check(opengeode.builder_type(cls) is builder_class, f"{name}: builder_type")
    check(
        type(opengeode.builder(loaded)) is builder_class,
        f"{name}: builder of an object",
    )
    # The builder modifies the object
    check_raises(
        lambda: opengeode.load(cls, os.path.join(directory, "missing." + extension)),
        opengeode.OpenGeodeDataError,
        f"{name}: load of a missing file",
    )
    check_raises(
        lambda: opengeode.save(obj, os.path.join(directory, name + ".unknown")),
        opengeode.OpenGeodeDataError,
        f"{name}: save with an unknown extension",
    )
    check(cls in opengeode.registered_types(), f"{name}: registered")


def same_structural(first, second):
    check(
        (first.nb_faults(), first.nb_horizons(), first.nb_surfaces(), first.nb_blocks())
        == (
            second.nb_faults(),
            second.nb_horizons(),
            second.nb_surfaces(),
            second.nb_blocks(),
        ),
        "the loaded StructuralModel is the saved one",
    )
    check(first.id().string() == second.id().string(), "same id")


def same_section(first, second):
    check(
        (first.nb_faults(), first.nb_horizons(), first.nb_lines(), first.nb_surfaces())
        == (
            second.nb_faults(),
            second.nb_horizons(),
            second.nb_lines(),
            second.nb_surfaces(),
        ),
        "the loaded CrossSection is the saved one",
    )


def same_stack(first, second):
    check(
        first.nb_horizons() == second.nb_horizons()
        and first.nb_stratigraphic_units() == second.nb_stratigraphic_units(),
        "the loaded HorizonsStack is the saved one",
    )


def same_problem(first, second):
    check(
        second.has_flow()
        and len(second.materials()) == 1
        and second.schedule().end_time == 10.0
        and second.id().string() == first.id().string(),
        "the loaded PhysicalProblem is the saved one",
    )


def test_round_trips(directory):
    check_round_trip(
        directory,
        "structural_model",
        explicit.StructuralModel,
        small_structural_model(),
        "og_strm",
        explicit.StructuralModelBuilder,
        same_structural,
    )
    check_round_trip(
        directory,
        "cross_section",
        explicit.CrossSection,
        small_cross_section(),
        "og_xsctn",
        explicit.CrossSectionBuilder,
        same_section,
    )
    implicit_model = implicit.ImplicitStructuralModel(structural_model())
    check_round_trip(
        directory,
        "implicit_structural_model",
        implicit.ImplicitStructuralModel,
        implicit_model,
        "og_istrm",
        implicit.ImplicitStructuralModelBuilder,
        same_structural,
    )
    implicit_section = implicit.ImplicitCrossSection(small_cross_section())
    check_round_trip(
        directory,
        "implicit_cross_section",
        implicit.ImplicitCrossSection,
        implicit_section,
        "og_ixsctn",
        implicit.ImplicitCrossSectionBuilder,
        same_section,
    )
    for dimension in ["2D", "3D"]:
        check_round_trip(
            directory,
            "stack" + dimension,
            getattr(implicit, "HorizonsStack" + dimension),
            horizons_stack(dimension),
            "og_hst" + dimension.lower(),
            getattr(implicit, "HorizonsStackBuilder" + dimension),
            same_stack,
        )
    check_round_trip(
        directory,
        "problem",
        simulation.PhysicalProblem,
        physical_problem(),
        "og_phpb",
        simulation.PhysicalProblemBuilder,
        same_problem,
    )


def test_derived_types_use_their_own_format(directory):
    # An ImplicitStructuralModel is saved by its own writer, not by the one of
    # the StructuralModel or of the BRep it derives from
    model = implicit.ImplicitStructuralModel(small_structural_model())
    check_raises(
        lambda: opengeode.save(model, os.path.join(directory, "x.og_strm")),
        opengeode.OpenGeodeDataError,
        "save of an ImplicitStructuralModel with the StructuralModel format",
    )
    check(
        opengeode.output_extensions(implicit.ImplicitStructuralModel) == ["og_istrm"],
        "output extensions of the implicit model",
    )
    # A StructuralModel is a BRep for the generic functions of OpenGeode
    brep_builder = opengeode.builder(opengeode.BRep())
    check(type(brep_builder) is opengeode.BRepBuilder, "BRep builder")
    check(
        type(opengeode.builder(small_structural_model()))
        is explicit.StructuralModelBuilder,
        "StructuralModel builder",
    )
    check(
        type(opengeode.builder(small_cross_section())) is explicit.CrossSectionBuilder,
        "CrossSection builder",
    )


def test_stratigraphic_types_have_no_native_format(directory):
    # No file format is registered for them (see OpenGeode-Geosciences)
    for cls, builder_class, obj in [
        (
            implicit.StratigraphicModel,
            implicit.StratigraphicModelBuilder,
            implicit.StratigraphicModel(),
        ),
        (
            implicit.StratigraphicSection,
            implicit.StratigraphicSectionBuilder,
            implicit.StratigraphicSection(),
        ),
    ]:
        check(cls in opengeode.registered_types(), f"{cls.__name__} is registered")
        check(opengeode.builder_type(cls) is builder_class, f"{cls.__name__} builder")
        check(
            type(opengeode.builder(obj)) is builder_class,
            f"{cls.__name__} builder of an object",
        )
        check(opengeode.input_extensions(cls) == [], f"{cls.__name__} input extensions")
        filename = os.path.join(directory, "x." + obj.native_extension())
        check(not opengeode.is_saveable(obj, filename), f"{cls.__name__} saveable")
        check_raises(
            lambda: opengeode.save(obj, filename),
            opengeode.OpenGeodeDataError,
            f"save of a {cls.__name__}",
        )
        check_raises(
            lambda: opengeode.load(cls, filename),
            opengeode.OpenGeodeDataError,
            f"load of a {cls.__name__}",
        )


def test_registration_errors():
    check_raises(
        lambda: opengeode.register_type(
            explicit.StructuralModel, load=explicit.load_structural_model
        ),
        ValueError,
        "register_type of a type which is already registered",
    )
    check_raises(
        lambda: opengeode.load(explicit.Fault3D, "x.og_strm"),
        TypeError,
        "load of a type which is not registered",
    )


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as directory:
        test_round_trips(directory)
        test_derived_types_use_their_own_format(directory)
        test_stratigraphic_types_have_no_native_format(directory)
    test_registration_errors()
