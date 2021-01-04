/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/geosciences/common.h>
#include <geode/geosciences/mixin/core/stratigraphic_unit.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_geosciences_geosciences_api StratigraphicUnits
    {
        OPENGEODE_DISABLE_COPY( StratigraphicUnits );

    public:
        using Builder = StratigraphicUnitsBuilder< dimension >;
        friend Builder;

        class opengeode_geosciences_geosciences_api StratigraphicUnitRangeBase
        {
        public:
            ~StratigraphicUnitRangeBase();

            bool operator!=(
                const StratigraphicUnitRangeBase& /*unused*/ ) const;

            void operator++();

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

        class opengeode_geosciences_geosciences_api StratigraphicUnitRange
            : public StratigraphicUnitRangeBase
        {
        public:
            StratigraphicUnitRange(
                const StratigraphicUnits& stratigraphic_units );

            const StratigraphicUnitRange& begin() const
            {
                return *this;
            }

            const StratigraphicUnitRange& end() const
            {
                return *this;
            }

            const StratigraphicUnit< dimension >& operator*() const;
        };

    public:
        ~StratigraphicUnits();

        index_t nb_stratigraphic_units() const;

        const StratigraphicUnit< dimension >& stratigraphic_unit(
            const uuid& id ) const;

        StratigraphicUnitRange stratigraphic_units() const;

        void save_stratigraphic_units( absl::string_view directory ) const;

    protected:
        friend class StratigraphicUnitsBuilder< dimension >;
        StratigraphicUnits();
        StratigraphicUnits( StratigraphicUnits&& );

    private:
        class ModifiableStratigraphicUnitRange
            : public StratigraphicUnitRangeBase
        {
        public:
            ModifiableStratigraphicUnitRange(
                const StratigraphicUnits& stratigraphic_units );

            const ModifiableStratigraphicUnitRange& begin() const
            {
                return *this;
            }

            const ModifiableStratigraphicUnitRange& end() const
            {
                return *this;
            }

            StratigraphicUnit< dimension >& operator*() const;
        };

    private:
        const uuid& create_stratigraphic_unit();

        void delete_stratigraphic_unit(
            const StratigraphicUnit< dimension >& stratigraphic_unit );

        void load_stratigraphic_units( absl::string_view directory );

        ModifiableStratigraphicUnitRange modifiable_stratigraphic_units();

        StratigraphicUnit< dimension >& modifiable_stratigraphic_unit(
            const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( StratigraphicUnits );
} // namespace geode
