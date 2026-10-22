/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/geosciences/explicit/geometry/geographic_coordinate_system.hpp>

#include <memory>

#include <absl/algorithm/container.h>

#include <ogr_spatialref.h>
#include <ogr_srs_api.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/internal/points_impl.hpp>

namespace geode
{

    GeographicCoordinateSystemInfo::GeographicCoordinateSystemInfo(
        std::string authority_in, std::string code_in, std::string name_in )
        : authority{ std::move( authority_in ) },
          code{ std::move( code_in ) },
          name{ std::move( name_in ) }
    {
    }

    GeographicCoordinateSystemInfo::GeographicCoordinateSystemInfo() = default;

    GeographicCoordinateSystemInfo::~GeographicCoordinateSystemInfo() = default;

    template < index_t dimension >
    class GeographicCoordinateSystem< dimension >::Impl
    {
        friend class bitsery::Access;

    public:
        Impl( GeographicCoordinateSystemInfo info ) : info_{ std::move( info ) }
        {
        }

        Impl() = default;

        const GeographicCoordinateSystemInfo& info() const
        {
            return info_;
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, Impl >{ { []( Archive& a, Impl& impl ) {
                    a.object( impl.info_ );
                } } } );
        }

    private:
        GeographicCoordinateSystemInfo info_;
    };

    template < index_t dimension >
    GeographicCoordinateSystem< dimension >::GeographicCoordinateSystem() =
        default;

    template < index_t dimension >
    GeographicCoordinateSystem< dimension >::GeographicCoordinateSystem(
        AttributeManager& manager, GeographicCoordinateSystemInfo info )
        : AttributeCoordinateReferenceSystem< dimension >{ manager, info.name },
          impl_{ std::move( info ) }
    {
    }

    template < index_t dimension >
    GeographicCoordinateSystem< dimension >::GeographicCoordinateSystem(
        AttributeManager& manager,
        const uuid& uuid,
        GeographicCoordinateSystemInfo info )
        : AttributeCoordinateReferenceSystem< dimension >{ manager, uuid },
          impl_{ std::move( info ) }
    {
    }

    template < index_t dimension >
    GeographicCoordinateSystem< dimension >::~GeographicCoordinateSystem() =
        default;

    template < index_t dimension >
    const GeographicCoordinateSystemInfo&
        GeographicCoordinateSystem< dimension >::info() const
    {
        return impl_->info();
    }

    template < index_t dimension >
    template < typename Archive >
    void GeographicCoordinateSystem< dimension >::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, GeographicCoordinateSystem >{
                       { []( Archive& a, GeographicCoordinateSystem& crs ) {
                           a.ext( crs, bitsery::ext::BaseClass<
                                           AttributeCoordinateReferenceSystem<
                                               dimension > >{} );
                           a.object( crs.impl_ );
                       } } } );
    }

    template < index_t dimension >
    absl::FixedArray< GeographicCoordinateSystemInfo >
        GeographicCoordinateSystem< dimension >::geographic_coordinate_systems()
    {
        int nb_crs{ 0 };
        auto** gdal_list =
            OSRGetCRSInfoListFromDatabase( nullptr, nullptr, &nb_crs );
        absl::FixedArray< GeographicCoordinateSystemInfo > infos( nb_crs );
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
    void GeographicCoordinateSystem< dimension >::import_coordinates(
        const GeographicCoordinateSystem< dimension >& crs )
    {
        const internal::GeographicCoordinateTransformation transformation{
            crs.info(), info()
        };
        auto points = transformation.transform( crs );
        for( const auto p : Indices{ points } )
        {
            this->set_point( p, std::move( points[p] ) );
        }
    }

    class internal::GeographicCoordinateTransformation::Impl
    {
    public:
        Impl( const GeographicCoordinateSystemInfo& from,
            const GeographicCoordinateSystemInfo& to )
        {
            OGRSpatialReference origin;
            set_spatial_reference( origin, from );
            OGRSpatialReference destination;
            set_spatial_reference( destination, to );
            transformer_.reset(
                OGRCreateCoordinateTransformation( &origin, &destination ) );
            OpenGeodeGeosciencesExplicitException::check_exception(
                transformer_ != nullptr, nullptr,
                OpenGeodeException::TYPE::data,
                "[GeographicCoordinateTransformation] Cannot create "
                "coordinate transformation from ",
                from.authority_code(), " to ", to.authority_code() );
        }

        template < index_t dimension >
        std::vector< Point< dimension > > transform(
            const GeographicCoordinateSystem< dimension >& from ) const
        {
            const auto nb_points = from.nb_points();
            std::array< std::vector< double >, 3 > values{
                std::vector< double >( nb_points, 0 ),
                std::vector< double >( nb_points, 0 ),
                std::vector< double >( nb_points, 0 )
            };
            for( const auto p : Range{ nb_points } )
            {
                const auto& point = from.point( p );
                for( const auto d : LRange{ dimension } )
                {
                    values[d][p] = point.value( d );
                }
            }
            if( nb_points != 0 )
            {
                std::vector< int > success( nb_points, FALSE );
                const auto status =
                    transformer_->Transform( nb_points, values[0].data(),
                        values[1].data(), values[2].data(), success.data() );
                const auto nb_failures =
                    static_cast< index_t >( absl::c_count( success, FALSE ) );
                OpenGeodeGeosciencesExplicitException::check_exception(
                    status && nb_failures == 0, nullptr,
                    OpenGeodeException::TYPE::data,
                    "[GeographicCoordinateTransformation] Failed to convert "
                    "coordinates of ",
                    nb_failures, " points" );
            }
            std::vector< Point< dimension > > points( nb_points );
            for( const auto p : Range{ nb_points } )
            {
                for( const auto d : LRange{ dimension } )
                {
                    points[p].set_value( d, values[d][p] );
                }
            }
            return points;
        }

    private:
        static void set_spatial_reference( OGRSpatialReference& reference,
            const GeographicCoordinateSystemInfo& info )
        {
            const auto authority_code = info.authority_code();
            OpenGeodeGeosciencesExplicitException::check_exception(
                reference.SetFromUserInput( authority_code.c_str() )
                    == OGRERR_NONE,
                nullptr, OpenGeodeException::TYPE::data,
                "[GeographicCoordinateTransformation] Unknown coordinate "
                "system ",
                authority_code );
            reference.SetAxisMappingStrategy( OAMS_TRADITIONAL_GIS_ORDER );
        }

    private:
        std::unique_ptr< OGRCoordinateTransformation,
            decltype( &OGRCoordinateTransformation::DestroyCT ) >
            transformer_{ nullptr, &OGRCoordinateTransformation::DestroyCT };
    };

    internal::GeographicCoordinateTransformation::
        GeographicCoordinateTransformation(
            const GeographicCoordinateSystemInfo& from,
            const GeographicCoordinateSystemInfo& to )
        : impl_{ from, to }
    {
    }

    internal::GeographicCoordinateTransformation::
        GeographicCoordinateTransformation(
            GeographicCoordinateTransformation&& ) noexcept = default;

    internal::GeographicCoordinateTransformation::
        ~GeographicCoordinateTransformation() = default;

    template < index_t dimension >
    std::vector< Point< dimension > >
        internal::GeographicCoordinateTransformation::transform(
            const GeographicCoordinateSystem< dimension >& from ) const
    {
        return impl_->transform( from );
    }

    template std::vector< Point< 2 > > opengeode_geosciences_explicit_api
        internal::GeographicCoordinateTransformation::transform(
            const GeographicCoordinateSystem< 2 >& ) const;
    template std::vector< Point< 3 > > opengeode_geosciences_explicit_api
        internal::GeographicCoordinateTransformation::transform(
            const GeographicCoordinateSystem< 3 >& ) const;

    template class opengeode_geosciences_explicit_api
        GeographicCoordinateSystem< 2 >;
    template class opengeode_geosciences_explicit_api
        GeographicCoordinateSystem< 3 >;

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_explicit_api, GeographicCoordinateSystem< 2 > );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_explicit_api, GeographicCoordinateSystem< 3 > );
} // namespace geode