/*
 * Copyright (c) 2029 - 2023 Geode-solutions
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

#include <geode/geosciences/mixin/core/geographic_coordinate_system.h>

#include <ogr_spatialref.h>
#include <ogr_srs_api.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/private/points_impl.h>

namespace geode
{
    template < index_t dimension >
    class GeographicCoordinateSystem< dimension >::Impl
        : public detail::PointsImpl< dimension >
    {
        friend class bitsery::Access;

    public:
        Impl( AttributeManager& manager, Info info )
            : detail::PointsImpl< dimension >{ manager, info.name },
              info_{ std::move( info ) }
        {
        }

        Impl() = default;

        const Info& info() const
        {
            return info_;
        }

        void convert_geographic_coordinate_system(
            const GeographicCoordinateSystem< dimension >& from,
            GeographicCoordinateSystem< dimension >& to,
            index_t nb_points ) const
        {
            OGRSpatialReference origin;
            origin.SetFromUserInput( info_.authority_code().c_str() );
            OGRSpatialReference destination;
            destination.SetFromUserInput( to.info().authority_code().c_str() );
            auto* transformer =
                OGRCreateCoordinateTransformation( &origin, &destination );
            for( const auto p : Range{ nb_points } )
            {
                auto point = from.point( p );
                std::array< double, 3 > values{ 0, 0, 0 };
                for( const auto d : LRange{ dimension } )
                {
                    values[d] = point.value( d );
                }
                const auto status = transformer->Transform(
                    1, &values[0], &values[1], &values[2] );
                OPENGEODE_EXCEPTION( status,
                    "[GeographicCoordinateSystem::convert_geographic_"
                    "coordinate_system] Failed to convert coordinates" );
                for( const auto d : LRange{ dimension } )
                {
                    point.set_value( d, values[d] );
                }
                to.set_point( p, std::move( point ) );
            }
            OGRCoordinateTransformation::DestroyCT( transformer );
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.ext( impl, bitsery::ext::BaseClass<
                                     detail::PointsImpl< dimension > >{} );
                    a.object( impl.info_ );
                } } } );
        }

    private:
        Info info_;
    };

    template < index_t dimension >
    GeographicCoordinateSystem< dimension >::GeographicCoordinateSystem()
    {
    }

    template < index_t dimension >
    GeographicCoordinateSystem< dimension >::GeographicCoordinateSystem(
        AttributeManager& manager, Info info )
        : impl_{ manager, std::move( info ) }
    {
    }

    template < index_t dimension >
    GeographicCoordinateSystem< dimension >::~GeographicCoordinateSystem()
    {
    }

    template < index_t dimension >
    auto GeographicCoordinateSystem< dimension >::info() const -> const Info&
    {
        return impl_->info();
    }

    template < index_t dimension >
    const Point< dimension >& GeographicCoordinateSystem< dimension >::point(
        index_t point_id ) const
    {
        return impl_->get_point( point_id );
    }

    template < index_t dimension >
    void GeographicCoordinateSystem< dimension >::set_point(
        index_t point_id, Point< dimension > point )
    {
        impl_->set_point( point_id, std::move( point ) );
    }

    template < index_t dimension >
    template < typename Archive >
    void GeographicCoordinateSystem< dimension >::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, GeographicCoordinateSystem >{
                       { []( Archive& a, GeographicCoordinateSystem& crs ) {
                           a.ext( crs,
                               bitsery::ext::BaseClass<
                                   CoordinateReferenceSystem< dimension > >{} );
                           a.object( crs.impl_ );
                       } } } );
    }

    template < index_t dimension >
    auto
        GeographicCoordinateSystem< dimension >::geographic_coordinate_systems()
            -> absl::FixedArray< Info >
    {
        int nb_crs{ 0 };
        auto** gdal_list =
            OSRGetCRSInfoListFromDatabase( nullptr, nullptr, &nb_crs );
        absl::FixedArray< Info > infos( nb_crs );
        for( const auto i : Range{ nb_crs } )
        {
            const auto* gdal_crs = gdal_list[i];
            infos[i] = { gdal_crs->pszAuthName, gdal_crs->pszCode,
                gdal_crs->pszName };
        }
        OSRDestroyCRSInfoList( gdal_list );
        return infos;
    }

    template < index_t dimension >
    void GeographicCoordinateSystem< dimension >::
        convert_geographic_coordinate_system(
            GeographicCoordinateSystem< dimension >& target,
            index_t nb_points ) const
    {
        impl_->convert_geographic_coordinate_system( *this, target, nb_points );
    }

    template class opengeode_geosciences_geosciences_api
        GeographicCoordinateSystem< 2 >;
    template class opengeode_geosciences_geosciences_api
        GeographicCoordinateSystem< 3 >;

    SERIALIZE_BITSERY_ARCHIVE( opengeode_geosciences_geosciences_api,
        GeographicCoordinateSystem< 2 > );
    SERIALIZE_BITSERY_ARCHIVE( opengeode_geosciences_geosciences_api,
        GeographicCoordinateSystem< 3 > );
} // namespace geode