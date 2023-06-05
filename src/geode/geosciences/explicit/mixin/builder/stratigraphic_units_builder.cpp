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

#include <geode/geosciences/explicit/mixin/builder/stratigraphic_units_builder.h>

#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.h>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_units.h>

namespace geode
{
    template < index_t dimension >
    const uuid&
        StratigraphicUnitsBuilder< dimension >::create_stratigraphic_unit()
    {
        return stratigraphic_units_.create_stratigraphic_unit();
    }

    template < index_t dimension >
    void StratigraphicUnitsBuilder< dimension >::create_stratigraphic_unit(
        uuid stratigraphic_unit_id )
    {
        stratigraphic_units_.create_stratigraphic_unit( stratigraphic_unit_id );
    }

    template < index_t dimension >
    void StratigraphicUnitsBuilder< dimension >::delete_stratigraphic_unit(
        const StratigraphicUnit< dimension >& stratigraphic_unit )
    {
        stratigraphic_units_.delete_stratigraphic_unit( stratigraphic_unit );
    }

    template < index_t dimension >
    void StratigraphicUnitsBuilder< dimension >::load_stratigraphic_units(
        absl::string_view directory )
    {
        return stratigraphic_units_.load_stratigraphic_units( directory );
    }

    template < index_t dimension >
    void StratigraphicUnitsBuilder< dimension >::set_stratigraphic_unit_name(
        const uuid& id, absl::string_view name )
    {
        stratigraphic_units_.modifiable_stratigraphic_unit( id )
            .set_stratigraphic_unit_name(
                name, typename StratigraphicUnit<
                          dimension >::StratigraphicUnitsBuilderKey{} );
    }

    template class opengeode_geosciences_explicit_api
        StratigraphicUnitsBuilder< 2 >;
    template class opengeode_geosciences_explicit_api
        StratigraphicUnitsBuilder< 3 >;
} // namespace geode
