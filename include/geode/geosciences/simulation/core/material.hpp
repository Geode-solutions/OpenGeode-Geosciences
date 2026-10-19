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

#include <array>
#include <memory>
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
    class RelativePermeability;
    class CapillaryPressure;
} // namespace geode

namespace geode
{
    struct opengeode_geosciences_simulation_api RockThermalProperties
    {
        /// J/(m3.K)
        double volumetric_heat_capacity{ 0 };
        /// W/(m.K), along x, y and z
        std::array< double, 3 > thermal_conductivity{ 0, 0, 0 };

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    /*!
     * Rock constants of model regions. Spatially varying porosity and
     * permeability are read from the model PhysicalProperties; the constants
     * here are used only where no such property exists.
     */
    struct opengeode_geosciences_simulation_api Material
    {
        std::string name;
        /// Model components or collections (Blocks, StratigraphicUnits...).
        /// Empty means every region without another material.
        std::vector< uuid > regions;
        /// 1/Pa
        double pore_compressibility{ 0 };
        /// Pa
        double reference_pressure{ 0 };
        std::optional< double > porosity;
        /// m2, along x, y and z
        std::optional< std::array< double, 3 > > permeability;
        /// Shared, must not be modified once given to a PhysicalProblem
        std::shared_ptr< RelativePermeability > relative_permeability;
        /// Optional, shared, must not be modified once given to a
        /// PhysicalProblem
        std::shared_ptr< CapillaryPressure > capillary_pressure;
        std::optional< RockThermalProperties > thermal;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
} // namespace geode
