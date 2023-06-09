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

#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>

namespace geode
{
    template < index_t dimension >
    StratigraphicUnitsStack< dimension >::StratigraphicUnitsStack()
    {
    }

    template < index_t dimension >
    StratigraphicUnitsStack< dimension >::StratigraphicUnitsStack(
        StratigraphicUnitsStack< dimension >&& su_stack )
        : StratigraphicRelationships{ std::move( su_stack ) },
          Horizons< dimension >{ std::move( su_stack ) },
          StratigraphicUnits< dimension >{ std::move( su_stack ) },
          Identifier{ std::move( su_stack ) }
    {
    }

    template < index_t dimension >
    StratigraphicUnitsStack< dimension >::~StratigraphicUnitsStack()
    {
    }

    template < index_t dimension >
    StratigraphicUnitsStack< dimension >&
        StratigraphicUnitsStack< dimension >::operator=(
            StratigraphicUnitsStack< dimension >&& other )
    {
        StratigraphicRelationships::operator=( std::move( other ) );
        Horizons< dimension >::operator=( std::move( other ) );
        StratigraphicUnits< dimension >::operator=( std::move( other ) );
        Identifier::operator=( std::move( other ) );
        return *this;
    }

    template class opengeode_geosciences_implicit_api
        StratigraphicUnitsStack< 2 >;
    template class opengeode_geosciences_implicit_api
        StratigraphicUnitsStack< 3 >;
} // namespace geode