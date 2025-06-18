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

#include <geode/geosciences/explicit/mixin/core/horizon.hpp>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/pimpl_impl.hpp>

namespace
{
    enum struct BACKWARD_CONTACT_TYPE
    {
        /// Default value - No horizon type defined
        no_type,
        conformal,
        non_conformal,
        topography,
        intrusion
    };
}

namespace geode
{
    template < index_t dimension >
    class Horizon< dimension >::Impl
    {
    public:
        CONTACT_TYPE contact_type() const
        {
            return contact_type_;
        }

        void set_contact_type( CONTACT_TYPE contact_type )
        {
            contact_type_ = contact_type;
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{
                    { []( Archive& archive2, Impl& impl ) {
                         BACKWARD_CONTACT_TYPE previous_type;
                         archive2.value4b( previous_type );
                         if( previous_type == BACKWARD_CONTACT_TYPE::no_type
                             || previous_type
                                    == BACKWARD_CONTACT_TYPE::conformal )
                         {
                             impl.contact_type_ = CONTACT_TYPE::conformal;
                         }
                         else if( previous_type
                                  == BACKWARD_CONTACT_TYPE::non_conformal )
                         {
                             impl.contact_type_ = CONTACT_TYPE::erosion;
                         }
                         else if( previous_type
                                  == BACKWARD_CONTACT_TYPE::topography )
                         {
                             impl.contact_type_ = CONTACT_TYPE::topography;
                         }
                         else if( previous_type
                                  == BACKWARD_CONTACT_TYPE::intrusion )
                         {
                             impl.contact_type_ = CONTACT_TYPE::intrusion;
                         }
                     },
                        []( Archive& archive2, Impl& impl ) {
                            archive2.value4b( impl.contact_type_ );
                        } } } );
        }

    private:
        CONTACT_TYPE contact_type_{ CONTACT_TYPE::conformal };
    };

    template < index_t dimension >
    Horizon< dimension >::Horizon() = default;

    template < index_t dimension >
    Horizon< dimension >::Horizon( typename Horizon::CONTACT_TYPE type )
    {
        impl_->set_contact_type( type );
    }

    template < index_t dimension >
    Horizon< dimension >::Horizon( Horizon&& ) noexcept = default;

    template < index_t dimension >
    Horizon< dimension >::~Horizon() = default;

    template < index_t dimension >
    typename Horizon< dimension >::CONTACT_TYPE
        Horizon< dimension >::contact_type() const
    {
        return impl_->contact_type();
    }

    template < index_t dimension >
    void Horizon< dimension >::set_contact_type(
        typename Horizon::CONTACT_TYPE contact_type, HorizonsBuilderKey )
    {
        return impl_->set_contact_type( contact_type );
    }

    template < index_t dimension >
    template < typename Archive >
    void Horizon< dimension >::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, Horizon >{ { []( Archive& a, Horizon& horizon ) {
                a.object( horizon.impl_ );
                a.ext( horizon,
                    bitsery::ext::BaseClass< Component< dimension > >{} );
            } } } );
    }

    template class opengeode_geosciences_explicit_api Horizon< 2 >;
    template class opengeode_geosciences_explicit_api Horizon< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_explicit_api, Horizon< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_explicit_api, Horizon< 3 > );
} // namespace geode
