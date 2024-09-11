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

#include <geode/geosciences/explicit/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Fault );
    FORWARD_DECLARATION_DIMENSION_CLASS( Faults );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class FaultsBuilder
    {
    public:
        void load_faults( std::string_view directory );

        void set_fault_type( const uuid& fault_id,
            typename Fault< dimension >::FAULT_TYPE type );

        void set_fault_name( const uuid& id, std::string_view name );

    protected:
        explicit FaultsBuilder( Faults< dimension >& faults )
            : faults_( faults )
        {
        }

        [[nodiscard]] const uuid& create_fault();

        [[nodiscard]] const uuid& create_fault(
            typename Fault< dimension >::FAULT_TYPE type );

        void create_fault( uuid fault_id );

        void create_fault(
            uuid fault_id, typename Fault< dimension >::FAULT_TYPE type );

        void delete_fault( const Fault< dimension >& fault );

    private:
        Faults< dimension >& faults_;
    };
    ALIAS_2D_AND_3D( FaultsBuilder );
} // namespace geode
