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

#include <geode/geosciences/simulation/core/initial_condition.hpp>

#include <utility>

#include <bitsery/ext/std_optional.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

namespace geode
{
    InitialCondition::InitialCondition( std::vector< uuid > regions )
        : regions_( std::move( regions ) )
    {
    }

    std::string_view UniformInitialCondition::type_name() const
    {
        return type_name_static();
    }

    UniformInitialCondition::UniformInitialCondition() = default;

    UniformInitialCondition::UniformInitialCondition(
        std::vector< uuid > regions,
        double pressure,
        std::optional< double > temperature,
        std::vector< double > composition )
        : InitialCondition( std::move( regions ) ),
          pressure_( pressure ),
          temperature_( temperature ),
          composition_( std::move( composition ) )
    {
    }

    InitialCondition::InitialCondition() = default;
    InitialCondition::InitialCondition( const InitialCondition& ) = default;
    InitialCondition::InitialCondition( InitialCondition&& ) noexcept = default;
    InitialCondition& InitialCondition::operator=(
        const InitialCondition& ) = default;
    InitialCondition& InitialCondition::operator=(
        InitialCondition&& ) noexcept = default;
    InitialCondition::~InitialCondition() = default;

    template < typename Archive >
    void InitialCondition::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, InitialCondition >{
                { []( Archive& local_archive, InitialCondition& condition ) {
                    local_archive.container( condition.regions_,
                        condition.regions_.max_size(),
                        []( Archive& region_archive, uuid& region ) {
                            region_archive.object( region );
                        } );
                } } } );
    }

    template < typename Archive >
    void UniformInitialCondition::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, UniformInitialCondition >{
                       { []( Archive& local_archive,
                             UniformInitialCondition& condition ) {
                           local_archive.ext( condition,
                               bitsery::ext::BaseClass< InitialCondition >{} );
                           local_archive.value8b( condition.pressure_ );
                           local_archive.ext( condition.temperature_,
                               bitsery::ext::StdOptional{},
                               []( Archive& value_archive,
                                   double& temperature_value ) {
                                   value_archive.value8b( temperature_value );
                               } );
                           local_archive.container8b( condition.composition_,
                               condition.composition_.max_size() );
                       } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, InitialCondition );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, UniformInitialCondition );
} // namespace geode
