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

#include <geode/geosciences/simulation/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    /// Simulated time interval, in seconds
    struct opengeode_geosciences_simulation_api Schedule
    {
        double start_time{ 0 };
        double end_time{ 0 };
        double initial_time_step{ 0 };
        std::optional< double > max_time_step;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    /// When results are written, in seconds
    struct opengeode_geosciences_simulation_api OutputSettings
    {
        std::optional< double > frequency;
        std::vector< double > times;
        bool binary{ true };
        /// Writes a restart file at the end of the run
        bool restart{ false };
        /// Writes a restart file periodically, in seconds
        std::optional< double > restart_frequency;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
} // namespace geode
