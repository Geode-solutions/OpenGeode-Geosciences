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

#include <optional>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/uuid.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace geode
{
    class BRep;
    class PhysicalProblem;
} // namespace geode

namespace geode
{
    /*!
     * Blocks of a problem region: the Block itself, or the items of a
     * non-empty collection made only of Blocks (BlockCollection,
     * StratigraphicUnit...). No value if the region is neither.
     */
    [[nodiscard]] std::optional< std::vector< uuid > >
        opengeode_geosciences_simulation_api physical_problem_region_blocks(
            const BRep& model, const uuid& region );

    /*!
     * Material index of each Block. Materials with regions are assigned
     * first (the first one wins); the first material without region then
     * covers every remaining Block. Invalid regions are skipped (they are
     * reported by validate_physical_problem_regions). Blocks covered by no
     * material are not in the result.
     */
    [[nodiscard]] absl::flat_hash_map< uuid, index_t >
        opengeode_geosciences_simulation_api physical_problem_block_materials(
            const PhysicalProblem& problem, const BRep& model );

    /// Initial condition index of each Block, assigned as materials are
    [[nodiscard]] absl::flat_hash_map< uuid, index_t >
        opengeode_geosciences_simulation_api
        physical_problem_block_initial_conditions(
            const PhysicalProblem& problem, const BRep& model );
} // namespace geode
