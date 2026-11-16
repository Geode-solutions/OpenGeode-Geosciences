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

"""Lifetime, copy and unique vertex checks of the builders of the models."""

import gc
import weakref

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as geosciences


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    raise ValueError(f"[Test] {message} should raise {error.__name__}")


def fill(mesh_builder, nb_vertices, dimension):
    for i in range(nb_vertices):
        coordinates = [float(i)] + [0.0] * (dimension - 1)
        mesh_builder.create_point(
            opengeode.Point3D(coordinates)
            if dimension == 3
            else opengeode.Point2D(coordinates)
        )


def build_structural_model():
    model = geosciences.StructuralModel()
    builder = geosciences.StructuralModelBuilder(model)
    surface = model.surface(builder.add_surface())
    block = model.block(builder.add_block())
    fill(builder.surface_mesh_builder(surface), 4, 3)
    fill(builder.block_mesh_builder(block), 5, 3)
    builder.create_unique_vertices(2)
    return model, builder, [surface, block]


def build_cross_section():
    section = geosciences.CrossSection()
    builder = geosciences.CrossSectionBuilder(section)
    line = section.line(builder.add_line())
    surface = section.surface(builder.add_surface())
    fill(builder.line_mesh_builder(line), 3, 2)
    fill(builder.surface_mesh_builder(surface), 4, 2)
    builder.create_unique_vertices(2)
    return section, builder, [line, surface]


def test_builders_keep_models_alive():
    for model_class, builder_class in [
        (geosciences.StructuralModel, geosciences.StructuralModelBuilder),
        (geosciences.CrossSection, geosciences.CrossSectionBuilder),
    ]:
        model = model_class()
        reference = weakref.ref(model)
        builder = builder_class(model)
        del model
        gc.collect()
        check(
            reference() is not None,
            f"{builder_class.__name__} should keep its model alive",
        )
        builder.add_fault()
        check(reference().nb_faults() == 1, "The builder should modify the model")
        del builder
        gc.collect()
        check(
            reference() is None,
            f"{model_class.__name__} should be released with its builder",
        )


def test_builders_remember_their_model():
    model = geosciences.StructuralModel()
    builder = geosciences.StructuralModelBuilder(model)
    check(builder._model is model, "StructuralModelBuilder should remember its model")
    section = geosciences.CrossSection()
    section_builder = geosciences.CrossSectionBuilder(section)
    check(
        section_builder._model is section,
        "CrossSectionBuilder should remember its model",
    )


def vertex(component, index):
    return opengeode.ComponentMeshVertex(component.component_id(), index)


def check_unique_vertex_checks(name, model, builder, components, nb_vertices):
    # Valid calls work for every component
    for component, size in zip(components, nb_vertices):
        builder.set_unique_vertex(vertex(component, size - 1), 1)
    for component, size in zip(components, nb_vertices):
        # The vertex of the component must exist in its mesh
        check_raises(
            lambda: builder.set_unique_vertex(vertex(component, size), 0),
            IndexError,
            f"{name}: set_unique_vertex with an unknown component vertex",
        )
        # The unique vertex must exist
        check_raises(
            lambda: builder.set_unique_vertex(vertex(component, 0), 99),
            IndexError,
            f"{name}: set_unique_vertex with an unknown unique vertex",
        )


def test_unique_vertex_checks():
    model, builder, components = build_structural_model()
    check_unique_vertex_checks("StructuralModelBuilder", model, builder, components, [4, 5])
    section, section_builder, components = build_cross_section()
    check_unique_vertex_checks("CrossSectionBuilder", section, section_builder, components, [3, 4])
    # A component which is not in the model is refused with a data error
    other, other_builder, other_components = build_structural_model()
    check_raises(
        lambda: builder.set_unique_vertex(vertex(other_components[0], 4), 0),
        opengeode.OpenGeodeDataError,
        "set_unique_vertex with a component of another model",
    )


def test_copy_returns_the_mapping():
    model, builder, components = build_structural_model()
    fault = builder.add_fault(geosciences.Fault3D.FaultType.normal)
    horizon = builder.add_horizon()
    builder.add_surface_in_fault(components[0], model.fault(fault))
    copy = geosciences.StructuralModel()
    mapping = geosciences.StructuralModelBuilder(copy).copy(model)
    check(
        isinstance(mapping, opengeode.ModelCopyMapping),
        "StructuralModelBuilder.copy should return the ModelCopyMapping",
    )
    new_fault = mapping.at(opengeode.ComponentType("Fault")).in2out(fault)
    check(copy.has_fault(new_fault), "The mapping should give the copied Fault")
    check(
        copy.fault(new_fault).type() == geosciences.Fault3D.FaultType.normal,
        "The copied Fault should keep its type",
    )
    check(copy.nb_horizons() == 1, "The Horizon should be copied")
    check(model.nb_horizons() == 1 and horizon is not None, "The source is unchanged")
    check_raises(
        lambda: geosciences.StructuralModelBuilder(copy).copy(model),
        opengeode.OpenGeodeDataError,
        "copy into a model which is not empty",
    )

    section, section_builder, section_components = build_cross_section()
    section_fault = section_builder.add_fault()
    section_builder.add_line_in_fault(
        section_components[0], section.fault(section_fault)
    )
    section_copy = geosciences.CrossSection()
    section_mapping = geosciences.CrossSectionBuilder(section_copy).copy(section)
    check(
        isinstance(section_mapping, opengeode.ModelCopyMapping),
        "CrossSectionBuilder.copy should return the ModelCopyMapping",
    )
    check(
        section_copy.has_fault(
            section_mapping.at(opengeode.ComponentType("Fault")).in2out(section_fault)
        ),
        "The mapping should give the copied Fault of the CrossSection",
    )
    check_raises(
        lambda: geosciences.CrossSectionBuilder(section_copy).copy(section),
        opengeode.OpenGeodeDataError,
        "copy into a cross section which is not empty",
    )


if __name__ == "__main__":
    geosciences.OpenGeodeGeosciencesExplicitLibrary.initialize()
    test_builders_keep_models_alive()
    test_builders_remember_their_model()
    test_unique_vertex_checks()
    test_copy_returns_the_mapping()
