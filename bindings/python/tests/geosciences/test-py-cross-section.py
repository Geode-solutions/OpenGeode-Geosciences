# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2022 Geode-solutions
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

import opengeode_geosciences_py_geosciences as geosciences


def add_faults(model, builder):
    fault0 = builder.add_fault()
    if model.fault(fault0).has_type():
        raise ValueError(
            "[Test] Addition of a Fault in CrossSection is not correct")

    fault1 = builder.add_fault()
    builder.set_fault_name(fault1, "fault1")
    if model.nb_faults() != 2:
        raise ValueError("[Test] Number of faults in CrossSection should be 2")
    if model.fault(fault1).name() != "fault1":
        raise ValueError("[Test] Wrong Fault name")


def add_horizons(model, builder):
    horizon0 = builder.add_horizon()
    if model.horizon(horizon0).has_type():
        raise ValueError(
            "[Test] Addition of a Horizon in CrossSection is not correct")

    horizon1 = builder.add_horizon()
    horizon2 = builder.add_horizon()
    builder.set_horizon_name(horizon2, "horizon2")
    if model.nb_horizons() != 3:
        raise ValueError(
            "[Test] Number of horizons in CrossSection should be 3")
    if model.horizon(horizon2).name() != "horizon2":
        raise ValueError("[Test] Wrong Horizon name")


def add_fault_blocks(model, builder):
    fault_block0 = builder.add_fault_block()
    fault_block1 = builder.add_fault_block()
    builder.add_fault_block()
    builder.set_fault_block_name(fault_block1, "hanging_wall")
    if model.nb_fault_blocks() != 3:
        raise ValueError(
            "[Test] Number of fault blocks in CrossSection should be 3")
    builder.remove_fault_block(model.fault_block(fault_block0))
    if model.fault_block(fault_block1).name() != "hanging_wall":
        raise ValueError("[Test] Wrong FaultBlock name")
    if model.nb_fault_blocks() != 2:
        raise ValueError(
            "[Test] Number of fault blocks in CrossSection should be 2")


def add_stratigraphic_units(model, builder):
    stratigraphic_unit0 = builder.add_stratigraphic_unit()
    stratigraphic_unit1 = builder.add_stratigraphic_unit()
    builder.add_stratigraphic_unit()
    builder.set_stratigraphic_unit_name(stratigraphic_unit1, "cretaceous")
    if model.nb_stratigraphic_units() != 3:
        raise ValueError(
            "[Test] Number of stratigraphic_units in CrossSection should be 3")
    builder.remove_stratigraphic_unit(
        model.stratigraphic_unit(stratigraphic_unit0))
    if model.stratigraphic_unit(stratigraphic_unit1).name() != "cretaceous":
        raise ValueError("[Test] Wrong StratigraphicUnit name")
    if model.nb_stratigraphic_units() != 2:
        raise ValueError(
            "[Test] Number of stratigraphic_units in CrossSection should be 2")


def add_lines(builder):
    for i in range(8):
        builder.add_line()


def add_surfaces(builder):
    for i in range(4):
        builder.add_surface()


def do_checks(model, line_uuids, surface_uuids, faults_uuids, horizons_uuids, fault_blocks_uuids, stratigraphic_units_uuids):
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

    if model.nb_items(fault_blocks_uuids[0]) != 2:
        raise ValueError(
            "[Test] Number of items in fault_blocks_uuids[0] should be 2")
    if model.nb_items(fault_blocks_uuids[1]) != 2:
        raise ValueError(
            "[Test] Number of items in fault_blocks_uuids[1] should be 2")

    if model.nb_items(stratigraphic_units_uuids[0]) != 2:
        raise ValueError(
            "[Test] Number of items in stratigraphic_units_uuids[0] should be 2")
    if model.nb_items(stratigraphic_units_uuids[1]) != 2:
        raise ValueError(
            "[Test] Number of items in stratigraphic_units_uuids[1] should be 2")

    for i in range(8):
        if i != 2:
            if model.nb_collections(line_uuids[i]) != 1:
                raise ValueError(
                    "[Test] Number of collections in which line_uuids[", i, "] is should be 1")

    for i in range(4):
        if model.nb_collections(surface_uuids[i]) != 2:
            raise ValueError("[Test] Number of collections in which surface_uuids[",
                             i, "] is should be 2 (one StratigraphicUnit and one FaultBlock")

    if len(model.horizon_items(model.horizon(horizons_uuids[2]))) != 3:
        raise ValueError(
            "[Test] Number of iterations on items in horizons_uuids[2] should be 3")
    if len(model.fault_items(model.fault(faults_uuids[1]))) != 2:
        raise ValueError(
            "[Test] Number of iterations on items in faults_uuids[1] should be 2")
    if len(model.fault_block_items(model.fault_block(fault_blocks_uuids[0]))) != 2:
        raise ValueError(
            "[Test] Number of iterations on items in fault_blocks_uuids[0] should be 2")
    if len(model.stratigraphic_unit_items(model.stratigraphic_unit(stratigraphic_units_uuids[0]))) != 2:
        raise ValueError(
            "[Test] Number of iterations on items in stratigraphic_units_uuids[0] should be 2")


def build_relations_between_geometry_and_geology(model, builder):
    lines_uuids = []
    for line in model.lines():
        lines_uuids.append(line.id())
    faults_uuids = []
    for fault in model.faults():
        faults_uuids.append(fault.id())
    horizons_uuids = []
    for horizon in model.horizons():
        horizons_uuids.append(horizon.id())

    builder.add_line_in_fault(model.line(
        lines_uuids[0]), model.fault(faults_uuids[0]))
    builder.add_line_in_fault(model.line(
        lines_uuids[1]), model.fault(faults_uuids[0]))
    builder.add_line_in_fault(model.line(
        lines_uuids[2]), model.fault(faults_uuids[0]))
    builder.add_line_in_fault(model.line(
        lines_uuids[2]), model.fault(faults_uuids[1]))
    builder.add_line_in_fault(model.line(
        lines_uuids[3]), model.fault(faults_uuids[1]))

    builder.add_line_in_horizon(model.line(
        lines_uuids[4]), model.horizon(horizons_uuids[0]))
    builder.add_line_in_horizon(model.line(
        lines_uuids[5]), model.horizon(horizons_uuids[2]))
    builder.add_line_in_horizon(model.line(
        lines_uuids[6]), model.horizon(horizons_uuids[2]))
    builder.add_line_in_horizon(model.line(
        lines_uuids[7]), model.horizon(horizons_uuids[2]))

    surfaces_uuids = []
    for surface in model.surfaces():
        surfaces_uuids.append(surface.id())

    fault_blocks_uuids = []
    for fault_block in model.fault_blocks():
        fault_blocks_uuids.append(fault_block.id())

    stratigraphic_units_uuids = []
    for stratigraphic_unit in model.stratigraphic_units():
        stratigraphic_units_uuids.append(stratigraphic_unit.id())

    builder.add_surface_in_fault_block(model.surface(
        surfaces_uuids[0]), model.fault_block(fault_blocks_uuids[0]))
    builder.add_surface_in_fault_block(model.surface(
        surfaces_uuids[1]), model.fault_block(fault_blocks_uuids[0]))
    builder.add_surface_in_fault_block(model.surface(
        surfaces_uuids[2]), model.fault_block(fault_blocks_uuids[1]))
    builder.add_surface_in_fault_block(model.surface(
        surfaces_uuids[3]), model.fault_block(fault_blocks_uuids[1]))

    builder.add_surface_in_stratigraphic_unit(model.surface(
        surfaces_uuids[0]), model.stratigraphic_unit(stratigraphic_units_uuids[0]))
    builder.add_surface_in_stratigraphic_unit(model.surface(
        surfaces_uuids[1]), model.stratigraphic_unit(stratigraphic_units_uuids[1]))
    builder.add_surface_in_stratigraphic_unit(model.surface(
        surfaces_uuids[2]), model.stratigraphic_unit(stratigraphic_units_uuids[0]))
    builder.add_surface_in_stratigraphic_unit(model.surface(
        surfaces_uuids[3]), model.stratigraphic_unit(stratigraphic_units_uuids[1]))

    do_checks(model, lines_uuids, surfaces_uuids, faults_uuids,
              horizons_uuids, fault_blocks_uuids, stratigraphic_units_uuids)


def check_reloaded_model(reloaded_model):
    if reloaded_model.nb_lines() != 8:
        raise ValueError(
            "[Test] Number of lines in reloaded model should be 8")
    if reloaded_model.nb_horizons() != 3:
        raise ValueError(
            "[Test] Number of horizons in reloaded model should be 3")
    if reloaded_model.nb_faults() != 2:
        raise ValueError(
            "[Test] Number of faults in reloaded model should be 2")
    if reloaded_model.nb_fault_blocks() != 2:
        raise ValueError(
            "[Test] Number of fault blocks in reloaded model should be 2")


def test_io(model):
    geosciences.save_cross_section(model, "test.og_xsctn")

    reloaded_model = geosciences.load_cross_section("test.og_xsctn")
    check_reloaded_model(reloaded_model)


def test_copy(model):
    copy = geosciences.CrossSection()
    copier = geosciences.CrossSectionBuilder(copy)
    copier.copy(model)

    if copy.nb_surfaces() != 4:
        raise ValueError(
            "[Test] Number of surfaces in copied model should be 4")
    if copy.nb_lines() != 8:
        raise ValueError("[Test] Number of lines in copied model should be 8")
    if copy.nb_horizons() != 3:
        raise ValueError(
            "[Test] Number of horizons in copied model should be 3")
    if copy.nb_faults() != 2:
        raise ValueError("[Test] Number of faults in copied model should be 2")
    if copy.nb_fault_blocks() != 2:
        raise ValueError(
            "[Test] Number of fault blocks in copied model should be 2")
    if copy.nb_stratigraphic_units() != 2:
        raise ValueError(
            "[Test] Number of stratigraphic units in copied model should be 2")
   # nb_surface_boundaries = 0
   # for surface in copy.surfaces():
   #     nb_surface_boundaries += len( copy.boundaries( surface ) )
   # if nb_surface_boundaries != 4:
   #     raise ValueError("[Test] Number of boundaries of Surfaces in copied model should be 4" )

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
    nb_fault_block_items = 0
    for fault_block in copy.fault_blocks():
        nb_fault_block_items += len(copy.fault_block_items(fault_block))
    if nb_fault_block_items != 4:
        raise ValueError(
            "[Test] Number of items in fault_blocks in copied model should be 4")
    nb_stratigraphic_unit_items = 0
    for stratigraphic_unit in copy.stratigraphic_units():
        nb_stratigraphic_unit_items += len(
            copy.stratigraphic_unit_items(stratigraphic_unit))
    if nb_stratigraphic_unit_items != 4:
        raise ValueError(
            "[Test] Number of items in stratigraphic_units in copied model should be 4")


def modify_model(model, builder):
    horizons_ids = []
    for horizon in model.horizons():
        horizons_ids.append(horizon.id())
        builder.set_horizon_type(
            horizon.id(), geosciences.Horizon2D.HORIZON_TYPE.TOPOGRAPHY)
        if horizon.has_type() == False:
            raise ValueError("[Test] All horizon should have a type")

    for horizon_id in horizons_ids:
        builder.remove_horizon(model.horizon(horizon_id))

    for fault in model.faults():
        builder.set_fault_type(
            fault.id(), geosciences.Fault2D.FAULT_TYPE.LISTRIC)
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
    model = geosciences.CrossSection()
    builder = geosciences.CrossSectionBuilder(model)
    add_faults(model, builder)
    add_horizons(model, builder)
    add_lines(builder)
    add_fault_blocks(model, builder)
    add_stratigraphic_units(model, builder)
    add_surfaces(builder)
    build_relations_between_geometry_and_geology(model, builder)
    test_io(model)
    test_copy(model)
    modify_model(model, builder)
