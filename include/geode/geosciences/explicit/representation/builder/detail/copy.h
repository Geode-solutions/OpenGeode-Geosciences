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

#pragma once

#include <absl/container/flat_hash_map.h>

#include <geode/basic/mapping.h>
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

namespace geode
{
    namespace detail
    {
        using Mapping = BijectiveMapping< uuid >;
        template < typename ModelFrom, typename BuilderTo >
        void copy_faults(
            const ModelFrom& from, BuilderTo& builder_to, Mapping& mapping )
        {
            for( const auto& fault : from.faults() )
            {
                if( mapping.has_mapping_input( fault.id() ) )
                {
                    const auto& fault_id = mapping.in2out( fault.id() );
                    builder_to.add_fault( fault_id );
                    builder_to.set_fault_name( fault_id, fault.name() );
                    builder_to.set_fault_type( fault_id, fault.type() );
                }
                else
                {
                    const auto& fault_id = builder_to.add_fault();
                    builder_to.set_fault_name( fault_id, fault.name() );
                    builder_to.set_fault_type( fault_id, fault.type() );
                    mapping.map( fault.id(), fault_id );
                }
            }
        }

        template < typename ModelFrom, typename BuilderTo >
        void copy_horizons(
            const ModelFrom& from, BuilderTo& builder_to, Mapping& mapping )
        {
            for( const auto& horizon : from.horizons() )
            {
                if( mapping.has_mapping_input( horizon.id() ) )
                {
                    const auto& horizon_id = mapping.in2out( horizon.id() );
                    builder_to.add_horizon( horizon_id );
                    builder_to.set_horizon_name( horizon_id, horizon.name() );
                    builder_to.set_horizon_type( horizon_id, horizon.type() );
                }
                else
                {
                    const auto& horizon_id = builder_to.add_horizon();
                    mapping.map( horizon.id(), horizon_id );
                    builder_to.set_horizon_name( horizon_id, horizon.name() );
                    builder_to.set_horizon_type( horizon_id, horizon.type() );
                }
            }
        }

        template < typename ModelFrom, typename BuilderTo >
        void copy_fault_blocks(
            const ModelFrom& from, BuilderTo& builder_to, Mapping& mapping )
        {
            for( const auto& fault_block : from.fault_blocks() )
            {
                if( mapping.has_mapping_input( fault_block.id() ) )
                {
                    const auto& fault_block_id =
                        mapping.in2out( fault_block.id() );
                    builder_to.add_fault( fault_block_id );
                    builder_to.set_fault_name(
                        fault_block_id, fault_block.name() );
                }
                else
                {
                    const auto& fault_block_id = builder_to.add_fault_block();
                    builder_to.set_fault_block_name(
                        fault_block_id, fault_block.name() );
                    mapping.map( fault_block.id(), fault_block_id );
                }
            }
        }

        template < typename ModelFrom, typename BuilderTo >
        void copy_stratigraphic_units(
            const ModelFrom& from, BuilderTo& builder_to, Mapping& mapping )
        {
            for( const auto& stratigraphic_unit : from.stratigraphic_units() )
            {
                if( mapping.has_mapping_input( stratigraphic_unit.id() ) )
                {
                    const auto& stratigraphic_unit_id =
                        mapping.in2out( stratigraphic_unit.id() );
                    builder_to.add_stratigraphic_unit( stratigraphic_unit_id );
                    builder_to.set_stratigraphic_unit_name(
                        stratigraphic_unit_id, stratigraphic_unit.name() );
                }
                else
                {
                    const auto& stratigraphic_unit_id =
                        builder_to.add_stratigraphic_unit();
                    mapping.map(
                        stratigraphic_unit.id(), stratigraphic_unit_id );
                    builder_to.set_stratigraphic_unit_name(
                        stratigraphic_unit_id, stratigraphic_unit.name() );
                }
            }
        }

        template < typename ModelFrom,
            typename CollectionRangeFrom,
            typename BuilderTo >
        void copy_collection_item_relationships( const ModelFrom& from,
            const CollectionRangeFrom& collection_range_from,
            const Mapping& collection_mapping,
            const Mapping& item_mapping,
            BuilderTo& builder_to )
        {
            for( const auto& collection_from : collection_range_from )
            {
                const auto& collection_to_id =
                    collection_mapping.in2out( collection_from.id() );
                for( const auto& item_from :
                    from.items( collection_from.id() ) )
                {
                    builder_to.add_item_in_collection(
                        item_mapping.in2out( item_from.id() ),
                        collection_to_id );
                }
            }
        }

    } // namespace detail
} // namespace geode
