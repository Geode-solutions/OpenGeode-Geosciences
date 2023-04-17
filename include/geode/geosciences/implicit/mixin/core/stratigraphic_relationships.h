/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/basic/passkey.h>
#include <geode/basic/pimpl.h>

#include <geode/model/mixin/core/component_type.h>
#include <geode/model/representation/core/mapping.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class AttributeManager;
    class StratigraphicRelationshipsBuilder;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * This class stores the relations between a set of geological components.
     * Each relationship links two components, one being above the other in the
     * stratigraphy. Components should be added through their uuids.
     */
    class opengeode_geosciences_implicit_api StratigraphicRelationships
    {
        PASSKEY( StratigraphicRelationshipsBuilder,
            StratigraphicRelationshipsBuilderKey );

    public:
        StratigraphicRelationships();
        ~StratigraphicRelationships();

        bool is_above( const uuid& above, const uuid& under ) const;

        absl::optional< uuid > above( const uuid& element ) const;

        absl::optional< uuid > under( const uuid& element ) const;

        void save_stratigraphic_relationships(
            absl::string_view directory ) const;

    public:
        /*!
         * Remove a component and all its associated relationships
         * @param[in] id Unique index of the component to remove
         */
        void remove_component(
            const uuid& id, StratigraphicRelationshipsBuilderKey );

        /*!
         * Adds a new relationship of type above-under between two
         * components
         */
        index_t add_above_relation( const ComponentID& above,
            const ComponentID& under,
            StratigraphicRelationshipsBuilderKey );

        /*!
         * Remove the relation between two components
         */
        void remove_relation( const uuid& id1,
            const uuid& id2,
            StratigraphicRelationshipsBuilderKey );

        void copy_stratigraphic_relationships( const ModelCopyMapping& mapping,
            const StratigraphicRelationships& relationships,
            StratigraphicRelationshipsBuilderKey );

        void load_stratigraphic_relationships(
            absl::string_view directory, StratigraphicRelationshipsBuilderKey );

    protected:
        StratigraphicRelationships( StratigraphicRelationships&& );

        StratigraphicRelationships& operator=(
            StratigraphicRelationships&& other );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
