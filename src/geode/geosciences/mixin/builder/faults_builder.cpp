/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geosciences/mixin/builder/faults_builder.h>

#include <geode/geosciences/mixin/core/fault.h>
#include <geode/geosciences/mixin/core/faults.h>

namespace geode
{
    template < index_t dimension >
    const uuid& FaultsBuilder< dimension >::create_fault()
    {
        return faults_.create_fault();
    }

    template < index_t dimension >
    const uuid& FaultsBuilder< dimension >::create_fault(
        typename Fault< dimension >::FAULT_TYPE type )
    {
        return faults_.create_fault( type );
    }

    template < index_t dimension >
    void FaultsBuilder< dimension >::delete_fault(
        const Fault< dimension >& fault )
    {
        faults_.delete_fault( fault );
    }

    template < index_t dimension >
    void FaultsBuilder< dimension >::load_faults( absl::string_view directory )
    {
        return faults_.load_faults( directory );
    }

    template < index_t dimension >
    void FaultsBuilder< dimension >::set_fault_type(
        const uuid& fault_id, typename Fault< dimension >::FAULT_TYPE type )
    {
        faults_.modifiable_fault( fault_id )
            .set_type( type, typename Fault< dimension >::FaultsBuilderKey{} );
    }

    template < index_t dimension >
    void FaultsBuilder< dimension >::set_fault_name(
        const uuid& id, absl::string_view name )
    {
        faults_.modifiable_fault( id ).set_fault_name(
            name, typename Fault< dimension >::FaultsBuilderKey{} );
    }

    template class opengeode_geosciences_geosciences_api FaultsBuilder< 2 >;
    template class opengeode_geosciences_geosciences_api FaultsBuilder< 3 >;
} // namespace geode
