/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class StratigraphicSection;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a StratigraphicSection.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    StratigraphicSection opengeode_geosciences_implicit_api
        load_stratigraphic_section( absl::string_view filename );

    class StratigraphicSectionInput : public Input< StratigraphicSection >
    {
    public:
        virtual ~StratigraphicSectionInput() = default;

    protected:
        StratigraphicSectionInput( absl::string_view filename )
            : Input< StratigraphicSection >{ filename }
        {
        }
    };

    using StratigraphicSectionInputFactory =
        Factory< std::string, StratigraphicSectionInput, absl::string_view >;
} // namespace geode
