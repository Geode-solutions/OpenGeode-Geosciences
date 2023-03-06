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

import opengeode
import opengeode_geosciences_py_geosciences as geosciences


def add_faults(model, builder):
    fault0 = builder.add_fault()
    if model.fault(fault0).has_type():
        raise ValueError(
            "[Test] Addition of a Fault in StructuralModel is not correct")

    fault1 = builder.add_fault()
    builder.set_fault_name(fault1, "fault1")
    if model.nb_faults() != 2:
        raise ValueError(
            "[Test] Number of faults in StructuralModel should be 2")
    if model.fault(fault1).name() != "fault1":
        raise ValueError("[Test] Wrong Fault name")

    for fault in model.faults():
        if fault.name() != "unknown":
            builder.set_fault_name(fault.id(), "new_" + fault.name())
    if model.fault(fault1).name() != "new_fault1":
        raise ValueError("[Test] Wrong modified Fault name")


def add_horizons(model, builder):
    horizon0 = builder.add_horizon()
    if model.horizon(horizon0).has_type():
        raise ValueError(
            "[Test] Addition of a Horizon in StructuralModel is not correct")

    horizon1 = builder.add_horizon()
    horizon2 = builder.add_horizon()
    builder.set_horizon_name(horizon2, "horizon2")
    if model.nb_horizons() != 3:
        raise ValueError(
            "[Test] Number of horizons in StructuralModel should be 3")
    if model.horizon(horizon2).name() != "horizon2":
        raise ValueError("[Test] Wrong Horizon name")

    for horizon in model.horizons():
        if horizon.name() != "unknown":
            builder.set_horizon_name(horizon.id(), "new_" + horizon.name())
    if model.horizon(horizon2).name() != "new_horizon2":
        raise ValueError("[Test] Wrong modified Horizon name")


def add_surfaces(builder):
    for i in range(8):
        builder.add_surface()


def do_checks(model, surface_uuids, faults_uuids, horizons_uuids):
    if model.nb_items(faults_uuids[0]) != 3:
        raise ValueError(
            "[Test] Number of items in fault_uuids[0] should be 3")
    if model.nb_items(faults_uuids[1]) != 2:
        raise ValueError(
            "[Test] Number of items in fault_uuids[1] should be 2")

    if model.nb_items(horizons_uuids[0]) != 1:
        raise ValueError(
            "[Test] Number of items in horizons_uuids[0] should be 1")
    if model.nb_items(horizons_uuids[1]) != 0:
        raise ValueError(
            "[Test] Number of items in horizons_uuids[1] should be 0")
    if model.nb_items(horizons_uuids[2]) != 3:
        raise ValueError(
            "[Test] Number of items in horizons_uuids[2] should be 3")

    for i in range(8):
        if i != 2:
            if model.nb_collections(surface_uuids[i]) != 1:
                raise ValueError(
                    "[Test] Number of collections in which surfaces_uuids[", i, "] is should be 1")

    if len(model.horizon_items(model.horizon(horizons_uuids[2]))) != 3:
        raise ValueError(
            "[Test] Number of iterations on items in horizons_uuids[2] should be 3")
    if len(model.fault_items(model.fault(faults_uuids[1]))) != 2:
        raise ValueError(
            "[Test] Number of iterations on items in faults_uuids[1] should be 2")


def build_relations_between_geometry_and_geology(model, builder):
    surfaces_uuids = []
    for surface in model.surfaces():
        surfaces_uuids.append(surface.id())
    faults_uuids = []
    for fault in model.faults():
        faults_uuids.append(fault.id())
    horizons_uuids = []
    for horizon in model.horizons():
        horizons_uuids.append(horizon.id())

    builder.add_surface_in_fault(model.surface(
        surfaces_uuids[0]), model.fault(faults_uuids[0]))
    builder.add_surface_in_fault(model.surface(
        surfaces_uuids[1]), model.fault(faults_uuids[0]))
    builder.add_surface_in_fault(model.surface(
        surfaces_uuids[2]), model.fault(faults_uuids[0]))
    builder.add_surface_in_fault(model.surface(
        surfaces_uuids[2]), model.fault(faults_uuids[1]))
    builder.add_surface_in_fault(model.surface(
        surfaces_uuids[3]), model.fault(faults_uuids[1]))

    builder.add_surface_in_horizon(model.surface(
        surfaces_uuids[4]), model.horizon(horizons_uuids[0]))
    builder.add_surface_in_horizon(model.surface(
        surfaces_uuids[5]), model.horizon(horizons_uuids[2]))
    builder.add_surface_in_horizon(model.surface(
        surfaces_uuids[6]), model.horizon(horizons_uuids[2]))
    builder.add_surface_in_horizon(model.surface(
        surfaces_uuids[7]), model.horizon(horizons_uuids[2]))

    do_checks(model, surfaces_uuids, faults_uuids, horizons_uuids)


def check_reloaded_model(reloaded_model):
    if reloaded_model.nb_surfaces() != 8:
        raise ValueError(
            "[Test] Number of surfaces in reloaded model should be 8")
    if reloaded_model.nb_horizons() != 3:
        raise ValueError(
            "[Test] Number of horizons in reloaded model should be 3")
    if reloaded_model.nb_faults() != 2:
        raise ValueError(
            "[Test] Number of faults in reloaded model should be 2")


def test_io(model):
    geosciences.save_structural_model(model, "test.og_strm")

    reloaded_model = geosciences.load_structural_model("test.og_strm")
    check_reloaded_model(reloaded_model)


def test_copy(model):
    copy = geosciences.StructuralModel()
    copier = geosciences.StructuralModelBuilder(copy)
    copier.copy(model)

    if copy.nb_surfaces() != 8:
        raise ValueError(
            "[Test] Number of surfaces in copied model should be 8")
    if copy.nb_horizons() != 3:
        raise ValueError(
            "[Test] Number of horizons in copied model should be 3")
    if copy.nb_faults() != 2:
        raise ValueError("[Test] Number of faults in copied model should be 2")
    if copy.nb_fault_blocks() != 0:
        raise ValueError(
            "[Test] Number of fault blocks in copied model should be 0")

    #nb_surface_boundaries = 0
    # for surface in copy.surfaces():
    #    nb_surface_boundaries += len( copy.boundaries( surface ) )
    # if nb_surface_boundaries != 8:
    #    raise ValueError("[Test] Number of boundaries of Surfaces in copied model should be 8" )

    nb_fault_items = 0
    for fault in copy.faults():
        nb_fault_items += len(copy.fault_items(fault))
    if nb_fault_items != 5:
        raise ValueError(
            "[Test] Number of items in faults in copied model should be 5")
    nb_horizon_items = 0
    for horizon in copy.horizons():
        nb_horizon_items += len(copy.horizon_items(horizon))
    if nb_horizon_items != 4:
        raise ValueError(
            "[Test] Number of items in stratigraphic_units in copied model should be 4")


def modify_model(model, builder):
    horizons_ids = []
    for horizon in model.horizons():
        horizons_ids.append(horizon.id())
        builder.set_horizon_type(
            horizon.id(), geosciences.Horizon3D.HORIZON_TYPE.TOPOGRAPHY)
        if horizon.has_type() == False:
            raise ValueError("[Test] All horizons should have a type")

    for horizon_id in horizons_ids:
        builder.remove_horizon(model.horizon(horizon_id))

    for fault in model.faults():
        builder.set_fault_type(
            fault.id(), geosciences.Fault3D.FAULT_TYPE.LISTRIC)
        if fault.has_type() == False:
            raise ValueError("[Test] All faults should have a type")

    temp_id = builder.add_fault()
    builder.remove_fault(model.fault(temp_id))
    if model.nb_horizons() != 0:
        raise ValueError(
            "[Test] Number of horizons in modified model should be 0")
    if model.nb_faults() != 2:
        raise ValueError(
            "[Test] Number of faults in modified model should be 2")


if __name__ == '__main__':
    geosciences.OpenGeodeGeosciencesGeosciences.initialize()
    model = geosciences.StructuralModel()
    builder = geosciences.StructuralModelBuilder(model)
    add_faults(model, builder)
    add_horizons(model, builder)
    add_surfaces(builder)
    build_relations_between_geometry_and_geology(model, builder)
    test_io(model)
    test_copy(model)
    modify_model(model, builder)
