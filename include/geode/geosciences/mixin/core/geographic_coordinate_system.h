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

#pragma once

#include <geode/basic/pimpl.h>

#include <geode/geosciences/common.h>
#include <geode/mesh/core/coordinate_reference_system.h>

namespace geode
{
    class AttributeManager;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class GeographicCoordinateSystem
        : public CoordinateReferenceSystem< dimension >
    {
        friend class bitsery::Access;

    public:
        struct Info
        {
            Info( std::string authority_in,
                std::string code_in,
                std::string name_in )
                : authority{ std::move( authority_in ) },
                  code{ std::move( code_in ) },
                  name{ std::move( name_in ) }
            {
            }
            Info() = default;

            std::string authority_code() const
            {
                return absl::StrCat( authority, ":", code );
            }

            std::string string() const
            {
                return absl::StrCat( "(", authority_code(), " -> ", name, ")" );
            }

            template < typename Archive >
            void serialize( Archive& archive )
            {
                archive.ext( *this,
                    Growable< Archive, Info >{ { []( Archive& a, Info& info ) {
                        a.text1b( info.authority, info.authority.max_size() );
                        a.text1b( info.code, info.code.max_size() );
                        a.text1b( info.name, info.name.max_size() );
                    } } } );
            }

            std::string authority;
            std::string code;
            std::string name;
        };

    public:
        GeographicCoordinateSystem( AttributeManager& manager, Info info );
        ~GeographicCoordinateSystem();

        static CRSType type_name_static()
        {
            return CRSType{ "GeographicCoordinateSystem" };
        }

        CRSType type_name() const override
        {
            return type_name_static();
        }

        const Info& info() const;

        const Point< dimension >& point( index_t point_id ) const override;

        void set_point( index_t point_id, Point< dimension > point ) override;

        static absl::FixedArray< Info > geographic_coordinate_systems();

        void convert_geographic_coordinate_system(
            GeographicCoordinateSystem< dimension >& target,
            index_t nb_points ) const;

    private:
        GeographicCoordinateSystem();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( GeographicCoordinateSystem );
} // namespace geode