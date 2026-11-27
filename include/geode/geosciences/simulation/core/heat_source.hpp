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
#include <string>
#include <vector>

#include <geode/basic/uuid.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    /*!
     * Heat released in the rock, in W (positive heats, negative cools), for
     * thermal problems. It is located either in a Block, where it is spread
     * uniformly over the Block volume, or at a named set of points given to
     * the simulator exporter, like wells.
     */
    struct opengeode_geosciences_simulation_api HeatSource
    {
        /// Unique name; the exporters locate a point heat source by this name
        std::string name;
        /// Block releasing the heat; empty for a point heat source
        std::optional< uuid > block;
        /// s, strictly increasing; empty for a constant power
        std::vector< double > times;
        /// W; one value if times is empty, otherwise one per time: linear
        /// between times, held before the first and after the last
        std::vector< double > powers;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
} // namespace geode
