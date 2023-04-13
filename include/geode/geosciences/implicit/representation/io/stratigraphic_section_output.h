/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class StratigraphicSection;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a StratigraphicSection.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] stratigraphic_section StratigraphicSection to save.
     * @param[in] filename Path to the file where save the
     * StratigraphicSection.
     */
    void opengeode_geosciences_implicit_api save_stratigraphic_section(
        const StratigraphicSection& stratigraphic_section,
        absl::string_view filename );

    class StratigraphicSectionOutput : public Output< StratigraphicSection >
    {
    public:
        virtual ~StratigraphicSectionOutput() = default;

    protected:
        StratigraphicSectionOutput( absl::string_view filename )
            : Output< StratigraphicSection >{ filename }
        {
        }
    };

    using StratigraphicSectionOutputFactory =
        Factory< std::string, StratigraphicSectionOutput, absl::string_view >;
} // namespace geode
