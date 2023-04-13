/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitCrossSection;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving an ImplicitCrossSection.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] implicit_model ImplicitCrossSection to save.
     * @param[in] filename Path to the file where save the
     * ImplicitCrossSection.
     */
    void opengeode_geosciences_implicit_api save_implicit_cross_section(
        const ImplicitCrossSection& implicit_model,
        absl::string_view filename );

    class ImplicitCrossSectionOutput : public Output< ImplicitCrossSection >
    {
    public:
        virtual ~ImplicitCrossSectionOutput() = default;

    protected:
        ImplicitCrossSectionOutput( absl::string_view filename )
            : Output< ImplicitCrossSection >{ filename }
        {
        }
    };

    using ImplicitCrossSectionOutputFactory =
        Factory< std::string, ImplicitCrossSectionOutput, absl::string_view >;
} // namespace geode
