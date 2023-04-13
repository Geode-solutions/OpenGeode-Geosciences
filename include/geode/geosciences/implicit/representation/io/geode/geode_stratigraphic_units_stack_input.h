/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#pragma once

#include <geode/geosciences/implicit/representation/core/stratigraphic_units_stack.h>
#include <geode/geosciences/implicit/representation/io/stratigraphic_units_stack_input.h>

namespace geode
{
    template < index_t dimension >
    class OpenGeodeStratigraphicUnitsStackInput final
        : public StratigraphicUnitsStackInput< dimension >
    {
    public:
        OpenGeodeStratigraphicUnitsStackInput( absl::string_view filename )
            : StratigraphicUnitsStackInput< dimension >( filename )
        {
        }

        static absl::string_view extension()
        {
            return StratigraphicUnitsStack<
                dimension >::native_extension_static();
        }

        void load_stratigraphic_units_stack_files(
            StratigraphicUnitsStack< dimension >& stratigraphic_units_stack,
            absl::string_view directory );

        StratigraphicUnitsStack< dimension > read() final;
    };
    ALIAS_2D_AND_3D( OpenGeodeStratigraphicUnitsStackInput );
} // namespace geode
