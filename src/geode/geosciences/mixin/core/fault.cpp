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

#include <geode/geosciences/mixin/core/fault.h>

#include <geode/basic/bitsery_archive.h>
#include <geode/basic/pimpl_impl.h>

namespace geode
{
    template < index_t dimension >
    class Fault< dimension >::Impl
    {
    public:
        FAULT_TYPE type() const
        {
            return fault_type_;
        }

        void set_type( FAULT_TYPE type )
        {
            fault_type_ = type;
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.value4b( impl.fault_type_ );
                } } } );
        }

    private:
        FAULT_TYPE fault_type_{ FAULT_TYPE::NO_TYPE };
    };

    template < index_t dimension >
    Fault< dimension >::Fault() // NOLINT
    {
    }

    template < index_t dimension >
    Fault< dimension >::Fault( typename Fault::FAULT_TYPE type )
    {
        impl_->set_type( type );
    }

    template < index_t dimension >
    Fault< dimension >::~Fault() // NOLINT
    {
    }

    template < index_t dimension >
    bool Fault< dimension >::has_type() const
    {
        return type() != FAULT_TYPE::NO_TYPE;
    }

    template < index_t dimension >
    typename Fault< dimension >::FAULT_TYPE Fault< dimension >::type() const
    {
        return impl_->type();
    }

    template < index_t dimension >
    void Fault< dimension >::set_type(
        typename Fault::FAULT_TYPE type, FaultsBuilderKey )
    {
        return impl_->set_type( type );
    }

    template < index_t dimension >
    template < typename Archive >
    void Fault< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, Fault >{ { []( Archive& a, Fault& fault ) {
                a.object( fault.impl_ );
                a.ext( fault,
                    bitsery::ext::BaseClass< Component< dimension > >{} );
            } } } );
    }

    template class opengeode_geosciences_geosciences_api Fault< 2 >;
    template class opengeode_geosciences_geosciences_api Fault< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_geosciences_api, Fault< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_geosciences_api, Fault< 3 > );
} // namespace geode
