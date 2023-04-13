/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/basic/factory.h>
#include <geode/basic/output.h>

#include <geode/geosciences/implicit/common.h>

namespace geode
{
    class ImplicitStructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * API function for saving an ImplicitStructuralModel.
     * The adequate saver is called depending on the given filename extension.
     * @param[in] implicit_model ImplicitStructuralModel to save.
     * @param[in] filename Path to the file where save the
     * ImplicitStructuralModel.
     */
    void opengeode_geosciences_implicit_api save_implicit_structural_model(
        const ImplicitStructuralModel& implicit_model,
        absl::string_view filename );

    class opengeode_geosciences_implicit_api ImplicitStructuralModelOutput
        : public Output< ImplicitStructuralModel >
    {
    public:
        virtual ~ImplicitStructuralModelOutput() = default;

    protected:
        ImplicitStructuralModelOutput( absl::string_view filename )
            : Output< ImplicitStructuralModel >{ filename }
        {
        }
    };

    using ImplicitStructuralModelOutputFactory = Factory< std::string,
        ImplicitStructuralModelOutput,
        absl::string_view >;
} // namespace geode
