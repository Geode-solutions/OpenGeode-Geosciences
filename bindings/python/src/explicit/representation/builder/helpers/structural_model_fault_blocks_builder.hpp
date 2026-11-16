/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <geode/geosciences/explicit/representation/builder/helpers/structural_model_fault_blocks_builder.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>

namespace geode
{
    void define_structural_model_fault_blocks_builder(
        pybind11::module& module )
    {
        module.def( "build_structural_model_fault_blocks",
            &build_structural_model_fault_blocks,
            pybind11::arg( "structural_model" ),
            pybind11::call_guard< pybind11::gil_scoped_release >(),
            "Group the Blocks of the StructuralModel into new FaultBlocks: "
            "Blocks sharing a Surface which is not a Fault are in the same "
            "FaultBlock. If the model has no Fault, all the Blocks are put in "
            "a single FaultBlock. Existing FaultBlocks are kept: a full new "
            "set is created next to them, so callers can compare both sets "
            "(e.g. to detect splits and merges)" );
    }
} // namespace geode
