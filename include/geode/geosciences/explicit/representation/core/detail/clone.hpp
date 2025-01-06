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

#include <geode/model/representation/core/mapping.hpp>

#include <geode/geosciences/explicit/common.hpp>

namespace geode
{
    class Section;
    class BRep;
} // namespace geode

namespace geode
{
    namespace detail
    {
        template < typename Model >
        void add_geology_clone_mapping(
            ModelCopyMapping& mapping, const Model& model );

        template < typename Model >
        [[nodiscard]] BijectiveMapping< uuid > clone_horizon_mapping(
            const Model& model )
        {
            BijectiveMapping< uuid > horizon_clone_mapping;
            for( const auto& horizon : model.horizons() )
            {
                horizon_clone_mapping.map( horizon.id(), horizon.id() );
            }
            return horizon_clone_mapping;
        }

        template < typename Model >
        [[nodiscard]] BijectiveMapping< uuid > clone_stratigraphic_unit_mapping(
            const Model& model )
        {
            BijectiveMapping< uuid > stratigraphic_unit_clone_mapping;
            for( const auto& stratigraphic_unit : model.stratigraphic_units() )
            {
                stratigraphic_unit_clone_mapping.map(
                    stratigraphic_unit.id(), stratigraphic_unit.id() );
            }
            return stratigraphic_unit_clone_mapping;
        }
    } // namespace detail
} // namespace geode
