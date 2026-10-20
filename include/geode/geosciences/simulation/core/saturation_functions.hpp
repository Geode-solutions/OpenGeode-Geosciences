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

#include <string_view>
#include <vector>

#include <absl/types/span.h>

#include <geode/geosciences/simulation/common.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    /// Sampled curve f(S), linearly interpolated
    struct opengeode_geosciences_simulation_api SaturationTable
    {
        /// Strictly increasing, in [0, 1]
        std::vector< double > saturations;
        std::vector< double > values;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    /*!
     * Relative permeability law. Leaf classes must never be renamed: their
     * type name is written in saved files.
     */
    class opengeode_geosciences_simulation_api RelativePermeability
    {
    public:
        virtual ~RelativePermeability();

        [[nodiscard]] virtual std::string_view type_name() const = 0;

        /// Order gives the phase index of every per-phase vector
        [[nodiscard]] absl::Span< const PHASE > phases() const
        {
            return phases_;
        }

    protected:
        RelativePermeability();
        explicit RelativePermeability( std::vector< PHASE > phases );
        RelativePermeability( const RelativePermeability& );
        RelativePermeability( RelativePermeability&& ) noexcept;
        RelativePermeability& operator=( const RelativePermeability& );
        RelativePermeability& operator=( RelativePermeability&& ) noexcept;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        friend class bitsery::Access;
        std::vector< PHASE > phases_;
    };

    class opengeode_geosciences_simulation_api BrooksCoreyRelativePermeability
        final : public RelativePermeability
    {
    public:
        BrooksCoreyRelativePermeability();
        /// One value per phase in each vector, in the phase order
        BrooksCoreyRelativePermeability( std::vector< PHASE > phases,
            std::vector< double > residual_saturations,
            std::vector< double > exponents,
            std::vector< double > max_values );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "BrooksCoreyRelativePermeability";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

        /// One value per phase
        [[nodiscard]] absl::Span< const double > residual_saturations() const
        {
            return residual_saturations_;
        }

        [[nodiscard]] absl::Span< const double > exponents() const
        {
            return exponents_;
        }

        [[nodiscard]] absl::Span< const double > max_values() const
        {
            return max_values_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        std::vector< double > residual_saturations_;
        std::vector< double > exponents_;
        std::vector< double > max_values_;
    };

    /// Two-phase relative permeability curves (SWOF/SGOF-like)
    struct
        opengeode_geosciences_simulation_api TwoPhaseRelativePermeabilityTable
    {
        PHASE wetting_phase{ PHASE::water };
        PHASE non_wetting_phase{ PHASE::oil };
        /// kr of the wetting phase versus its saturation
        SaturationTable wetting_kr;
        /// kr of the non-wetting phase versus its saturation
        SaturationTable non_wetting_kr;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    class opengeode_geosciences_simulation_api TableRelativePermeability final
        : public RelativePermeability
    {
    public:
        TableRelativePermeability();
        /// One table for two phases, two (water-oil and gas-oil) for three
        TableRelativePermeability( std::vector< PHASE > phases,
            std::vector< TwoPhaseRelativePermeabilityTable > tables );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "TableRelativePermeability";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

        /// One table for two phases, two (water-oil and gas-oil) for three
        [[nodiscard]] absl::Span< const TwoPhaseRelativePermeabilityTable >
            tables() const
        {
            return tables_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        std::vector< TwoPhaseRelativePermeabilityTable > tables_;
    };

    /*!
     * Capillary pressure law. Leaf classes must never be renamed: their type
     * name is written in saved files.
     */
    class opengeode_geosciences_simulation_api CapillaryPressure
    {
    public:
        virtual ~CapillaryPressure();

        [[nodiscard]] virtual std::string_view type_name() const = 0;

        /// Order gives the phase index of every per-phase vector
        [[nodiscard]] absl::Span< const PHASE > phases() const
        {
            return phases_;
        }

    protected:
        CapillaryPressure();
        explicit CapillaryPressure( std::vector< PHASE > phases );
        CapillaryPressure( const CapillaryPressure& );
        CapillaryPressure( CapillaryPressure&& ) noexcept;
        CapillaryPressure& operator=( const CapillaryPressure& );
        CapillaryPressure& operator=( CapillaryPressure&& ) noexcept;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        friend class bitsery::Access;
        std::vector< PHASE > phases_;
    };

    /// Pc = entry_pressure * Se^(-1/lambda)
    class opengeode_geosciences_simulation_api BrooksCoreyCapillaryPressure
        final : public CapillaryPressure
    {
    public:
        BrooksCoreyCapillaryPressure();
        /*!
         * One value per phase in each vector, in the phase order.
         * entry_pressures in Pa, pore_size_distribution_indices is lambda.
         */
        BrooksCoreyCapillaryPressure( std::vector< PHASE > phases,
            std::vector< double > residual_saturations,
            std::vector< double > entry_pressures,
            std::vector< double > pore_size_distribution_indices );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "BrooksCoreyCapillaryPressure";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

        /// One value per phase
        [[nodiscard]] absl::Span< const double > residual_saturations() const
        {
            return residual_saturations_;
        }

        /// Pa
        [[nodiscard]] absl::Span< const double > entry_pressures() const
        {
            return entry_pressures_;
        }

        /// lambda
        [[nodiscard]] absl::Span< const double >
            pore_size_distribution_indices() const
        {
            return pore_size_distribution_indices_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        std::vector< double > residual_saturations_;
        std::vector< double > entry_pressures_;
        std::vector< double > pore_size_distribution_indices_;
    };

    /// Capillary pressure (Pa) versus wetting phase saturation
    struct opengeode_geosciences_simulation_api TwoPhaseCapillaryPressureTable
    {
        PHASE wetting_phase{ PHASE::water };
        PHASE non_wetting_phase{ PHASE::oil };
        SaturationTable pressure;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    class opengeode_geosciences_simulation_api TableCapillaryPressure final
        : public CapillaryPressure
    {
    public:
        TableCapillaryPressure();
        TableCapillaryPressure( std::vector< PHASE > phases,
            std::vector< TwoPhaseCapillaryPressureTable > tables );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "TableCapillaryPressure";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

        [[nodiscard]] absl::Span< const TwoPhaseCapillaryPressureTable >
            tables() const
        {
            return tables_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        std::vector< TwoPhaseCapillaryPressureTable > tables_;
    };
} // namespace geode
