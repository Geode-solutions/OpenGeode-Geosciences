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
#include <string_view>
#include <vector>

#include <absl/types/span.h>

#include <geode/basic/uuid.hpp>

#include <geode/geosciences/simulation/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    /*!
     * Initial state of model regions. Leaf classes must never be renamed:
     * their type name is written in saved files.
     */
    class opengeode_geosciences_simulation_api InitialCondition
    {
    public:
        virtual ~InitialCondition();

        [[nodiscard]] virtual std::string_view type_name() const = 0;

        /// Model components or collections. Empty means every region without
        /// another initial condition.
        [[nodiscard]] absl::Span< const uuid > regions() const
        {
            return regions_;
        }

    protected:
        InitialCondition();
        explicit InitialCondition( std::vector< uuid > regions );
        InitialCondition( const InitialCondition& );
        InitialCondition( InitialCondition&& ) noexcept;
        InitialCondition& operator=( const InitialCondition& );
        InitialCondition& operator=( InitialCondition&& ) noexcept;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        friend class bitsery::Access;
        std::vector< uuid > regions_;
    };

    class opengeode_geosciences_simulation_api UniformInitialCondition final
        : public InitialCondition
    {
    public:
        UniformInitialCondition();
        /*!
         * Empty regions means every region without another initial condition.
         * pressure in Pa, temperature in K, composition as global fractions
         * in the fluid component order.
         */
        UniformInitialCondition( std::vector< uuid > regions,
            double pressure,
            std::optional< double > temperature = std::nullopt,
            std::vector< double > composition = {} );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "UniformInitialCondition";
        }

        [[nodiscard]] std::string_view type_name() const final;

        /// Pa
        [[nodiscard]] double pressure() const
        {
            return pressure_;
        }

        /// K
        [[nodiscard]] const std::optional< double >& temperature() const
        {
            return temperature_;
        }

        /// Global fractions, in the fluid component order
        [[nodiscard]] absl::Span< const double > composition() const
        {
            return composition_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        double pressure_{ 0 };
        std::optional< double > temperature_;
        std::vector< double > composition_;
    };
} // namespace geode
