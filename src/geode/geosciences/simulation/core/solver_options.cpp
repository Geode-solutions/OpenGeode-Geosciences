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

#include <geode/geosciences/simulation/core/solver_options.hpp>

#include <bitsery/ext/std_map.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

namespace
{
    using SimulatorOptions = std::map< std::string, std::string, std::less<> >;
} // namespace

namespace geode
{
    void SolverOptions::set( std::string_view simulator,
        std::string_view key,
        std::string_view value )
    {
        auto simulator_it = options_.find( simulator );
        if( simulator_it == options_.end() )
        {
            simulator_it =
                options_.emplace( std::string{ simulator }, SimulatorOptions{} )
                    .first;
        }
        simulator_it->second.insert_or_assign(
            std::string{ key }, std::string{ value } );
    }

    std::optional< std::string_view > SolverOptions::get(
        std::string_view simulator, std::string_view key ) const
    {
        const auto& simulator_options = options( simulator );
        const auto option_it = simulator_options.find( key );
        if( option_it == simulator_options.end() )
        {
            return std::nullopt;
        }
        return option_it->second;
    }

    const SimulatorOptions& SolverOptions::options(
        std::string_view simulator ) const
    {
        static const SimulatorOptions no_options;
        const auto simulator_it = options_.find( simulator );
        if( simulator_it == options_.end() )
        {
            return no_options;
        }
        return simulator_it->second;
    }

    template < typename Archive >
    void SolverOptions::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, SolverOptions >{ { []( Archive& local_archive,
                                                      SolverOptions& solver ) {
                local_archive.ext( solver.options_,
                    bitsery::ext::StdMap{ solver.options_.max_size() },
                    []( Archive& simulator_archive, std::string& simulator,
                        SimulatorOptions& simulator_options ) {
                        simulator_archive.text1b(
                            simulator, simulator.max_size() );
                        simulator_archive.ext( simulator_options,
                            bitsery::ext::StdMap{
                                simulator_options.max_size() },
                            []( Archive& option_archive, std::string& key,
                                std::string& value ) {
                                option_archive.text1b( key, key.max_size() );
                                option_archive.text1b(
                                    value, value.max_size() );
                            } );
                    } );
            } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, SolverOptions );
} // namespace geode
