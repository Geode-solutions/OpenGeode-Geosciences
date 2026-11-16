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

"""The optional fields of the simulation structures.

Material.thermal (an optional RockThermalProperties) is a live reference:
modifying the value read modifies the material, and it cannot be set to None
from Python (the stored object would be destroyed while Python references it).
The optional scalars and arrays (porosity, permeability, max_time_step,
frequency, restart_frequency) are values: a value read stays unchanged when
the field is reassigned or reset to None."""

import gc

import geode_test_env

import opengeode_geosciences_py_simulation as simulation


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def rock_thermal(capacity):
    thermal = simulation.RockThermalProperties()
    thermal.volumetric_heat_capacity = capacity
    thermal.thermal_conductivity = [2.0, 2.0, 2.0]
    return thermal


def churn_memory():
    gc.collect()
    return [bytearray(64) for _ in range(100000)]


def check_none_refused(material, message):
    try:
        material.thermal = None
    except TypeError:
        return
    raise ValueError("[Test] " + message)


def test_material_thermal():
    material = simulation.Material()
    check(material.thermal is None, "Material.thermal should be None by default")
    check_none_refused(
        material, "Material.thermal = None should raise TypeError while unset"
    )
    check(material.thermal is None, "Material.thermal should stay None")
    assigned = rock_thermal(2e6)
    material.thermal = assigned
    assigned.volumetric_heat_capacity = 9e6
    check(
        material.thermal.volumetric_heat_capacity == 2e6,
        "Material.thermal should store a copy of the assigned object",
    )
    thermal = material.thermal
    thermal.volumetric_heat_capacity = 3e6
    thermal.thermal_conductivity = [1.0, 2.0, 3.0]
    check(
        material.thermal.volumetric_heat_capacity == 3e6
        and material.thermal.thermal_conductivity == [1.0, 2.0, 3.0],
        "Modifying Material.thermal in place should modify the material",
    )
    check_none_refused(material, "Material.thermal = None should raise TypeError")
    check(
        material.thermal is not None
        and material.thermal.volumetric_heat_capacity == 3e6,
        "A refused None should leave Material.thermal unchanged",
    )
    material.thermal = rock_thermal(4e6)
    junk = churn_memory()
    check(
        thermal.volumetric_heat_capacity == 4e6
        and thermal.thermal_conductivity == [2.0, 2.0, 2.0],
        "Material.thermal read before a reassignment should show the new value",
    )
    del junk
    for capacity in range(100):
        material.thermal = rock_thermal(float(capacity))
    check(
        thermal.volumetric_heat_capacity == 99.0,
        "Material.thermal read before many reassignments should stay valid",
    )
    del material
    junk = churn_memory()
    check(
        thermal.volumetric_heat_capacity == 99.0,
        "Material.thermal should keep its material alive",
    )
    del junk


def test_scalar_and_array_fields():
    material = simulation.Material()
    check(
        material.porosity is None and material.permeability is None,
        "Material porosity and permeability should be None by default",
    )
    material.porosity = 0.2
    material.permeability = [1e-13, 2e-13, 3e-13]
    porosity = material.porosity
    permeability = material.permeability
    material.porosity = None
    material.permeability = None
    check(
        porosity == 0.2 and permeability == [1e-13, 2e-13, 3e-13],
        "Material porosity and permeability values should be unchanged",
    )
    check(
        material.porosity is None and material.permeability is None,
        "Material porosity and permeability should be reset",
    )
    material.permeability = [1.0, 2.0, 3.0]
    permeability = material.permeability
    permeability[0] = 5.0
    check(
        material.permeability == [1.0, 2.0, 3.0],
        "Modifying a Material.permeability value should not modify the material",
    )
    schedule = simulation.Schedule(0.0, 10.0, 1.0, 5.0)
    max_time_step = schedule.max_time_step
    schedule.max_time_step = None
    check(
        max_time_step == 5.0 and schedule.max_time_step is None,
        "Schedule.max_time_step value should be unchanged",
    )
    output = simulation.OutputSettings()
    output.frequency = 1e5
    output.restart_frequency = 1e6
    frequency = output.frequency
    restart_frequency = output.restart_frequency
    output.frequency = None
    output.restart_frequency = None
    check(
        frequency == 1e5 and restart_frequency == 1e6,
        "OutputSettings frequency values should be unchanged",
    )
    check(
        output.frequency is None and output.restart_frequency is None,
        "OutputSettings frequencies should be reset",
    )


def test_docstrings():
    thermal_doc = simulation.Material.thermal.__doc__ or ""
    check(
        "live reference" in thermal_doc
        and "cannot be set to None" in thermal_doc
        and "copy" not in thermal_doc,
        "Material.thermal should say it is a live reference: " + thermal_doc,
    )
    for cls, names in (
        (simulation.Material, ("porosity", "permeability")),
        (simulation.Schedule, ("max_time_step",)),
        (simulation.OutputSettings, ("frequency", "restart_frequency")),
    ):
        for name in names:
            doc = getattr(cls, name).__doc__ or ""
            check(
                "None" in doc and "copy" not in doc and "live" not in doc,
                cls.__name__ + "." + name + " should be a plain optional: " + doc,
            )
    for cls, name in (
        (simulation.Material, "regions"),
        (simulation.OutputSettings, "times"),
    ):
        doc = getattr(cls, name).__doc__ or ""
        check(
            "returned as a copy" in doc,
            cls.__name__ + "." + name + " should say it is a copy: " + doc,
        )


if __name__ == "__main__":
    test_material_thermal()
    test_scalar_and_array_fields()
    test_docstrings()
