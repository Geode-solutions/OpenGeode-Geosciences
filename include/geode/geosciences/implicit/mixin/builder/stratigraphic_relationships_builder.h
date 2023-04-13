/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/model/mixin/builder/relationships_builder.h>

#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.h>

namespace geode
{
    /*!
     * Class managing modification of StratigraphicRelationships
     */
    class opengeode_geosciences_implicit_api StratigraphicRelationshipsBuilder
    {
    public:
        StratigraphicRelationshipsBuilder(
            StratigraphicRelationships& relationships );

        /*!
         * Remove a component from the set of components registered by the
         * StratigraphicRelationships, and its above/under relationships
         * @param[in] id Unique index of the component to remove
         */
        void unregister_component( const uuid& id );

        /*!
         * Remove a relationship between two components
         */
        void remove_relation( const uuid& id1, const uuid& id2 );

        void copy_stratigraphic_relationships( const ModelCopyMapping& mapping,
            const StratigraphicRelationships& relationships );

        void load_stratigraphic_relationships( absl::string_view directory );

    protected:
        /*!
         * Adds a new relationship of type above-under between two
         * components
         */
        index_t add_above_relation(
            const ComponentID& above, const ComponentID& under );

    private:
        StratigraphicRelationships& relationships_;
    };
} // namespace geode