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

"""The operations removing or replacing geological components, or the
HorizonsStack of an implicit model, warn in their docstrings that the Python
objects of what they remove refer to freed memory afterwards, and the uuid
advice they give works."""

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as explicit
import opengeode_geosciences_py_implicit as implicit

FREED = "refer to freed memory"
# Removals of relations, not of objects
RELATION_REMOVALS = {"remove_relation", "remove_above_relation"}


def removal_methods(module):
    for class_name, cls in sorted(vars(module).items()):
        if not isinstance(cls, type):
            continue
        # Only the methods defined here: the inherited OpenGeode ones are
        # checked by OpenGeode
        for name in sorted(vars(cls)):
            if name.startswith("remove_") and name not in RELATION_REMOVALS:
                yield class_name + "." + name, getattr(cls, name)


def check_doc(qualified_name, method, *claims):
    doc = method.__doc__ or ""
    for claim in claims:
        if claim not in doc:
            raise ValueError(
                "[Test] " + qualified_name + " should say '" + claim + "': " + doc
            )


def test_explicit_removals():
    names = []
    for name, method in removal_methods(explicit):
        names.append(name)
        check_doc(name, method, FREED, "keep its uuid", "(has_")
    for builder in ("StructuralModelBuilder", "CrossSectionBuilder"):
        for kind in ("fault", "horizon", "fault_block", "stratigraphic_unit"):
            if builder + ".remove_" + kind not in names:
                raise ValueError("[Test] Missing " + builder + ".remove_" + kind)


def test_explicit_uuid_advice():
    model = explicit.StructuralModel()
    builder = explicit.StructuralModelBuilder(model)
    fault_id = builder.add_fault()
    horizon_id = builder.add_horizon()
    builder.remove_fault(model.fault(fault_id))
    builder.remove_horizon(model.horizon(horizon_id))
    if model.has_fault(fault_id) or model.has_horizon(horizon_id):
        raise ValueError("[Test] The removed components should be gone")


def test_implicit_removals():
    names = []
    for name, method in removal_methods(implicit):
        names.append(name)
        check_doc(name, method, FREED)
    for dimension in ("2D", "3D"):
        stack_builder = getattr(implicit, "HorizonsStackBuilder" + dimension)
        for name in ("remove_horizon", "remove_stratigraphic_unit"):
            qualified_name = stack_builder.__name__ + "." + name
            if qualified_name not in names:
                raise ValueError("[Test] Missing " + qualified_name)
            check_doc(qualified_name, getattr(stack_builder, name), "(has_")
        check_doc(
            stack_builder.__name__ + ".add_horizon_in_stratigraphic_unit",
            stack_builder.add_horizon_in_stratigraphic_unit,
            FREED,
        )
    for builder, model in (
        (implicit.ImplicitStructuralModelBuilder, implicit.ImplicitStructuralModel),
        (implicit.ImplicitCrossSectionBuilder, implicit.ImplicitCrossSection),
    ):
        check_doc(
            builder.__name__ + ".set_horizons_stack",
            builder.set_horizons_stack,
            FREED,
            "horizons_stack()",
        )
        check_doc(
            model.__name__ + ".horizons_stack",
            model.horizons_stack,
            FREED,
            "set_horizons_stack",
        )


def test_implicit_uuid_advice():
    stack = implicit.horizons_stack_from_top_to_bottom_names3D(
        horizons_names=["h1", "h2"], units_names=["u1", "u2", "u3"]
    )
    builder = implicit.HorizonsStackBuilder3D(stack)
    horizon_id = next(iter(stack.horizons())).id()
    builder.remove_horizon(stack.horizon(horizon_id))
    if stack.has_horizon(horizon_id):
        raise ValueError("[Test] The removed Horizon should be gone")
    model = implicit.ImplicitStructuralModel()
    implicit.ImplicitStructuralModelBuilder(model).set_horizons_stack(stack)
    if model.horizons_stack().nb_horizons() != 1:
        raise ValueError("[Test] horizons_stack() should return the new stack")


if __name__ == "__main__":
    test_explicit_removals()
    test_explicit_uuid_advice()
    test_implicit_removals()
    test_implicit_uuid_advice()
