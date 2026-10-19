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

    public:
        /// Order gives the phase index of every per-phase vector
        std::vector< PHASE > phases;
        /// Order gives the component index of every composition vector
        std::vector< FluidComponent > components;

    protected:
        FluidModel();
        FluidModel( const FluidModel& );
        FluidModel( FluidModel&& ) noexcept;
        FluidModel& operator=( const FluidModel& );
        FluidModel& operator=( FluidModel&& ) noexcept;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        friend class bitsery::Access;
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
        [[nodiscard]] static std::string_view type_name_static()
        {
            return "SinglePhaseFluid";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

    public:
        /// kg/m3
        double reference_density{ 0 };
        /// Pa.s
        double reference_viscosity{ 0 };
        /// 1/Pa
        double compressibility{ 0 };
        /// 1/Pa
        double viscosibility{ 0 };
        /// Pa
        double reference_pressure{ 0 };
        std::optional< FluidThermalProperties > thermal;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
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
        [[nodiscard]] static std::string_view type_name_static()
        {
            return "DeadOilFluid";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

    public:
        /// kg/m3, one per phase
        std::vector< double > surface_densities;
        /// One per oil/gas phase
        std::vector< HydrocarbonPvtTable > hydrocarbon_pvt;
        /// Required if and only if water is a phase
        std::optional< WaterPvt > water_pvt;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
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
        [[nodiscard]] static std::string_view type_name_static()
        {
            return "Co2BrineFluid";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

    public:
        CO2_BRINE_CORRELATION correlation{ CO2_BRINE_CORRELATION::phillips };
        /// mol/kg
        double salinity{ 0 };
        /// Pa
        ValidityRange pressure_range;
        /// K
        ValidityRange temperature_range;
        std::vector< double > ezrokhi_density_coefficients;
        std::vector< double > ezrokhi_viscosity_coefficients;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
} // namespace geode
