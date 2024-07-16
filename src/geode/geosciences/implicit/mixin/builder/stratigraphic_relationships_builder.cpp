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

#include <geode/geosciences/implicit/mixin/builder/stratigraphic_relationships_builder.hpp>

namespace geode
{
    StratigraphicRelationshipsBuilder::StratigraphicRelationshipsBuilder(
        StratigraphicRelationships& relationships )
        : relationships_( relationships )
    {
    }

    void StratigraphicRelationshipsBuilder::unregister_component(
        const uuid& id )
    {
        relationships_.remove_component( id, {} );
    }

    index_t StratigraphicRelationshipsBuilder::add_above_relation(
        const ComponentID& above, const ComponentID& under )
    {
        return relationships_.add_above_relation( above, under, {} );
    }

    index_t StratigraphicRelationshipsBuilder::add_erosion_relation(
        const ComponentID& erosion, const ComponentID& eroded )
    {
        return relationships_.add_erosion_relation( erosion, eroded, {} );
    }

    index_t StratigraphicRelationshipsBuilder::add_baselap_relation(
        const ComponentID& baselap_top, const ComponentID& baselap )
    {
        return relationships_.add_baselap_relation( baselap_top, baselap, {} );
    }

    void StratigraphicRelationshipsBuilder::remove_relation(
        const uuid& id1, const uuid& id2 )
    {
        return relationships_.remove_relation( id1, id2, {} );
    }

    void StratigraphicRelationshipsBuilder::remove_above_relation(
        const uuid& id1, const uuid& id2 )
    {
        relationships_.remove_above_relation( id1, id2, {} );
    }

    void StratigraphicRelationshipsBuilder::remove_unconformity_relation(
        const uuid& id1, const uuid& id2 )
    {
        relationships_.remove_unconformity_relation( id1, id2, {} );
    }

    void StratigraphicRelationshipsBuilder::copy_stratigraphic_relationships(
        const ModelCopyMapping& mapping,
        const StratigraphicRelationships& relationships )
    {
        relationships_.copy_stratigraphic_relationships(
            mapping, relationships, {} );
    }

    void StratigraphicRelationshipsBuilder::load_stratigraphic_relationships(
        std::string_view directory )
    {
        relationships_.load_stratigraphic_relationships( directory, {} );
    }

} // namespace geode
