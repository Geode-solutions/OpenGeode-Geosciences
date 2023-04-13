/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class StratigraphicModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a StratigraphicModel.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    StratigraphicModel opengeode_geosciences_implicit_api
        load_stratigraphic_model( absl::string_view filename );

    class StratigraphicModelInput : public Input< StratigraphicModel >
    {
    public:
        virtual ~StratigraphicModelInput() = default;

    protected:
        StratigraphicModelInput( absl::string_view filename )
            : Input< StratigraphicModel >{ filename }
        {
        }
    };

    using StratigraphicModelInputFactory =
        Factory< std::string, StratigraphicModelInput, absl::string_view >;
} // namespace geode
