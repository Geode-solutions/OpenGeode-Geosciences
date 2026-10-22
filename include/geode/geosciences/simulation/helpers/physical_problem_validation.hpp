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

#include <string>
#include <vector>

#include <geode/geosciences/simulation/common.hpp>

namespace geode
{
    class BRep;
    class PhysicalProblem;
} // namespace geode

namespace geode
{
    /*!
     * Check the problem consistency, independently of any simulator and
     * model. Returns one message per issue, empty if the problem is valid.
     * Only the content of the leaf classes defined in this library is
     * checked: leaf classes registered by other libraries are accepted as is.
     */
    [[nodiscard]] std::vector< std::string >
        opengeode_geosciences_simulation_api validate_physical_problem(
            const PhysicalProblem& problem );

    /*!
     * Check that every region referenced by the problem is a Block or a
     * collection of Blocks of the model (BlockCollection,
     * StratigraphicUnit...), that no Block is covered by two materials or two
     * initial conditions, and that the recorded model id, if any, matches.
     * Returns one message per issue, empty if valid.
     */
    [[nodiscard]] std::vector< std::string >
        opengeode_geosciences_simulation_api validate_physical_problem_regions(
            const PhysicalProblem& problem, const BRep& model );
} // namespace geode
