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

#include <geode/basic/identifier.hpp>

#include <geode/geosciences/explicit/mixin/core/horizons.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.hpp>
#include <geode/geosciences/implicit/common.hpp>
#include <geode/geosciences/implicit/mixin/core/stratigraphic_relationships.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStackBuilder );
    FORWARD_DECLARATION_DIMENSION_CLASS( Horizon );
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnit );
    ALIAS_2D_AND_3D( Horizon );
    ALIAS_2D_AND_3D( StratigraphicUnit );
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * A HorizonsStack is a set of Horizons and StratigraphicUnits,
     * linked by relations defining how they are stacked (which unit is above
     * and under which horizon, and inversely).
     */
    template < index_t dimension >
    class opengeode_geosciences_implicit_api HorizonsStack
        : public StratigraphicRelationships,
          public Horizons< dimension >,
          public StratigraphicUnits< dimension >,
          public Identifier
    {
        PASSKEY( HorizonsStackBuilder< dimension >, HorizonsStackBuilderKey );

    public:
        using Builder = HorizonsStackBuilder< dimension >;
        static constexpr auto dim = dimension;

        HorizonsStack();
        HorizonsStack( HorizonsStack< dimension >&& horizons_stack ) noexcept;
        ~HorizonsStack();

        HorizonsStack< dimension >& operator=(
            HorizonsStack< dimension >&& other ) noexcept;

        HorizonsStack< dimension > clone() const;

        static std::string_view native_extension_static()
        {
            static const auto extension =
                absl::StrCat( "og_hst", dimension, "d" );
            return extension;
        }

        std::string_view native_extension() const
        {
            return native_extension_static();
        }

        uuid top_horizon() const;

        uuid bottom_horizon() const;

        bool is_eroded_by( const StratigraphicUnit< dimension >& eroded,
            const Horizon< dimension >& erosion ) const;

        bool is_baselap_of( const Horizon< dimension >& baselap,
            const StratigraphicUnit< dimension >& baselap_top ) const;
    };
    ALIAS_2D_AND_3D( HorizonsStack );
} // namespace geode