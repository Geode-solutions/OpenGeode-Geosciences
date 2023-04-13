/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/geosciences/explicit/common.h>

#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_stratigraphic_units_stack_input.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_stratigraphic_units_stack_output.h>

namespace
{
    void register_stratigraphic_units_stack_input()
    {
        geode::StratigraphicUnitsStackInputFactory2D::register_creator<
            geode::OpenGeodeStratigraphicUnitsStackInput2D >(
            geode::OpenGeodeStratigraphicUnitsStackOutput2D::extension()
                .data() );
        geode::StratigraphicUnitsStackInputFactory3D::register_creator<
            geode::OpenGeodeStratigraphicUnitsStackInput3D >(
            geode::OpenGeodeStratigraphicUnitsStackOutput3D::extension()
                .data() );
    }

    void register_stratigraphic_units_stack_output()
    {
        geode::StratigraphicUnitsStackOutputFactory2D::register_creator<
            geode::OpenGeodeStratigraphicUnitsStackOutput2D >(
            geode::OpenGeodeStratigraphicUnitsStackOutput2D::extension()
                .data() );
        geode::StratigraphicUnitsStackOutputFactory3D::register_creator<
            geode::OpenGeodeStratigraphicUnitsStackOutput3D >(
            geode::OpenGeodeStratigraphicUnitsStackOutput3D::extension()
                .data() );
    }
} // namespace

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( GeosciencesImplicit )
    {
        GeosciencesExplicit::initialize();
        register_stratigraphic_units_stack_input();
        register_stratigraphic_units_stack_output();
    }
} // namespace geode
