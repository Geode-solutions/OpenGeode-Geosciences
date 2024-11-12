/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/explicit/mixin/builder/fault_blocks_builder.hpp>

#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_blocks.hpp>

namespace geode
{
    template < index_t dimension >
    const uuid& FaultBlocksBuilder< dimension >::create_fault_block()
    {
        return fault_blocks_.create_fault_block( {} );
    }

    template < index_t dimension >
    void FaultBlocksBuilder< dimension >::create_fault_block(
        uuid fault_block_id )
    {
        fault_blocks_.create_fault_block( std::move( fault_block_id ), {} );
    }

    template < index_t dimension >
    void FaultBlocksBuilder< dimension >::delete_fault_block(
        const FaultBlock< dimension >& fault_block )
    {
        fault_blocks_.delete_fault_block( fault_block, {} );
    }

    template < index_t dimension >
    void FaultBlocksBuilder< dimension >::load_fault_blocks(
        std::string_view directory )
    {
        return fault_blocks_.load_fault_blocks( directory, {} );
    }

    template < index_t dimension >
    void FaultBlocksBuilder< dimension >::set_fault_block_name(
        const uuid& id, std::string_view name )
    {
        fault_blocks_.modifiable_fault_block( id, {} ).set_fault_block_name(
            name, typename FaultBlock< dimension >::FaultBlocksBuilderKey{} );
    }

    template class opengeode_geosciences_explicit_api FaultBlocksBuilder< 2 >;
    template class opengeode_geosciences_explicit_api FaultBlocksBuilder< 3 >;
} // namespace geode
