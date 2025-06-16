/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
#include <geode/geosciences/explicit/representation/builder/helpers/structural_model_fault_blocks_builder.hpp>

#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <queue>

namespace
{
    std::optional< geode::uuid > is_fault(
        const geode::StructuralModel& structural_model,
        const geode::Surface3D& surface )
    {
        for( const auto& fault : structural_model.faults() )
        {
            for( const auto& fault_surface :
                structural_model.fault_items( fault ) )
            {
                if( fault_surface.id() == surface.id() )
                {
                    return fault.id();
                }
            }
        }
        return std::nullopt;
    }
} // namespace

namespace geode
{
    void build_structural_model_fault_blocks(
        StructuralModel& structural_model )
    {
        StructuralModelBuilder builder( structural_model );
        if( structural_model.nb_faults() == 0 )
        {
            const auto& fault_block =
                structural_model.fault_block( builder.add_fault_block() );
            for( const auto& block : structural_model.blocks() )
            {
                builder.add_block_in_fault_block( block, fault_block );
            }
            return;
        }
        std::queue< uuid > fault_queue;
        std::vector< uuid > visited;
        std::vector< uuid > treated_faults;
        const auto& first_block = *structural_model.blocks().begin();
        fault_queue.push( first_block.id() );
        while( !fault_queue.empty() )
        {
            std::queue< uuid > block_queue;
            block_queue.push( fault_queue.front() );
            visited.push_back( fault_queue.front() );
            fault_queue.pop();
            const auto& fault_block =
                structural_model.fault_block( builder.add_fault_block() );
            while( !block_queue.empty() )
            {
                const auto block_id = block_queue.front();
                block_queue.pop();
                const auto& block = structural_model.block( block_id );
                builder.add_block_in_fault_block( block, fault_block );
                for( const auto& boundary :
                    structural_model.boundaries( block ) )
                {
                    const auto fault_id =
                        is_fault( structural_model, boundary );
                    if( fault_id )
                    {
                        if( absl::c_contains(
                                treated_faults, fault_id.value() ) )
                        {
                            continue;
                        }
                        treated_faults.push_back( fault_id.value() );
                        for( const auto& incident_block :
                            structural_model.incidences( boundary ) )
                        {
                            if( !absl::c_contains(
                                    visited, incident_block.id() )
                                && incident_block.id() != block.id() )
                            {
                                fault_queue.push( incident_block.id() );
                                visited.push_back( incident_block.id() );
                                continue;
                            }
                        }
                        continue;
                    }
                    for( const auto& incident_block :
                        structural_model.incidences( boundary ) )
                    {
                        if( !absl::c_contains( visited, incident_block.id() ) )
                        {
                            block_queue.push( incident_block.id() );
                            visited.push_back( incident_block.id() );
                        }
                    }
                }
            }
        }
    }
} // namespace geode