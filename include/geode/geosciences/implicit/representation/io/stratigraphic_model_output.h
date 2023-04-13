/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class StratigraphicModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving a StratigraphicModel.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] stratigraphic_model StratigraphicModel to save.
     * @param[in] filename Path to the file where save the
     * StratigraphicModel.
     */
    void opengeode_geosciences_implicit_api save_stratigraphic_model(
        const StratigraphicModel& stratigraphic_model,
        absl::string_view filename );

    class opengeode_geosciences_implicit_api StratigraphicModelOutput
        : public Output< StratigraphicModel >
    {
    public:
        virtual ~StratigraphicModelOutput() = default;

    protected:
        StratigraphicModelOutput( absl::string_view filename )
            : Output< StratigraphicModel >{ filename }
        {
        }
    };

    using StratigraphicModelOutputFactory =
        Factory< std::string, StratigraphicModelOutput, absl::string_view >;
} // namespace geode
