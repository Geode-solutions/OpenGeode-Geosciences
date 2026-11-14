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

"""build_structural_model_fault_blocks groups the Blocks of a
StructuralModel into new FaultBlocks (mirrors the C++ test
test-structural-model-fault-blocks-builder)."""

import os

import geode_test_env

import opengeode
import opengeode_geosciences_py_explicit as explicit

DATA = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "../../../../tests/data")
)
# The ComponentType names of the FaultBlock3D and Fault3D collections
FAULT_BLOCK_TYPE = "FaultBlock"
FAULT_TYPE = "Fault"


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def load(name):
    path = os.path.join(DATA, name)
    return opengeode.load_one(explicit.load_structural_model(path), path)


def collections_of_type(model, component_id, component_type):
    return sorted(
        collection.id.string()
        for collection in model.collections(component_id)
        if collection.type.get() == component_type
    )


def check_fault_blocks(model, expected_sizes):
    for block in model.blocks():
        fault_blocks = collections_of_type(model, block.id(), FAULT_BLOCK_TYPE)
        check(
            len(fault_blocks) == 1,
            "Block " + block.id().string() + " should be in exactly one FaultBlock",
        )
        for surface in model.boundaries(block.id()):
            if collections_of_type(model, surface.id, FAULT_TYPE):
                continue
            for other in model.incidences(surface.id):
                check(
                    collections_of_type(model, other.id, FAULT_BLOCK_TYPE)
                    == fault_blocks,
                    "Blocks sharing a non-fault Surface should be in the "
                    "same FaultBlock",
                )
    sizes = sorted(
        model.nb_items(fault_block.id()) for fault_block in model.fault_blocks()
    )
    check(sizes == expected_sizes, "Wrong FaultBlock sizes: " + str(sizes))


def test_fault_blocks_builder():
    model = load("modelA2.og_strm")
    check(model.nb_fault_blocks() == 0, "modelA2 should have no FaultBlock")
    check(model.nb_faults() > 0, "modelA2 should have Faults")
    check(
        model.faults()[0].component_id().type.get() == FAULT_TYPE,
        "Fault3D ComponentType name",
    )
    explicit.build_structural_model_fault_blocks(model)
    check(model.nb_fault_blocks() == 3, "Number of FaultBlocks should be 3")
    check(
        model.fault_blocks()[0].component_id().type.get() == FAULT_BLOCK_TYPE,
        "FaultBlock3D ComponentType name",
    )
    check_fault_blocks(model, [4, 4, 4])


def test_model_without_faults():
    model = load("vri2.og_strm")
    check(model.nb_faults() == 0, "vri2 should have no Fault")
    explicit.build_structural_model_fault_blocks(structural_model=model)
    check(
        model.nb_fault_blocks() == 1,
        "Number of FaultBlocks in a model without Fault should be 1",
    )
    check_fault_blocks(model, [2])


def test_rebuild_next_to_existing_ones():
    model = load("modelA2.og_strm")
    explicit.build_structural_model_fault_blocks(model)
    old_fault_blocks = [fault_block.id() for fault_block in model.fault_blocks()]
    explicit.build_structural_model_fault_blocks(model)
    check(
        model.nb_fault_blocks() == 6,
        "Rebuilding should create a full new set of 3 FaultBlocks",
    )
    for fault_block_id in old_fault_blocks:
        check(
            model.has_fault_block(fault_block_id),
            "The existing FaultBlocks should be kept",
        )
    for block in model.blocks():
        check(
            len(collections_of_type(model, block.id(), FAULT_BLOCK_TYPE)) == 2,
            "Block " + block.id().string() + " should be in an old and a new "
            "FaultBlock",
        )
    builder = explicit.StructuralModelBuilder(model)
    for fault_block_id in old_fault_blocks:
        builder.remove_fault_block(model.fault_block(fault_block_id))
    check_fault_blocks(model, [4, 4, 4])


def test_empty_model():
    model = explicit.StructuralModel()
    explicit.build_structural_model_fault_blocks(model)
    check(model.nb_blocks() == 0, "An empty model should stay empty")


def test_wrong_argument():
    try:
        explicit.build_structural_model_fault_blocks(opengeode.BRep())
    except TypeError:
        return
    raise ValueError("[Test] A BRep should not be accepted")


def test_docstring():
    doc = explicit.build_structural_model_fault_blocks.__doc__ or ""
    for text in (
        "structural_model",
        "Blocks sharing a Surface which is not a Fault",
        "single FaultBlock",
        "Existing FaultBlocks are kept",
        "full new set",
    ):
        check(text in doc, "Docstring should contain '" + text + "': " + doc)


if __name__ == "__main__":
    explicit.OpenGeodeGeosciencesExplicitLibrary.initialize()
    test_fault_blocks_builder()
    test_model_without_faults()
    test_rebuild_next_to_existing_ones()
    test_empty_model()
    test_wrong_argument()
    test_docstring()
