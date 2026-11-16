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

"""Index checks, lifetimes, copies and conventions of the implicit models, of
the horizons stack and of their builders."""

import gc
import os
import weakref

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as explicit
import opengeode_geosciences_py_implicit as implicit

DATA = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../../../tests/data")
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


def stratigraphic_model():
    model = implicit.StratigraphicModel(structural_model())
    implicit.ImplicitStructuralModelBuilder(
        model
    ).import_old_implicit_attribute_values_from_attribute_name(
        "geode_implicit_attribute"
    )
    implicit.StratigraphicModelBuilder(
        model
    ).import_old_stratigraphic_attribute_values_from_attribute_name(
        "geode_stratigraphic_location"
    )
    return model


def stratigraphic_section():
    path = DATA + "/test_section.og_sctn"
    section = explicit.CrossSection(
        opengeode.load_one(opengeode.load_section(path), path)
    )
    result = implicit.StratigraphicSection(section)
    builder = implicit.StratigraphicSectionBuilder(result)
    for surface in result.surfaces():
        mesh = surface.mesh()
        for vertex_id in range(mesh.nb_vertices()):
            builder.set_stratigraphic_coordinates(
                surface,
                vertex_id,
                implicit.StratigraphicPoint2D(
                    [mesh.point(vertex_id).value(0), float(vertex_id)]
                ),
            )
    return result


def test_model_index_checks():
    model = stratigraphic_model()
    block = model.blocks()[0]
    nb_vertices = block.mesh().nb_vertices()
    nb_polyhedra = block.mesh().nb_polyhedra()
    point = block.mesh().polyhedron_barycenter(0)
    strati_point = model.stratigraphic_coordinates_from_vertex_id(block, 0)
    builder = implicit.StratigraphicModelBuilder(model)

    # Valid indices work
    model.implicit_value_from_vertex_id(block=block, vertex_id=nb_vertices - 1)
    model.stratigraphic_coordinates_from_vertex_id(block, nb_vertices - 1)
    model.implicit_value_from_geometric_point_and_tetra_id(block, point, 0)
    model.stratigraphic_coordinates_from_geometric_point_and_tetra_id(block, point, 0)
    model.geometric_coordinates_from_stratigraphic_point_and_tetra_id(
        block, strati_point, 0
    )
    builder.set_implicit_value(block, nb_vertices - 1, 1.5)
    check(
        model.implicit_value_from_vertex_id(block, nb_vertices - 1) == 1.5,
        "set_implicit_value",
    )

    calls = {
        "implicit_value_from_vertex_id": lambda: model.implicit_value_from_vertex_id(
            block, nb_vertices
        ),
        "stratigraphic_coordinates_from_vertex_id": lambda: model.stratigraphic_coordinates_from_vertex_id(
            block, nb_vertices
        ),
        "implicit_value_from_geometric_point_and_tetra_id": lambda: model.implicit_value_from_geometric_point_and_tetra_id(
            block, point, nb_polyhedra
        ),
        "stratigraphic_coordinates_from_geometric_point_and_tetra_id": lambda: model.stratigraphic_coordinates_from_geometric_point_and_tetra_id(
            block, point, nb_polyhedra
        ),
        "geometric_coordinates_from_stratigraphic_point_and_tetra_id": lambda: model.geometric_coordinates_from_stratigraphic_point_and_tetra_id(
            block, strati_point, nb_polyhedra
        ),
        "set_implicit_value": lambda: builder.set_implicit_value(
            block, nb_vertices, 0.0
        ),
        "set_stratigraphic_location": lambda: builder.set_stratigraphic_location(
            block, nb_vertices, opengeode.Point2D([0.0, 0.0])
        ),
        "set_stratigraphic_coordinates": lambda: builder.set_stratigraphic_coordinates(
            block, nb_vertices, strati_point
        ),
    }
    for name, call in calls.items():
        check_raises(call, IndexError, name + " with an index out of range")
        check_raises(call, Exception, name + " with an index out of range")
    check_raises(
        lambda: model.implicit_value_from_vertex_id(block, -1),
        TypeError,
        "implicit_value_from_vertex_id with a negative index",
    )
    # The implicit model has the same checks
    implicit_model = implicit.ImplicitStructuralModel(structural_model())
    implicit_block = implicit_model.blocks()[0]
    check_raises(
        lambda: implicit_model.implicit_value_from_vertex_id(
            implicit_block, implicit_block.mesh().nb_vertices()
        ),
        IndexError,
        "ImplicitStructuralModel.implicit_value_from_vertex_id",
    )
    check_raises(
        lambda: implicit.ImplicitStructuralModelBuilder(
            implicit_model
        ).set_implicit_value(implicit_block, implicit_block.mesh().nb_vertices(), 0.0),
        IndexError,
        "ImplicitStructuralModelBuilder.set_implicit_value",
    )


def test_section_index_checks():
    section = stratigraphic_section()
    surface = section.surfaces()[0]
    nb_vertices = surface.mesh().nb_vertices()
    nb_polygons = surface.mesh().nb_polygons()
    point = surface.mesh().polygon_barycenter(0)
    strati_point = section.stratigraphic_coordinates_from_vertex_id(surface, 0)
    builder = implicit.StratigraphicSectionBuilder(section)

    section.implicit_value_from_vertex_id(surface=surface, vertex_id=nb_vertices - 1)
    section.stratigraphic_coordinates_from_vertex_id(surface, nb_vertices - 1)
    section.implicit_value_from_geometric_point_and_triangle_id(surface, point, 0)
    section.stratigraphic_coordinates_from_geometric_point_and_triangle_id(
        surface, point, 0
    )
    section.geometric_coordinates_from_stratigraphic_point_and_triangle_id(
        surface, strati_point, 0
    )
    calls = {
        "implicit_value_from_vertex_id": lambda: section.implicit_value_from_vertex_id(
            surface, nb_vertices
        ),
        "stratigraphic_coordinates_from_vertex_id": lambda: section.stratigraphic_coordinates_from_vertex_id(
            surface, nb_vertices
        ),
        "implicit_value_from_geometric_point_and_triangle_id": lambda: section.implicit_value_from_geometric_point_and_triangle_id(
            surface, point, nb_polygons
        ),
        "stratigraphic_coordinates_from_geometric_point_and_triangle_id": lambda: section.stratigraphic_coordinates_from_geometric_point_and_triangle_id(
            surface, point, nb_polygons
        ),
        "geometric_coordinates_from_stratigraphic_point_and_triangle_id": lambda: section.geometric_coordinates_from_stratigraphic_point_and_triangle_id(
            surface, strati_point, nb_polygons
        ),
        "set_implicit_value": lambda: builder.set_implicit_value(
            surface, nb_vertices, 0.0
        ),
        "set_stratigraphic_location": lambda: builder.set_stratigraphic_location(
            surface, nb_vertices, opengeode.Point1D([0.0])
        ),
        "set_stratigraphic_coordinates": lambda: builder.set_stratigraphic_coordinates(
            surface, nb_vertices, strati_point
        ),
    }
    for name, call in calls.items():
        check_raises(call, IndexError, name + " with an index out of range")


def test_builders_keep_models_alive():
    for model_class, builder_class in [
        (implicit.ImplicitStructuralModel, implicit.ImplicitStructuralModelBuilder),
        (implicit.ImplicitCrossSection, implicit.ImplicitCrossSectionBuilder),
        (implicit.StratigraphicModel, implicit.StratigraphicModelBuilder),
        (implicit.StratigraphicSection, implicit.StratigraphicSectionBuilder),
    ]:
        model = model_class()
        reference = weakref.ref(model)
        builder = builder_class(model)
        check(builder._model is model, f"{builder_class.__name__} remembers its model")
        del model
        gc.collect()
        check(reference() is not None, f"{builder_class.__name__} keeps its model")
        del builder
        gc.collect()
        check(reference() is None, f"{model_class.__name__} is released")
    # The builder of the stack keeps the builder of the model, and so the model
    for model_class, builder_class in [
        (implicit.ImplicitStructuralModel, implicit.ImplicitStructuralModelBuilder),
        (implicit.ImplicitCrossSection, implicit.ImplicitCrossSectionBuilder),
    ]:
        model = model_class()
        reference = weakref.ref(model)
        stack_builder = builder_class(model).horizons_stack_builder()
        del model
        gc.collect()
        check(reference() is not None, "The stack builder keeps the model alive")
        stack_builder.add_horizon()
        check(reference().horizons_stack().nb_horizons() == 1, "stack builder works")
        del stack_builder
        gc.collect()
        check(reference() is None, "The model is released with the stack builder")
    stack = implicit.HorizonsStack3D()
    reference = weakref.ref(stack)
    stack_builder = implicit.HorizonsStackBuilder3D(stack)
    relationships_builder = implicit.StratigraphicRelationshipsBuilder(stack)
    del stack
    gc.collect()
    check(reference() is not None, "The builders keep the stack alive")
    del stack_builder, relationships_builder
    gc.collect()
    check(reference() is None, "The stack is released with its builders")


def test_builders_copy_and_return_the_mapping():
    model = stratigraphic_model()
    for source, copy, builder_class in [
        (model, implicit.StratigraphicModel(), implicit.StratigraphicModelBuilder),
        (model, implicit.ImplicitStructuralModel(), implicit.ImplicitStructuralModelBuilder),
    ]:
        mapping = builder_class(copy).copy(source)
        check(
            isinstance(mapping, opengeode.ModelCopyMapping),
            f"{builder_class.__name__}.copy returns the ModelCopyMapping",
        )
        check(copy.nb_blocks() == source.nb_blocks(), "blocks copied")
        check(
            copy.horizons_stack().nb_horizons() == source.horizons_stack().nb_horizons(),
            "the horizons stack is copied",
        )
        check_raises(
            lambda: builder_class(copy).copy(source),
            opengeode.OpenGeodeDataError,
            "copy into a model which is not empty",
        )
    section = stratigraphic_section()
    for copy, builder_class in [
        (implicit.StratigraphicSection(), implicit.StratigraphicSectionBuilder),
        (implicit.ImplicitCrossSection(), implicit.ImplicitCrossSectionBuilder),
    ]:
        mapping = builder_class(copy).copy(section)
        check(
            isinstance(mapping, opengeode.ModelCopyMapping),
            f"{builder_class.__name__}.copy returns the ModelCopyMapping",
        )
        check(copy.nb_surfaces() == section.nb_surfaces(), "surfaces copied")


def test_stratigraphic_point():
    first = implicit.StratigraphicPoint3D(
        stratigraphic_location=opengeode.Point2D([1.0, 2.0]), implicit_value=3.0
    )
    second = implicit.StratigraphicPoint3D(values=[1.0, 2.0, 3.0])
    third = implicit.StratigraphicPoint3D(
        stratigraphic_coordinates=opengeode.Point3D([1.0, 2.0, 4.0])
    )
    check(first == second, "StratigraphicPoints with the same values are equal")
    check(hash(first) == hash(second), "equal points have the same hash")
    check(first != third, "StratigraphicPoints with other values differ")
    check(len({first, second, third}) == 2, "StratigraphicPoints are hashable")
    check(
        repr(first) == "StratigraphicPoint3D(Point2D([1.0, 2.0]), 3.0)",
        f"repr {first!r}",
    )
    check(
        eval(repr(first), {**vars(implicit), **vars(opengeode)}) == first,
        "repr is evaluable",
    )
    first.set_implicit_value(implicit_value=5.0)
    first.set_stratigraphic_location(
        stratigraphic_location=opengeode.Point2D([0.0, 0.5])
    )
    check(first.implicit_value() == 5.0, "set_implicit_value")
    check(
        first.stratigraphic_coordinates().inexact_equal(
            opengeode.Point3D([0.0, 0.5, 5.0])
        ),
        "stratigraphic_coordinates",
    )
    section_point = implicit.StratigraphicPoint2D(
        opengeode.Point1D([1.0]), 2.0
    )
    check(
        repr(section_point) == "StratigraphicPoint2D(Point1D([1.0]), 2.0)",
        f"repr {section_point!r}",
    )
    check(section_point == implicit.StratigraphicPoint2D([1.0, 2.0]), "2D equality")


def build_stack():
    stack = implicit.HorizonsStack3D()
    builder = implicit.HorizonsStackBuilder3D(horizons_stack=stack)
    horizons = [builder.add_horizon() for _ in range(2)]
    units = [builder.add_stratigraphic_unit() for _ in range(3)]
    stack.horizon(horizons[0])
    builder.set_horizon_under(
        horizon_under=stack.horizon(horizons[0]),
        strati_unit_above=stack.stratigraphic_unit(units[0]),
    )
    builder.set_horizon_above(
        horizon_above=stack.horizon(horizons[0]),
        strati_unit_under=stack.stratigraphic_unit(units[1]),
    )
    builder.set_horizon_under(stack.horizon(horizons[1]), stack.stratigraphic_unit(units[1]))
    builder.set_horizon_above(stack.horizon(horizons[1]), stack.stratigraphic_unit(units[2]))
    return stack, builder, horizons, units


def test_horizons_stack():
    stack, builder, horizons, units = build_stack()
    # Without top and bottom horizons the iterations are empty
    check(stack.top_horizon() is None and stack.bottom_horizon() is None, "not computed")
    check(len(stack.top_to_bottom_horizons()) == 0, "empty iteration")
    builder.compute_top_and_bottom_horizons()
    check(len(stack.top_to_bottom_horizons()) == 2, "top_to_bottom_horizons")
    check(len(stack.bottom_to_top_units()) == 3, "bottom_to_top_units")
    check(stack.top_horizon() is not None, "top horizon computed")
    # Relationships
    check(stack.is_above(above=units[0], under=horizons[0]), "is_above")
    check(
        stack.is_above(units[0], units[2]),
        "is_above through the other components",
    )
    check(not stack.is_above(units[2], units[0]), "is_above is oriented")
    check(stack.is_directly_above(units[0], horizons[0]), "is_directly_above")
    check(not stack.is_directly_above(horizons[0], units[0]), "not directly above")
    # Conformity
    check(stack.is_conformal_above(component=horizons[0]), "conformal above")
    check(stack.is_conformal_under(horizons[0]), "conformal under")
    check_raises(
        lambda: stack.is_conformal_above(opengeode.uuid()),
        opengeode.OpenGeodeDataError,
        "is_conformal_above of an unknown component",
    )
    # clone, copy constructor, repr
    for copy in (stack.clone(), implicit.HorizonsStack3D(stack=stack)):
        check(copy.nb_horizons() == 2 and copy.nb_stratigraphic_units() == 3, "clone")
        check(copy.id().string() == stack.id().string(), "the clone keeps its id")
        check(copy.top_horizon() == stack.top_horizon(), "the clone keeps the top")
    text = repr(stack)
    check(
        text.startswith("HorizonsStack3D(id='")
        and "nb_horizons=2" in text
        and "nb_stratigraphic_units=3" in text,
        f"repr {text}",
    )
    check(stack.string(), "string")
    check(stack.native_extension() == "og_hst3d", "native extension")
    check(
        stack.horizon_stack_component(id=horizons[0]).id().string() == horizons[0].string(),
        "horizon_stack_component",
    )
    check_raises(
        lambda: stack.horizon_stack_component(opengeode.uuid()),
        opengeode.OpenGeodeError,
        "horizon_stack_component of an unknown uuid",
    )
    # Insertion
    info = builder.add_horizon_in_stratigraphic_unit(
        strati_unit=stack.stratigraphic_unit(units[1])
    )
    check(
        repr(info).startswith("InsertedHorizonInfo3D(new_horizon_id=")
        and "strati_unit_under_id=" in repr(info),
        f"repr of the inserted horizon info {info!r}",
    )
    check(stack.nb_horizons() == 3 and stack.nb_stratigraphic_units() == 4, "inserted")
    info.new_horizon_id = opengeode.uuid()
    check(info.new_horizon_id != info.strati_unit_above_id, "fields are writable")
    # Removal keeps the order up to date
    builder.remove_stratigraphic_unit(
        stratigraphic_unit=stack.stratigraphic_unit(units[2])
    )
    check(stack.nb_stratigraphic_units() == 3, "remove_stratigraphic_unit")
    # Copies
    copy = implicit.HorizonsStack3D()
    mapping = implicit.HorizonsStackBuilder3D(copy).copy(horizons_stack=stack)
    check(isinstance(mapping, opengeode.ModelCopyMapping), "copy returns the mapping")
    check(copy.nb_horizons() == stack.nb_horizons(), "copied horizons")
    check_raises(
        lambda: implicit.HorizonsStackBuilder3D(copy).copy(stack),
        opengeode.OpenGeodeDataError,
        "copy into a stack which is not empty",
    )
    components = implicit.HorizonsStack3D()
    components_mapping = implicit.HorizonsStackBuilder3D(components).copy_components(
        horizons_stack=stack
    )
    check(isinstance(components_mapping, opengeode.ModelCopyMapping), "mapping")
    check(components.nb_horizons() == stack.nb_horizons(), "copy_components")
    # The mapping can be given
    preset = implicit.HorizonsStack3D()
    given = opengeode.ModelCopyMapping()
    implicit.HorizonsStackBuilder3D(preset).copy(mapping=given, horizons_stack=stack)
    check(preset.nb_horizons() == stack.nb_horizons(), "copy with a mapping")
    other = implicit.HorizonsStack3D()
    implicit.HorizonsStackBuilder3D(other).copy_components(
        mapping=opengeode.ModelCopyMapping(), horizons_stack=stack
    )
    check(other.nb_stratigraphic_units() == stack.nb_stratigraphic_units(), "components")


def test_creation_with_uuid():
    for dimension in ("2D", "3D"):
        stack = getattr(implicit, "HorizonsStack" + dimension)()
        builder = getattr(implicit, "HorizonsStackBuilder" + dimension)(stack)
        horizon_id = opengeode.uuid()
        unit_id = opengeode.uuid()
        builder.add_horizon(horizon_id=horizon_id)
        builder.add_stratigraphic_unit(stratigraphic_unit_id=unit_id)
        check(stack.has_horizon(horizon_id), "add_horizon with a uuid")
        check(stack.has_stratigraphic_unit(unit_id), "add_stratigraphic_unit with a uuid")


def test_stratigraphic_relationships():
    stack, builder, horizons, units = build_stack()
    check(stack.above(element=horizons[0]) == units[0], "above")
    check(stack.under(element=horizons[0]) == units[1], "under")
    check(stack.above(units[0]) is None, "nothing above the top unit")
    builder.remove_above_relation(id1=units[0], id2=horizons[0])
    check(not stack.is_directly_above(units[0], horizons[0]), "remove_above_relation")
    builder.remove_relation(id1=horizons[0], id2=units[1])
    builder.unregister_component(id=units[2])


def test_helpers():
    for dimension in ("2D", "3D"):
        names = getattr(implicit, "horizons_stack_from_top_to_bottom_names" + dimension)(
            horizons_names=["H2", "H1"], units_names=["U3", "U2", "U1"]
        )
        check(names.nb_horizons() == 2, "from top to bottom")
        reverse = getattr(implicit, "horizons_stack_from_bottom_to_top_names" + dimension)(
            horizons_names=["H1", "H2"], units_names=["U1", "U2", "U3"]
        )
        check(reverse.nb_stratigraphic_units() == 3, "from bottom to top")
        check(
            getattr(implicit, "horizon_id_from_name" + dimension)(
                horizons_stack=names, horizon_name="H1"
            )
            is not None,
            "horizon_id_from_name",
        )
        check(
            getattr(implicit, "stratigraphic_unit_id_from_name" + dimension)(
                horizons_stack=names, unit_name="U1"
            )
            is not None,
            "stratigraphic_unit_id_from_name",
        )
        stack_class = getattr(implicit, "HorizonsStack" + dimension)
        builder_class = getattr(implicit, "HorizonsStackBuilder" + dimension)
        stack = stack_class()
        builder = builder_class(stack)
        builder.add_horizon()
        getattr(implicit, "repair_horizon_stack_if_possible" + dimension)(
            horizons_stack=stack, builder=builder
        )
        check(stack.nb_stratigraphic_units() == 2, "repair adds the missing units")
    check_raises(
        lambda: implicit.horizons_stack_from_top_to_bottom_names3D(
            ["H1"], ["U1", "U2", "U3", "U4"]
        ),
        opengeode.OpenGeodeDataError,
        "too many units",
    )


def test_implicit_helpers():
    model = implicit.ImplicitStructuralModel(structural_model())
    builder = implicit.ImplicitStructuralModelBuilder(model)
    builder.instantiate_implicit_attribute_on_blocks()
    block = model.blocks()[0]
    builder.set_implicit_value(block=block, vertex_id=0, value=2.0)
    implicit.rescale_model_implicit_value(model=model, scaling_factor=3.0)
    check(
        model.implicit_value_from_vertex_id(block, 0) == 6.0,
        "rescale_model_implicit_value",
    )
    strati = stratigraphic_model()
    implicit.rescale_model_implicit_value_to_bbox_scale(model=strati)
    section = stratigraphic_section()
    implicit.rescale_section_implicit_value(section=section, scaling_factor=2.0)
    implicit.rescale_section_implicit_value_to_bbox_scale(section=section)
    check_raises(
        lambda: implicit.stratigraphic_model_from_implicit_model_and_coords(
            implicit_model=model, implicit_axis=3
        ),
        opengeode.OpenGeodeDataError,
        "an axis out of range",
    )
    result = implicit.stratigraphic_model_from_implicit_model_and_coords(
        implicit_model=model, implicit_axis=2
    )
    check(isinstance(result, implicit.StratigraphicModel), "stratigraphic model")


def test_scalar_field_helpers():
    structural = structural_model()
    structural_builder = explicit.StructuralModelBuilder(structural)
    structural_builder.remove_block(structural.blocks()[1])
    block = structural.blocks()[0]
    manager = block.mesh().vertex_attribute_manager()
    attribute_id = manager.create_attribute_variable_double("scalar_field", 0.0)
    attribute = manager.find_attribute_variable_double(attribute_id)
    for vertex_id in range(block.mesh().nb_vertices()):
        attribute.set_value(vertex_id, 2.0 * vertex_id)
    result = implicit.implicit_model_from_structural_model_scalar_field(
        structural_model=structural, scalar_attribute_id=attribute_id
    )
    check(isinstance(result, implicit.ImplicitStructuralModel), "implicit model")
    result_block = result.block(block.id())
    check(
        result.implicit_value_from_vertex_id(result_block, 3) == 6.0,
        "the implicit values come from the scalar field",
    )
    check(structural.nb_blocks() == 1, "the structural model is cloned, not consumed")
    check_raises(
        lambda: implicit.implicit_model_from_structural_model_scalar_field(
            structural, opengeode.uuid()
        ),
        opengeode.OpenGeodeError,
        "an unknown scalar attribute",
    )


def test_stratigraphic_surfaces_are_saved(directory):
    model = stratigraphic_model()
    prefix = os.path.join(directory, "strati_blocks_")
    implicit.save_stratigraphic_blocks(model=model, prefix=prefix)
    check(os.path.exists(prefix + "0.og_tso3d"), "save_stratigraphic_blocks")
    section = stratigraphic_section()
    prefix = os.path.join(directory, "strati_surfaces_")
    implicit.save_stratigraphic_surfaces(section=section, prefix=prefix)
    check(os.path.exists(prefix + "0.og_tsf2d"), "save_stratigraphic_surfaces")


if __name__ == "__main__":
    import tempfile

    implicit.OpenGeodeGeosciencesImplicitLibrary.initialize()
    test_model_index_checks()
    test_section_index_checks()
    test_builders_keep_models_alive()
    test_builders_copy_and_return_the_mapping()
    test_stratigraphic_point()
    test_horizons_stack()
    test_creation_with_uuid()
    test_stratigraphic_relationships()
    test_helpers()
    test_implicit_helpers()
    test_scalar_field_helpers()
    with tempfile.TemporaryDirectory() as directory:
        test_stratigraphic_surfaces_are_saved(directory)
