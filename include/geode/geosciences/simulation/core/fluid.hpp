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
#include <string_view>
#include <vector>

#include <absl/types/span.h>

#include <geode/geosciences/simulation/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    enum struct PHASE
    {
        oil,
        gas,
        water
    };

    [[nodiscard]] std::string_view opengeode_geosciences_simulation_api
        phase_name( PHASE phase );

    struct opengeode_geosciences_simulation_api FluidComponent
    {
        std::string name;
        /// kg/mol
        double molar_weight{ 0 };

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    struct opengeode_geosciences_simulation_api FluidThermalProperties
    {
        /// J/(kg.K)
        double specific_heat_capacity{ 0 };
        /// 1/K
        double thermal_expansion{ 0 };
        /// 1/K
        double viscosity_temperature_coefficient{ 0 };

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    /*!
     * Fluid description shared by every simulator.
     * Leaf classes must never be renamed: their type name is written in
     * saved files.
     */
    class opengeode_geosciences_simulation_api FluidModel
    {
    public:
        virtual ~FluidModel();

        [[nodiscard]] virtual std::string_view type_name() const = 0;

        /// Order gives the phase index of every per-phase vector
        [[nodiscard]] absl::Span< const PHASE > phases() const
        {
            return phases_;
        }

        /// Order gives the component index of every composition vector
        [[nodiscard]] absl::Span< const FluidComponent > components() const
        {
            return components_;
        }

    protected:
        FluidModel();
        FluidModel( std::vector< PHASE > phases,
            std::vector< FluidComponent > components );
        FluidModel( const FluidModel& );
        FluidModel( FluidModel&& ) noexcept;
        FluidModel& operator=( const FluidModel& );
        FluidModel& operator=( FluidModel&& ) noexcept;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        friend class bitsery::Access;
        std::vector< PHASE > phases_;
        std::vector< FluidComponent > components_;
    };

    /*!
     * Component carried by a phase. It is defined when the fluid has one
     * component per phase, in the phase order (e.g. dead oil or CO2-brine).
     * Simulator writers use it to translate phase-based data, such as boundary
     * fluid fractions, into component indices.
     */
    [[nodiscard]] std::optional< index_t > opengeode_geosciences_simulation_api
        phase_component( const FluidModel& fluid, PHASE phase );

    class opengeode_geosciences_simulation_api SinglePhaseFluid final
        : public FluidModel
    {
    public:
        SinglePhaseFluid();
        /*!
         * Units: reference_density in kg/m3, reference_viscosity in Pa.s,
         * compressibility and viscosibility in 1/Pa, reference_pressure in Pa
         */
        SinglePhaseFluid( std::vector< PHASE > phases,
            std::vector< FluidComponent > components,
            double reference_density,
            double reference_viscosity,
            double compressibility,
            double viscosibility,
            double reference_pressure,
            std::optional< FluidThermalProperties > thermal = std::nullopt );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "SinglePhaseFluid";
        }

        [[nodiscard]] std::string_view type_name() const final;

        /// kg/m3
        [[nodiscard]] double reference_density() const
        {
            return reference_density_;
        }

        /// Pa.s
        [[nodiscard]] double reference_viscosity() const
        {
            return reference_viscosity_;
        }

        /// 1/Pa
        [[nodiscard]] double compressibility() const
        {
            return compressibility_;
        }

        /// 1/Pa
        [[nodiscard]] double viscosibility() const
        {
            return viscosibility_;
        }

        /// Pa
        [[nodiscard]] double reference_pressure() const
        {
            return reference_pressure_;
        }

        [[nodiscard]] const std::optional< FluidThermalProperties >&
            thermal() const
        {
            return thermal_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        double reference_density_{ 0 };
        double reference_viscosity_{ 0 };
        double compressibility_{ 0 };
        double viscosibility_{ 0 };
        double reference_pressure_{ 0 };
        std::optional< FluidThermalProperties > thermal_;
    };

    /// Formation volume factor and viscosity of a hydrocarbon phase versus
    /// pressure (PVDO/PVDG-like table)
    struct opengeode_geosciences_simulation_api HydrocarbonPvtTable
    {
        PHASE phase{ PHASE::oil };
        /// Pa, strictly increasing
        std::vector< double > pressures;
        std::vector< double > formation_volume_factors;
        /// Pa.s
        std::vector< double > viscosities;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    /// Water properties (PVTW-like)
    struct opengeode_geosciences_simulation_api WaterPvt
    {
        /// Pa
        double reference_pressure{ 0 };
        double formation_volume_factor{ 1 };
        /// 1/Pa
        double compressibility{ 0 };
        /// Pa.s
        double viscosity{ 0 };

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    class opengeode_geosciences_simulation_api DeadOilFluid final
        : public FluidModel
    {
    public:
        DeadOilFluid();
        /*!
         * Two or three phases. surface_densities in kg/m3, one per phase.
         * One hydrocarbon PVT table per oil/gas phase. water_pvt is required
         * if and only if water is a phase.
         */
        DeadOilFluid( std::vector< PHASE > phases,
            std::vector< FluidComponent > components,
            std::vector< double > surface_densities,
            std::vector< HydrocarbonPvtTable > hydrocarbon_pvt,
            std::optional< WaterPvt > water_pvt = std::nullopt );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "DeadOilFluid";
        }

        [[nodiscard]] std::string_view type_name() const final;

        /// kg/m3, one per phase
        [[nodiscard]] absl::Span< const double > surface_densities() const
        {
            return surface_densities_;
        }

        /// One per oil/gas phase
        [[nodiscard]] absl::Span< const HydrocarbonPvtTable >
            hydrocarbon_pvt() const
        {
            return hydrocarbon_pvt_;
        }

        /// Defined if and only if water is a phase
        [[nodiscard]] const std::optional< WaterPvt >& water_pvt() const
        {
            return water_pvt_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        std::vector< double > surface_densities_;
        std::vector< HydrocarbonPvtTable > hydrocarbon_pvt_;
        std::optional< WaterPvt > water_pvt_;
    };

    enum struct CO2_BRINE_CORRELATION
    {
        phillips,
        ezrokhi
    };

    struct opengeode_geosciences_simulation_api ValidityRange
    {
        double min{ 0 };
        double max{ 0 };
        double step{ 0 };

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    /*!
     * CO2-brine fluid computed from correlations. Thermal behavior is part of
     * the correlation: writers select the thermal variant when the problem is
     * thermal.
     */
    class opengeode_geosciences_simulation_api Co2BrineFluid final
        : public FluidModel
    {
    public:
        Co2BrineFluid();
        /// salinity in mol/kg, pressure_range in Pa, temperature_range in K
        Co2BrineFluid( std::vector< PHASE > phases,
            std::vector< FluidComponent > components,
            CO2_BRINE_CORRELATION correlation,
            double salinity,
            ValidityRange pressure_range,
            ValidityRange temperature_range,
            std::vector< double > ezrokhi_density_coefficients = {},
            std::vector< double > ezrokhi_viscosity_coefficients = {} );

        [[nodiscard]] static std::string_view type_name_static()
        {
            return "Co2BrineFluid";
        }

        [[nodiscard]] std::string_view type_name() const final;

        [[nodiscard]] CO2_BRINE_CORRELATION correlation() const
        {
            return correlation_;
        }

        /// mol/kg
        [[nodiscard]] double salinity() const
        {
            return salinity_;
        }

        /// Pa
        [[nodiscard]] const ValidityRange& pressure_range() const
        {
            return pressure_range_;
        }

        /// K
        [[nodiscard]] const ValidityRange& temperature_range() const
        {
            return temperature_range_;
        }

        [[nodiscard]] absl::Span< const double >
            ezrokhi_density_coefficients() const
        {
            return ezrokhi_density_coefficients_;
        }

        [[nodiscard]] absl::Span< const double >
            ezrokhi_viscosity_coefficients() const
        {
            return ezrokhi_viscosity_coefficients_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        CO2_BRINE_CORRELATION correlation_{ CO2_BRINE_CORRELATION::phillips };
        double salinity_{ 0 };
        ValidityRange pressure_range_;
        ValidityRange temperature_range_;
        std::vector< double > ezrokhi_density_coefficients_;
        std::vector< double > ezrokhi_viscosity_coefficients_;
    };
} // namespace geode
