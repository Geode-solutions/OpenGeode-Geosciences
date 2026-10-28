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

#include <geode/geosciences/simulation/helpers/physical_problem_blocks.hpp>

#include <absl/types/span.h>

#include <geode/basic/range.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>

namespace
{
    // Blocks already assigned keep their item: the first item wins
    void assign_region_blocks( const geode::BRep& model,
        absl::Span< const geode::uuid > regions,
        geode::index_t item,
        absl::flat_hash_map< geode::uuid, geode::index_t >& block_items )
    {
        for( const auto& region : regions )
        {
            const auto blocks =
                geode::physical_problem_region_blocks( model, region );
            if( !blocks )
            {
                continue;
            }
            for( const auto& block : blocks.value() )
            {
                block_items.try_emplace( block, item );
            }
        }
    }

    template < typename ItemRegions >
    absl::flat_hash_map< geode::uuid, geode::index_t > assign_blocks(
        const geode::BRep& model,
        geode::index_t nb_items,
        const ItemRegions& item_regions )
    {
        absl::flat_hash_map< geode::uuid, geode::index_t > block_items;
        std::optional< geode::index_t > default_item;
        for( const auto item : geode::Range{ nb_items } )
        {
            const auto regions = item_regions( item );
            if( regions.empty() )
            {
                if( !default_item )
                {
                    default_item = item;
                }
                continue;
            }
            assign_region_blocks( model, regions, item, block_items );
        }
        if( default_item )
        {
            for( const auto& block : model.blocks() )
            {
                block_items.try_emplace( block.id(), default_item.value() );
            }
        }
        return block_items;
    }
} // namespace

namespace geode
{
    std::optional< std::vector< uuid > > physical_problem_region_blocks(
        const BRep& model, const uuid& region )
    {
        if( model.has_block( region ) )
        {
            return std::vector< uuid >{ region };
        }
        std::vector< uuid > blocks;
        for( const auto& item : model.items( region ) )
        {
            if( item.type != Block3D::component_type_static() )
            {
                return std::nullopt;
            }
            blocks.push_back( item.id );
        }
        if( blocks.empty() )
        {
            return std::nullopt;
        }
        return blocks;
    }

    absl::flat_hash_map< uuid, index_t > physical_problem_block_materials(
        const PhysicalProblem& problem, const BRep& model )
    {
        const auto materials = problem.materials();
        return assign_blocks(
            model, materials.size(), [&materials]( index_t material ) {
                return absl::Span< const uuid >{ materials[material].regions };
            } );
    }

    absl::flat_hash_map< uuid, index_t >
        physical_problem_block_initial_conditions(
            const PhysicalProblem& problem, const BRep& model )
    {
        return assign_blocks( model, problem.nb_initial_conditions(),
            [&problem]( index_t condition ) {
                return problem.initial_condition( condition ).regions();
            } );
    }
} // namespace geode
