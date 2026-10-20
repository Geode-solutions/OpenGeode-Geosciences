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

#include <geode/geosciences/simulation/core/saturation_functions.hpp>

#include <utility>

#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

namespace geode
{
    RelativePermeability::RelativePermeability( std::vector< PHASE > phases )
        : phases_( std::move( phases ) )
    {
    }

    BrooksCoreyRelativePermeability::BrooksCoreyRelativePermeability() =
        default;

    BrooksCoreyRelativePermeability::BrooksCoreyRelativePermeability(
        std::vector< PHASE > phases,
        std::vector< double > residual_saturations,
        std::vector< double > exponents,
        std::vector< double > max_values )
        : RelativePermeability( std::move( phases ) ),
          residual_saturations_( std::move( residual_saturations ) ),
          exponents_( std::move( exponents ) ),
          max_values_( std::move( max_values ) )
    {
    }

    TableRelativePermeability::TableRelativePermeability() = default;

    TableRelativePermeability::TableRelativePermeability(
        std::vector< PHASE > phases,
        std::vector< TwoPhaseRelativePermeabilityTable > tables )
        : RelativePermeability( std::move( phases ) ),
          tables_( std::move( tables ) )
    {
    }

    CapillaryPressure::CapillaryPressure( std::vector< PHASE > phases )
        : phases_( std::move( phases ) )
    {
    }

    BrooksCoreyCapillaryPressure::BrooksCoreyCapillaryPressure() = default;

    BrooksCoreyCapillaryPressure::BrooksCoreyCapillaryPressure(
        std::vector< PHASE > phases,
        std::vector< double > residual_saturations,
        std::vector< double > entry_pressures,
        std::vector< double > pore_size_distribution_indices )
        : CapillaryPressure( std::move( phases ) ),
          residual_saturations_( std::move( residual_saturations ) ),
          entry_pressures_( std::move( entry_pressures ) ),
          pore_size_distribution_indices_(
              std::move( pore_size_distribution_indices ) )
    {
    }

    TableCapillaryPressure::TableCapillaryPressure() = default;

    TableCapillaryPressure::TableCapillaryPressure( std::vector< PHASE > phases,
        std::vector< TwoPhaseCapillaryPressureTable > tables )
        : CapillaryPressure( std::move( phases ) ),
          tables_( std::move( tables ) )
    {
    }

    template < typename Archive >
    void SaturationTable::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, SaturationTable >{
                       { []( Archive& local_archive, SaturationTable& table ) {
                           local_archive.container8b( table.saturations,
                               table.saturations.max_size() );
                           local_archive.container8b(
                               table.values, table.values.max_size() );
                       } } } );
    }

    RelativePermeability::RelativePermeability() = default;
    RelativePermeability::RelativePermeability(
        const RelativePermeability& ) = default;
    RelativePermeability::RelativePermeability(
        RelativePermeability&& ) noexcept = default;
    RelativePermeability& RelativePermeability::operator=(
        const RelativePermeability& ) = default;
    RelativePermeability& RelativePermeability::operator=(
        RelativePermeability&& ) noexcept = default;
    RelativePermeability::~RelativePermeability() = default;

    template < typename Archive >
    void RelativePermeability::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, RelativePermeability >{
                { []( Archive& local_archive, RelativePermeability& relperm ) {
                    local_archive.container4b(
                        relperm.phases_, relperm.phases_.max_size() );
                } } } );
    }

    template < typename Archive >
    void BrooksCoreyRelativePermeability::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, BrooksCoreyRelativePermeability >{
                { []( Archive& local_archive,
                      BrooksCoreyRelativePermeability& relperm ) {
                    local_archive.ext( relperm,
                        bitsery::ext::BaseClass< RelativePermeability >{} );
                    local_archive.container8b( relperm.residual_saturations_,
                        relperm.residual_saturations_.max_size() );
                    local_archive.container8b(
                        relperm.exponents_, relperm.exponents_.max_size() );
                    local_archive.container8b(
                        relperm.max_values_, relperm.max_values_.max_size() );
                } } } );
    }

    template < typename Archive >
    void TwoPhaseRelativePermeabilityTable::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, TwoPhaseRelativePermeabilityTable >{
                       { []( Archive& local_archive,
                             TwoPhaseRelativePermeabilityTable& table ) {
                           local_archive.value4b( table.wetting_phase );
                           local_archive.value4b( table.non_wetting_phase );
                           local_archive.object( table.wetting_kr );
                           local_archive.object( table.non_wetting_kr );
                       } } } );
    }

    template < typename Archive >
    void TableRelativePermeability::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, TableRelativePermeability >{
                { []( Archive& local_archive,
                      TableRelativePermeability& relperm ) {
                    local_archive.ext( relperm,
                        bitsery::ext::BaseClass< RelativePermeability >{} );
                    local_archive.container( relperm.tables_,
                        relperm.tables_.max_size(),
                        []( Archive& table_archive,
                            TwoPhaseRelativePermeabilityTable& table ) {
                            table_archive.object( table );
                        } );
                } } } );
    }

    CapillaryPressure::CapillaryPressure() = default;
    CapillaryPressure::CapillaryPressure( const CapillaryPressure& ) = default;
    CapillaryPressure::CapillaryPressure(
        CapillaryPressure&& ) noexcept = default;
    CapillaryPressure& CapillaryPressure::operator=(
        const CapillaryPressure& ) = default;
    CapillaryPressure& CapillaryPressure::operator=(
        CapillaryPressure&& ) noexcept = default;
    CapillaryPressure::~CapillaryPressure() = default;

    template < typename Archive >
    void CapillaryPressure::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, CapillaryPressure >{
                { []( Archive& local_archive, CapillaryPressure& capillary ) {
                    local_archive.container4b(
                        capillary.phases_, capillary.phases_.max_size() );
                } } } );
    }

    template < typename Archive >
    void BrooksCoreyCapillaryPressure::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, BrooksCoreyCapillaryPressure >{
                { []( Archive& local_archive,
                      BrooksCoreyCapillaryPressure& capillary ) {
                    local_archive.ext( capillary,
                        bitsery::ext::BaseClass< CapillaryPressure >{} );
                    local_archive.container8b( capillary.residual_saturations_,
                        capillary.residual_saturations_.max_size() );
                    local_archive.container8b( capillary.entry_pressures_,
                        capillary.entry_pressures_.max_size() );
                    local_archive.container8b(
                        capillary.pore_size_distribution_indices_,
                        capillary.pore_size_distribution_indices_.max_size() );
                } } } );
    }

    template < typename Archive >
    void TwoPhaseCapillaryPressureTable::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, TwoPhaseCapillaryPressureTable >{
                       { []( Archive& local_archive,
                             TwoPhaseCapillaryPressureTable& table ) {
                           local_archive.value4b( table.wetting_phase );
                           local_archive.value4b( table.non_wetting_phase );
                           local_archive.object( table.pressure );
                       } } } );
    }

    template < typename Archive >
    void TableCapillaryPressure::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, TableCapillaryPressure >{
                       { []( Archive& local_archive,
                             TableCapillaryPressure& capillary ) {
                           local_archive.ext( capillary,
                               bitsery::ext::BaseClass< CapillaryPressure >{} );
                           local_archive.container( capillary.tables_,
                               capillary.tables_.max_size(),
                               []( Archive& table_archive,
                                   TwoPhaseCapillaryPressureTable& table ) {
                                   table_archive.object( table );
                               } );
                       } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, RelativePermeability );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, BrooksCoreyRelativePermeability );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, TableRelativePermeability );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, CapillaryPressure );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, BrooksCoreyCapillaryPressure );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, TableCapillaryPressure );
} // namespace geode
