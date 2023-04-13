/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/implicit/mixin/builder/stratigraphic_relationships_builder.h>

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

    void StratigraphicRelationshipsBuilder::remove_relation(
        const uuid& id1, const uuid& id2 )
    {
        return relationships_.remove_relation( id1, id2, {} );
    }

    void StratigraphicRelationshipsBuilder::copy_stratigraphic_relationships(
        const ModelCopyMapping& mapping,
        const StratigraphicRelationships& relationships )
    {
        relationships_.copy_stratigraphic_relationships(
            mapping, relationships, {} );
    }

    void StratigraphicRelationshipsBuilder::load_stratigraphic_relationships(
        absl::string_view directory )
    {
        relationships_.load_stratigraphic_relationships( directory, {} );
    }

} // namespace geode
