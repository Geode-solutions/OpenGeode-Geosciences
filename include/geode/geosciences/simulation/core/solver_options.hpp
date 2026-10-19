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

#include <map>
#include <optional>
#include <string>
#include <string_view>

#include <geode/geosciences/simulation/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    /*!
     * Simulator-specific numerical settings, stored as text per simulator
     * (e.g. simulator "geos", key "nonlinear/newtonTol", value "1e-6").
     * Each simulator writer documents its keys.
     */
    class opengeode_geosciences_simulation_api SolverOptions
    {
    public:
        void set( std::string_view simulator,
            std::string_view key,
            std::string_view value );

        [[nodiscard]] std::optional< std::string_view > get(
            std::string_view simulator, std::string_view key ) const;

        /// Options of the simulator, sorted by key (empty if none)
        [[nodiscard]] const std::map< std::string, std::string, std::less<> >&
            options( std::string_view simulator ) const;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        std::map< std::string,
            std::map< std::string, std::string, std::less<> >,
            std::less<> >
            options_;
    };
} // namespace geode
