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

#include <geode/basic/pimpl.hpp>

#include <geode/mesh/core/attribute_coordinate_reference_system.hpp>

#include <geode/geosciences/explicit/common.hpp>

namespace geode
{
    class AttributeManager;
} // namespace geode

namespace geode
{
    struct GeographicCoordinateSystemInfo
    {
        GeographicCoordinateSystemInfo(
            std::string authority_in, std::string code_in, std::string name_in )
            : authority{ std::move( authority_in ) },
              code{ std::move( code_in ) },
              name{ std::move( name_in ) }
        {
        }
        GeographicCoordinateSystemInfo() = default;
        ~GeographicCoordinateSystemInfo() = default;

        [[nodiscard]] std::string authority_code() const
        {
            return absl::StrCat( authority, ":", code );
        }

        [[nodiscard]] std::string string() const
        {
            return absl::StrCat( "(", authority_code(), " -> ", name, ")" );
        }

        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this,
                Growable< Archive, GeographicCoordinateSystemInfo >{
                    { []( Archive& a, GeographicCoordinateSystemInfo& info ) {
                        a.text1b( info.authority, info.authority.max_size() );
                        a.text1b( info.code, info.code.max_size() );
                        a.text1b( info.name, info.name.max_size() );
                    } } } );
        }

        std::string authority;
        std::string code;
        std::string name;
    };

    template < index_t dimension >
    class GeographicCoordinateSystem
        : public AttributeCoordinateReferenceSystem< dimension >
    {
        friend class bitsery::Access;

    public:
        GeographicCoordinateSystem(
            AttributeManager& manager, GeographicCoordinateSystemInfo info );
        ~GeographicCoordinateSystem();

        [[nodiscard]] static GeographicCoordinateSystem< dimension >
            create_from_attribute(
                const AttributeCoordinateReferenceSystem< dimension >& crs,
                AttributeManager& manager,
                GeographicCoordinateSystemInfo info );

        [[nodiscard]] static CRSType type_name_static()
        {
            return CRSType{ "GeographicCoordinateSystem" };
        }

        [[nodiscard]] CRSType type_name() const override
        {
            return type_name_static();
        }

        [[nodiscard]] const GeographicCoordinateSystemInfo& info() const;

        [[nodiscard]] static absl::FixedArray< GeographicCoordinateSystemInfo >
            geographic_coordinate_systems();

        void import_coordinates(
            const GeographicCoordinateSystem< dimension >& crs );

    private:
        GeographicCoordinateSystem();

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( GeographicCoordinateSystem );
} // namespace geode