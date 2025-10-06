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

#pragma once

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/geosciences/explicit/common.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_geosciences_explicit_api StratigraphicUnits
    {
        OPENGEODE_DISABLE_COPY( StratigraphicUnits );
        PASSKEY( StratigraphicUnitsBuilder< dimension >,
            StratigraphicUnitsBuilderKey );

    public:
        using Builder = StratigraphicUnitsBuilder< dimension >;
        using Type = StratigraphicUnit< dimension >;

        class opengeode_geosciences_explicit_api StratigraphicUnitRangeBase
        {
        public:
            ~StratigraphicUnitRangeBase();

            [[nodiscard]] bool operator!=(
                const StratigraphicUnitRangeBase& /*unused*/ ) const;

            void operator++();

            void set_active_only();

        protected:
            StratigraphicUnitRangeBase(
                const StratigraphicUnits& stratigraphic_units );
            StratigraphicUnitRangeBase(
                StratigraphicUnitRangeBase&& other ) noexcept;
            StratigraphicUnitRangeBase(
                const StratigraphicUnitRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_geosciences_explicit_api StratigraphicUnitRange
            : public StratigraphicUnitRangeBase
        {
        public:
            StratigraphicUnitRange(
                const StratigraphicUnits& stratigraphic_units );

            [[nodiscard]] const StratigraphicUnitRange& begin() const
            {
                return *this;
            }

            [[nodiscard]] const StratigraphicUnitRange& end() const
            {
                return *this;
            }

            [[nodiscard]] const StratigraphicUnit< dimension >&
                operator*() const;
        };

    public:
        ~StratigraphicUnits();

        [[nodiscard]] index_t nb_stratigraphic_units() const;

        [[nodiscard]] index_t nb_active_stratigraphic_units() const;

        [[nodiscard]] bool has_stratigraphic_unit( const uuid& id ) const;

        [[nodiscard]] const StratigraphicUnit< dimension >& stratigraphic_unit(
            const uuid& id ) const;

        [[nodiscard]] StratigraphicUnitRange stratigraphic_units() const;

        [[nodiscard]] StratigraphicUnitRange active_stratigraphic_units() const;

        [[nodiscard]] StratigraphicUnitRange components() const
        {
            return stratigraphic_units();
        }

        [[nodiscard]] bool has_component( const uuid& id ) const
        {
            return has_stratigraphic_unit( id );
        }

        [[nodiscard]] const StratigraphicUnit< dimension >& component(
            const uuid& id ) const
        {
            return stratigraphic_unit( id );
        }

        void save_stratigraphic_units( std::string_view directory ) const;

    protected:
        StratigraphicUnits();
        StratigraphicUnits( StratigraphicUnits&& other ) noexcept;

        StratigraphicUnits< dimension >& operator=(
            StratigraphicUnits< dimension >&& other ) noexcept;

    private:
        class ModifiableStratigraphicUnitRange
            : public StratigraphicUnitRangeBase
        {
        public:
            ModifiableStratigraphicUnitRange(
                const StratigraphicUnits& stratigraphic_units );

            [[nodiscard]] const ModifiableStratigraphicUnitRange& begin() const
            {
                return *this;
            }

            [[nodiscard]] const ModifiableStratigraphicUnitRange& end() const
            {
                return *this;
            }

            [[nodiscard]] StratigraphicUnit< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_stratigraphic_unit(
            StratigraphicUnitsBuilderKey key );

        void create_stratigraphic_unit(
            uuid stratigraphic_unit_id, StratigraphicUnitsBuilderKey key );

        void delete_stratigraphic_unit(
            const StratigraphicUnit< dimension >& stratigraphic_unit,
            StratigraphicUnitsBuilderKey key );

        void load_stratigraphic_units(
            std::string_view directory, StratigraphicUnitsBuilderKey key );

        [[nodiscard]] ModifiableStratigraphicUnitRange
            modifiable_stratigraphic_units( StratigraphicUnitsBuilderKey key );

        [[nodiscard]] StratigraphicUnit< dimension >&
            modifiable_stratigraphic_unit(
                const uuid& id, StratigraphicUnitsBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( StratigraphicUnits );
} // namespace geode
