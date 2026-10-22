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

#include <geode/geosciences/simulation/core/schedule.hpp>

#include <bitsery/ext/std_optional.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

namespace geode
{
    template < typename Archive >
    void Schedule::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, Schedule >{
                       { []( Archive& local_archive, Schedule& schedule ) {
                           local_archive.value8b( schedule.start_time );
                           local_archive.value8b( schedule.end_time );
                           local_archive.value8b( schedule.initial_time_step );
                           local_archive.ext( schedule.max_time_step,
                               bitsery::ext::StdOptional{},
                               []( Archive& value_archive, double& time_step ) {
                                   value_archive.value8b( time_step );
                               } );
                       } } } );
    }

    template < typename Archive >
    void OutputSettings::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, OutputSettings >{
                { []( Archive& local_archive, OutputSettings& output ) {
                    local_archive.ext( output.frequency,
                        bitsery::ext::StdOptional{},
                        []( Archive& value_archive, double& frequency_value ) {
                            value_archive.value8b( frequency_value );
                        } );
                    local_archive.container8b(
                        output.times, output.times.max_size() );
                    local_archive.value1b( output.binary );
                    local_archive.value1b( output.restart );
                    local_archive.ext( output.restart_frequency,
                        bitsery::ext::StdOptional{},
                        []( Archive& value_archive, double& frequency_value ) {
                            value_archive.value8b( frequency_value );
                        } );
                } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_geosciences_simulation_api, Schedule );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, OutputSettings );
} // namespace geode
