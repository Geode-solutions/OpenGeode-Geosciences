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

"""Geological components of the models: enums, active components, creation
with a given uuid, keyword arguments and representations."""

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


MODELS = [
    (
        "StructuralModel",
        geosciences.StructuralModel,
        geosciences.StructuralModelBuilder,
        geosciences.Fault3D,
        geosciences.Horizon3D,
    ),
    (
        "CrossSection",
        geosciences.CrossSection,
        geosciences.CrossSectionBuilder,
        geosciences.Fault2D,
        geosciences.Horizon2D,
    ),
]


def test_enums():
    for fault_class in [geosciences.Fault2D, geosciences.Fault3D]:
        names = [member.name for member in fault_class.FaultType]
        check(
            names
            == ["no_type", "normal", "reverse", "strike_slip", "listric", "decollement"],
            f"Wrong FaultType members {names}",
        )
    for horizon_class in [geosciences.Horizon2D, geosciences.Horizon3D]:
        names = [member.name for member in horizon_class.ContactType]
        check(
            names
            == [
                "conformal",
                "erosion",
                "baselap",
                "discontinuity",
                "topography",
                "intrusion",
            ],
            f"Wrong ContactType members {names}",
        )


def test_active_components():
    for name, model_class, builder_class, fault_class, horizon_class in MODELS:
        model = model_class()
        builder = builder_class(model)
        faults = [model.fault(builder.add_fault()) for _ in range(3)]
        horizons = [model.horizon(builder.add_horizon()) for _ in range(2)]
        blocks = [model.fault_block(builder.add_fault_block()) for _ in range(2)]
        units = [
            model.stratigraphic_unit(builder.add_stratigraphic_unit()) for _ in range(2)
        ]
        check(model.nb_active_faults() == 3, f"{name}: 3 active faults")
        builder.set_fault_active(fault=faults[1], active=False)
        builder.set_horizon_active(horizon=horizons[0], active=False)
        builder.set_fault_block_active(fault_block=blocks[0], active=False)
        builder.set_stratigraphic_unit_active(stratigraphic_unit=units[0], active=False)
        check(model.nb_active_faults() == 2, f"{name}: nb_active_faults")
        check(model.nb_active_horizons() == 1, f"{name}: nb_active_horizons")
        check(model.nb_active_fault_blocks() == 1, f"{name}: nb_active_fault_blocks")
        check(
            model.nb_active_stratigraphic_units() == 1,
            f"{name}: nb_active_stratigraphic_units",
        )
        check(len(model.faults()) == 3, f"{name}: faults() lists the inactive ones")
        check(
            [fault.id().string() for fault in model.active_faults()]
            == [faults[0].id().string(), faults[2].id().string()],
            f"{name}: active_faults",
        )
        check(len(model.active_horizons()) == 1, f"{name}: active_horizons")
        check(len(model.active_fault_blocks()) == 1, f"{name}: active_fault_blocks")
        check(
            len(model.active_stratigraphic_units()) == 1,
            f"{name}: active_stratigraphic_units",
        )
        check(model.has_fault(faults[0].id()), f"{name}: has_fault")
        check(not model.has_fault(horizons[0].id()), f"{name}: has_fault of a Horizon")
        check(model.has_horizon(horizons[1].id()), f"{name}: has_horizon")
        check(model.has_fault_block(blocks[1].id()), f"{name}: has_fault_block")
        check(
            model.has_stratigraphic_unit(units[1].id()),
            f"{name}: has_stratigraphic_unit",
        )
        check_raises(
            lambda: model.fault(id=horizons[0].id()),
            opengeode.OpenGeodeError,
            f"{name}: fault of an unknown uuid",
        )


def test_creation_with_uuid():
    for name, model_class, builder_class, fault_class, horizon_class in MODELS:
        model = model_class()
        builder = builder_class(model)
        fault_id = opengeode.uuid()
        horizon_id = opengeode.uuid()
        typed_fault_id = opengeode.uuid()
        typed_horizon_id = opengeode.uuid()
        block_id = opengeode.uuid()
        unit_id = opengeode.uuid()
        builder.add_fault(fault_id=fault_id)
        builder.add_horizon(horizon_id=horizon_id)
        builder.add_fault(
            fault_id=typed_fault_id, type=fault_class.FaultType.listric
        )
        builder.add_horizon(
            horizon_id=typed_horizon_id,
            contact_type=horizon_class.ContactType.baselap,
        )
        builder.add_fault_block(fault_block_id=block_id)
        builder.add_stratigraphic_unit(stratigraphic_unit_id=unit_id)
        check(model.fault(fault_id).type() == fault_class.FaultType.no_type, name)
        check(not model.fault(fault_id).has_type(), f"{name}: no type")
        check(model.fault(typed_fault_id).has_type(), f"{name}: typed fault")
        check(
            model.fault(typed_fault_id).type() == fault_class.FaultType.listric, name
        )
        check(
            model.horizon(horizon_id).contact_type()
            == horizon_class.ContactType.conformal,
            name,
        )
        check(
            model.horizon(typed_horizon_id).contact_type()
            == horizon_class.ContactType.baselap,
            name,
        )
        check(model.has_fault_block(block_id), name)
        check(model.has_stratigraphic_unit(unit_id), name)
        check(model.fault(fault_id).id().string() == fault_id.string(), name)
        # A component cannot be created twice
        check_raises(
            lambda: builder.add_fault(fault_id=fault_id),
            Exception,
            f"{name}: add_fault with an existing uuid",
        )


def test_items_return_the_relation_index():
    model = geosciences.StructuralModel()
    builder = geosciences.StructuralModelBuilder(model)
    surface = model.surface(builder.add_surface())
    block = model.block(builder.add_block())
    fault = model.fault(builder.add_fault())
    horizon = model.horizon(builder.add_horizon())
    fault_block = model.fault_block(builder.add_fault_block())
    unit = model.stratigraphic_unit(builder.add_stratigraphic_unit())
    indices = [
        builder.add_surface_in_fault(surface=surface, fault=fault),
        builder.add_surface_in_horizon(surface=surface, horizon=horizon),
        builder.add_block_in_fault_block(block=block, fault_block=fault_block),
        builder.add_block_in_stratigraphic_unit(block=block, stratigraphic_unit=unit),
    ]
    check(all(isinstance(index, int) for index in indices), "relation indices")
    check(len(set(indices)) == 4, "the relations have distinct indices")
    check(
        builder.add_surface_in_fault(surface, fault) == indices[0],
        "an existing relation returns its index",
    )
    check(len(model.stratigraphic_unit_items(unit)) == 1, "stratigraphic unit items")
    check(len(model.fault_block_items(fault_block)) == 1, "fault block items")

    section = geosciences.CrossSection()
    section_builder = geosciences.CrossSectionBuilder(section)
    line = section.line(section_builder.add_line())
    surface2d = section.surface(section_builder.add_surface())
    fault2d = section.fault(section_builder.add_fault())
    horizon2d = section.horizon(section_builder.add_horizon())
    block2d = section.fault_block(section_builder.add_fault_block())
    unit2d = section.stratigraphic_unit(section_builder.add_stratigraphic_unit())
    section_indices = [
        section_builder.add_line_in_fault(line=line, fault=fault2d),
        section_builder.add_line_in_horizon(line=line, horizon=horizon2d),
        section_builder.add_surface_in_fault_block(
            surface=surface2d, fault_block=block2d
        ),
        section_builder.add_surface_in_stratigraphic_unit(
            surface=surface2d, stratigraphic_unit=unit2d
        ),
    ]
    check(len(set(section_indices)) == 4, "the cross section relations")
    check(len(section.fault_items(fault2d)) == 1, "fault items")
    check(len(section.horizon_items(horizon2d)) == 1, "horizon items")


def test_remove_with_keywords():
    for name, model_class, builder_class, fault_class, horizon_class in MODELS:
        model = model_class()
        builder = builder_class(model)
        fault = model.fault(builder.add_fault())
        horizon = model.horizon(builder.add_horizon())
        block = model.fault_block(builder.add_fault_block())
        unit = model.stratigraphic_unit(builder.add_stratigraphic_unit())
        builder.remove_fault(fault=fault)
        builder.remove_horizon(horizon=horizon)
        builder.remove_fault_block(fault_block=block)
        builder.remove_stratigraphic_unit(stratigraphic_unit=unit)
        check(
            model.nb_faults() + model.nb_horizons() + model.nb_fault_blocks()
            + model.nb_stratigraphic_units()
            == 0,
            f"{name}: components removed",
        )


def test_representations():
    model = geosciences.StructuralModel()
    builder = geosciences.StructuralModelBuilder(model)
    builder.add_fault()
    builder.add_horizon()
    builder.add_surface()
    text = repr(model)
    check(text.startswith("StructuralModel(id='"), f"repr {text}")
    for expected in ["nb_surfaces=1", "nb_faults=1", "nb_horizons=1", "nb_blocks=0"]:
        check(expected in text, f"{expected} in {text}")
    section = geosciences.CrossSection()
    section_builder = geosciences.CrossSectionBuilder(section)
    section_builder.add_fault_block()
    text = repr(section)
    check(text.startswith("CrossSection(id='"), f"repr {text}")
    check("nb_fault_blocks=1" in text and "nb_lines=0" in text, text)
    check("nb_blocks" not in text, text)


def test_constructors():
    model = geosciences.StructuralModel()
    builder = geosciences.StructuralModelBuilder(model)
    builder.add_fault()
    copy = geosciences.StructuralModel(model=model)
    check(copy.nb_faults() == 1, "copy constructor keeps the faults")
    check(copy.id().string() == model.id().string(), "clone keeps the identifier")
    brep = opengeode.BRep()
    opengeode.BRepBuilder(brep).add_surface()
    from_brep = geosciences.StructuralModel(brep=brep)
    check(from_brep.nb_surfaces() == 1 and from_brep.nb_faults() == 0, "from a BRep")
    check(brep.nb_surfaces() == 1, "the BRep is cloned, not consumed")
    section = geosciences.CrossSection(cross_section=geosciences.CrossSection())
    check(section.nb_faults() == 0, "empty copy")
    from_section = geosciences.CrossSection(section=opengeode.Section())
    check(from_section.nb_lines() == 0, "from a Section")
    check(model.native_extension() == "og_strm", "native extension")
    check(section.native_extension() == "og_xsctn", "native extension")


if __name__ == "__main__":
    geosciences.OpenGeodeGeosciencesExplicitLibrary.initialize()
    test_enums()
    test_active_components()
    test_creation_with_uuid()
    test_items_return_the_relation_index()
    test_remove_with_keywords()
    test_representations()
    test_constructors()
