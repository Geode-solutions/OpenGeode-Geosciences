/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geometry/point.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    /*!
     * A Stratigraphic Point represents a point in a stratigraphic space, with
     * an implicit field value and a point of dimension-1 giving its position in
     * the stratigraphic space.
     */
    template < index_t dimension >
    class StratigraphicPoint
    {
        static constexpr auto location_dim = dimension - 1;

    public:
        StratigraphicPoint()
            : stratigraphic_location_{ Point< location_dim >() },
              implicit_value_{ 0 }
        {
        }
        StratigraphicPoint( Point< location_dim > stratigraphic_location,
            double implicit_value )
            : stratigraphic_location_{ std::move( stratigraphic_location ) },
              implicit_value_{ implicit_value }
        {
        }
        StratigraphicPoint( const std::array< double, dimension >& values )
            : implicit_value_{ values[dimension - 1] }
        {
            for( const auto d : LRange{ location_dim } )
            {
                stratigraphic_location_.set_value( d, values[d] );
            }
        }
        StratigraphicPoint(
            const Point< dimension >& stratigraphic_coordinates )
            : implicit_value_{ stratigraphic_coordinates.value(
                dimension - 1 ) }
        {
            for( const auto d : LRange{ location_dim } )
            {
                stratigraphic_location_.set_value(
                    d, stratigraphic_coordinates.value( d ) );
            }
        }

        const Point< location_dim >& stratigraphic_location() const
        {
            return stratigraphic_location_;
        }

        double implicit_value() const
        {
            return implicit_value_;
        }

        Point< dimension > stratigraphic_coordinates() const
        {
            Point< dimension > result;
            for( const auto d : LRange{ location_dim } )
            {
                result.set_value( d, stratigraphic_location_.value( d ) );
            }
            result.set_value( location_dim, implicit_value_ );
            return result;
        }

        void set_stratigraphic_location(
            Point< location_dim > stratigraphic_location )
        {
            stratigraphic_location_ = std::move( stratigraphic_location );
        }

        void set_implicit_value( double implicit_value )
        {
            implicit_value_ = implicit_value;
        }

        std::string string() const
        {
            std::ostringstream oss;
            oss.precision( std::numeric_limits< double >::digits10 );
            const auto* sep = "";
            for( const auto i : LRange{ location_dim } )
            {
                oss << sep << stratigraphic_location_.value( i );
                sep = " ";
            }
            oss << sep << implicit_value_;
            return oss.str();
        }

    private:
        Point< location_dim > stratigraphic_location_;
        double implicit_value_;
    };
    ALIAS_2D_AND_3D( StratigraphicPoint );
} // namespace geode
