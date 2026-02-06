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

    void add_all_blocks_to_single_fault_block(
        const geode::StructuralModel& structural_model,
        geode::StructuralModelBuilder& builder )
    {
        const auto& fault_block =
            structural_model.fault_block( builder.add_fault_block() );
        for( const auto& block : structural_model.blocks() )
        {
            builder.add_block_in_fault_block( block, fault_block );
        }
    }

    void add_incident_blocks_to_queue(
        const geode::StructuralModel& structural_model,
        const geode::Surface3D& boundary,
        std::queue< geode::uuid >& block_queue,
        std::vector< geode::uuid >& visited )
    {
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

    void build_fault_blocks( const geode::StructuralModel& structural_model,
        geode::StructuralModelBuilder& builder )
    {
        std::vector< geode::uuid > visited;
        std::vector< geode::uuid > treated_faults;
        for( const auto& block : structural_model.blocks() )
        {
            std::queue< geode::uuid > block_queue;
            const auto block_id = block.id();
            if( absl::c_contains( visited, block_id ) )
            {
                continue;
            }
            block_queue.push( block_id );
            visited.push_back( block_id );
            const auto& fault_block =
                structural_model.fault_block( builder.add_fault_block() );
            while( !block_queue.empty() )
            {
                const auto current_block_id = block_queue.front();
                block_queue.pop();
                const auto& curent_block =
                    structural_model.block( current_block_id );
                builder.add_block_in_fault_block( curent_block, fault_block );
                for( const auto& boundary :
                    structural_model.boundaries( curent_block ) )
                {
                    const auto fault_id =
                        is_fault( structural_model, boundary );
                    if( !fault_id )
                    {
                        add_incident_blocks_to_queue(
                            structural_model, boundary, block_queue, visited );
                    }
                }
            }
        }
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
            add_all_blocks_to_single_fault_block( structural_model, builder );
            return;
        }
        build_fault_blocks( structural_model, builder );
    }
} // namespace geode