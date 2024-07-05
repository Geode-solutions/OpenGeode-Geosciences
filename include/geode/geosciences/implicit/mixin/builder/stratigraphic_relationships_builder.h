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
        explicit StratigraphicRelationshipsBuilder(
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

        /*!
         * Remove a relationship of type above/under between two components
         */
        void remove_above_relation( const uuid& id1, const uuid& id2 );

        /*!
         * Remove an unconformity relationship between two components
         */
        void remove_unconformity_relation( const uuid& id1, const uuid& id2 );

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

        /*!
         * Adds a new unconformity relationship of erosion type between two
         * components
         */
        index_t add_erosion_relation(
            const ComponentID& erosion, const ComponentID& eroded );

        /*!
         * Adds a new unconformity relationship of baselap type between two
         * components
         */
        index_t add_baselap_relation(
            const ComponentID& baselap_top, const ComponentID& baselap );

    private:
        StratigraphicRelationships& relationships_;
    };
} // namespace geode