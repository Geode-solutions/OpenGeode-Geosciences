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

#include <optional>

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/model/mixin/core/component_type.hpp>
#include <geode/model/representation/core/mapping.hpp>

#include <geode/geosciences/implicit/common.hpp>

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

        [[nodiscard]] bool is_above(
            const uuid& above, const uuid& under ) const;

        [[nodiscard]] std::optional< uuid > above( const uuid& element ) const;

        [[nodiscard]] std::optional< uuid > under( const uuid& element ) const;

        void save_stratigraphic_relationships(
            std::string_view directory ) const;

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

        index_t add_erosion_relation( const ComponentID& erosion,
            const ComponentID& eroded,
            StratigraphicRelationshipsBuilderKey );

        index_t add_baselap_relation( const ComponentID& baselap_top,
            const ComponentID& baselap,
            StratigraphicRelationshipsBuilderKey );

        /*!
         * Remove all relations between two components
         */
        void remove_relation( const uuid& id1,
            const uuid& id2,
            StratigraphicRelationshipsBuilderKey );

        /*!
         * Removes any above/under relationship between two components
         */
        void remove_above_relation( const uuid& id1,
            const uuid& id2,
            StratigraphicRelationshipsBuilderKey );

        /*!
         * Removes any unconformity relationship between two components
         */
        void remove_unconformity_relation( const uuid& id1,
            const uuid& id2,
            StratigraphicRelationshipsBuilderKey );

        void copy_stratigraphic_relationships( const ModelCopyMapping& mapping,
            const StratigraphicRelationships& relationships,
            StratigraphicRelationshipsBuilderKey );

        void load_stratigraphic_relationships(
            std::string_view directory, StratigraphicRelationshipsBuilderKey );

    protected:
        StratigraphicRelationships(
            StratigraphicRelationships&& other ) noexcept;

        StratigraphicRelationships& operator=(
            StratigraphicRelationships&& other ) noexcept;

        [[nodiscard]] bool is_eroded_by(
            const uuid& eroded, const uuid& erosion ) const;

        [[nodiscard]] bool is_baselap_of(
            const uuid& baselap, const uuid& baselap_top ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
