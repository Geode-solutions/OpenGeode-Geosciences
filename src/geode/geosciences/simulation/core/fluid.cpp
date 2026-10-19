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

#include <geode/geosciences/simulation/core/fluid.hpp>

#include <bitsery/ext/std_optional.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>
#include <geode/basic/range.hpp>

namespace geode
{
    std::string_view phase_name( PHASE phase )
    {
        switch( phase )
        {
            case PHASE::oil:
                return "oil";
            case PHASE::gas:
                return "gas";
            case PHASE::water:
                return "water";
        }
        return "";
    }

    std::optional< index_t > phase_component(
        const FluidModel& fluid, PHASE phase )
    {
        if( fluid.components.size() != fluid.phases.size() )
        {
            return std::nullopt;
        }
        for( const auto phase_id : Indices{ fluid.phases } )
        {
            if( fluid.phases[phase_id] == phase )
            {
                return phase_id;
            }
        }
        return std::nullopt;
    }

    template < typename Archive >
    void FluidComponent::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, FluidComponent >{
                { []( Archive& local_archive, FluidComponent& component ) {
                    local_archive.text1b(
                        component.name, component.name.max_size() );
                    local_archive.value8b( component.molar_weight );
                } } } );
    }

    template < typename Archive >
    void FluidThermalProperties::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, FluidThermalProperties >{
                { []( Archive& local_archive,
                      FluidThermalProperties& properties ) {
                    local_archive.value8b( properties.specific_heat_capacity );
                    local_archive.value8b( properties.thermal_expansion );
                    local_archive.value8b(
                        properties.viscosity_temperature_coefficient );
                } } } );
    }

    FluidModel::FluidModel() = default;
    FluidModel::FluidModel( const FluidModel& ) = default;
    FluidModel::FluidModel( FluidModel&& ) noexcept = default;
    FluidModel& FluidModel::operator=( const FluidModel& ) = default;
    FluidModel& FluidModel::operator=( FluidModel&& ) noexcept = default;
    FluidModel::~FluidModel() = default;

    template < typename Archive >
    void FluidModel::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, FluidModel >{
                       { []( Archive& local_archive, FluidModel& fluid ) {
                           local_archive.container4b(
                               fluid.phases, fluid.phases.max_size() );
                           local_archive.container( fluid.components,
                               fluid.components.max_size(),
                               []( Archive& component_archive,
                                   FluidComponent& component ) {
                                   component_archive.object( component );
                               } );
                       } } } );
    }

    template < typename Archive >
    void SinglePhaseFluid::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, SinglePhaseFluid >{
                { []( Archive& local_archive, SinglePhaseFluid& fluid ) {
                    local_archive.ext(
                        fluid, bitsery::ext::BaseClass< FluidModel >{} );
                    local_archive.value8b( fluid.reference_density );
                    local_archive.value8b( fluid.reference_viscosity );
                    local_archive.value8b( fluid.compressibility );
                    local_archive.value8b( fluid.viscosibility );
                    local_archive.value8b( fluid.reference_pressure );
                    local_archive.ext( fluid.thermal,
                        bitsery::ext::StdOptional{},
                        []( Archive& thermal_archive,
                            FluidThermalProperties& thermal_properties ) {
                            thermal_archive.object( thermal_properties );
                        } );
                } } } );
    }

    template < typename Archive >
    void HydrocarbonPvtTable::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, HydrocarbonPvtTable >{
                { []( Archive& local_archive, HydrocarbonPvtTable& table ) {
                    local_archive.value4b( table.phase );
                    local_archive.container8b(
                        table.pressures, table.pressures.max_size() );
                    local_archive.container8b( table.formation_volume_factors,
                        table.formation_volume_factors.max_size() );
                    local_archive.container8b(
                        table.viscosities, table.viscosities.max_size() );
                } } } );
    }

    template < typename Archive >
    void WaterPvt::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, WaterPvt >{
                { []( Archive& local_archive, WaterPvt& water ) {
                    local_archive.value8b( water.reference_pressure );
                    local_archive.value8b( water.formation_volume_factor );
                    local_archive.value8b( water.compressibility );
                    local_archive.value8b( water.viscosity );
                } } } );
    }

    template < typename Archive >
    void DeadOilFluid::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, DeadOilFluid >{ { []( Archive& local_archive,
                                                     DeadOilFluid& fluid ) {
                local_archive.ext(
                    fluid, bitsery::ext::BaseClass< FluidModel >{} );
                local_archive.container8b( fluid.surface_densities,
                    fluid.surface_densities.max_size() );
                local_archive.container( fluid.hydrocarbon_pvt,
                    fluid.hydrocarbon_pvt.max_size(),
                    []( Archive& table_archive, HydrocarbonPvtTable& table ) {
                        table_archive.object( table );
                    } );
                local_archive.ext( fluid.water_pvt, bitsery::ext::StdOptional{},
                    []( Archive& water_archive, WaterPvt& water ) {
                        water_archive.object( water );
                    } );
            } } } );
    }

    template < typename Archive >
    void ValidityRange::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, ValidityRange >{
                       { []( Archive& local_archive, ValidityRange& range ) {
                           local_archive.value8b( range.min );
                           local_archive.value8b( range.max );
                           local_archive.value8b( range.step );
                       } } } );
    }

    template < typename Archive >
    void Co2BrineFluid::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, Co2BrineFluid >{ { []( Archive& local_archive,
                                                      Co2BrineFluid& fluid ) {
                local_archive.ext(
                    fluid, bitsery::ext::BaseClass< FluidModel >{} );
                local_archive.value4b( fluid.correlation );
                local_archive.value8b( fluid.salinity );
                local_archive.object( fluid.pressure_range );
                local_archive.object( fluid.temperature_range );
                local_archive.container8b( fluid.ezrokhi_density_coefficients,
                    fluid.ezrokhi_density_coefficients.max_size() );
                local_archive.container8b( fluid.ezrokhi_viscosity_coefficients,
                    fluid.ezrokhi_viscosity_coefficients.max_size() );
            } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, FluidModel );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, SinglePhaseFluid );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, DeadOilFluid );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, Co2BrineFluid );
} // namespace geode
