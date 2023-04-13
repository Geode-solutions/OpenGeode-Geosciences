/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/input.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitStructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading an ImplicitStructuralModel.
     * The adequate loader is called depending on the filename extension.
     * @param[in] filename Path to the file to load.
     */
    ImplicitStructuralModel opengeode_geosciences_implicit_api
        load_implicit_structural_model( absl::string_view filename );

    class ImplicitStructuralModelInput : public Input< ImplicitStructuralModel >
    {
    public:
        virtual ~ImplicitStructuralModelInput() = default;

    protected:
        ImplicitStructuralModelInput( absl::string_view filename )
            : Input< ImplicitStructuralModel >{ filename }
        {
        }
    };

    using ImplicitStructuralModelInputFactory =
        Factory< std::string, ImplicitStructuralModelInput, absl::string_view >;
} // namespace geode
