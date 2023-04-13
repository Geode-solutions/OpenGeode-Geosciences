/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitCrossSection;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an ImplicitCrossSection.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    ImplicitCrossSection opengeode_geosciences_implicit_api
        load_implicit_cross_section( absl::string_view filename );

    class ImplicitCrossSectionInput : public Input< ImplicitCrossSection >
    {
    public:
        virtual ~ImplicitCrossSectionInput() = default;

    protected:
        ImplicitCrossSectionInput( absl::string_view filename )
            : Input< ImplicitCrossSection >{ filename }
        {
        }
    };

    using ImplicitCrossSectionInputFactory =
        Factory< std::string, ImplicitCrossSectionInput, absl::string_view >;
} // namespace geode
