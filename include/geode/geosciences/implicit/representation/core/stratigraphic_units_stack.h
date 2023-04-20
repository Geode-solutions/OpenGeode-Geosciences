/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/identifier.h>

#include <geode/geosciences/explicit/mixin/core/horizons.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>
#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStackBuilder );
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * A StratigraphicUnitsStack is a set of Horizons and StratigraphicUnits,
     * linked by relations defining how they are stacked (which unit is above
     * and under which horizon, and inversely).
     */
    template < index_t dimension >
    class opengeode_geosciences_implicit_api StratigraphicUnitsStack
        : public StratigraphicRelationships,
          public Horizons< dimension >,
          public StratigraphicUnits< dimension >,
          public Identifier
    {
        PASSKEY( StratigraphicUnitsStackBuilder< dimension >,
            StratigraphicUnitsStackBuilderKey );

    public:
        StratigraphicUnitsStack();
        StratigraphicUnitsStack(
            StratigraphicUnitsStack< dimension >&& su_stack );
        ~StratigraphicUnitsStack();

        StratigraphicUnitsStack< dimension >& operator=(
            StratigraphicUnitsStack< dimension >&& other );

        static absl::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_sus", dimension, "d" );
            return extension;
        }

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }
    };
    ALIAS_2D_AND_3D( StratigraphicUnitsStack );
} // namespace geode