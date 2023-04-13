# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_geosciences_py_explicit
import opengeode_geosciences_py_implicit as geode_imp

def test_stratigraphic_stack():
    units_stack = geode_imp.StratigraphicUnitsStack3D()
    stack_builder = geode_imp.StratigraphicUnitsStackBuilder3D( units_stack )

    hor0 = stack_builder.add_horizon()
    hor1 = stack_builder.add_horizon()
    hor2 = stack_builder.add_horizon()
    unit0 = stack_builder.add_stratigraphic_unit()
    unit1 = stack_builder.add_stratigraphic_unit()
    if units_stack.nb_horizons() != 3:
        raise ValueError( "[TEST] Stratigraphic Units Stack should have 3 horizons." )
    if units_stack.nb_stratigraphic_units() != 2:
        raise ValueError( "[TEST] Stratigraphic Units Stack should have 2 Stratigraphic Units." )
    stack_builder.add_horizon_under( units_stack.horizon( hor0 ), units_stack.stratigraphic_unit( unit0 ) )
    stack_builder.add_horizon_above( units_stack.horizon( hor1 ), units_stack.stratigraphic_unit( unit0 ) )
    stack_builder.add_horizon_under( units_stack.horizon( hor1 ), units_stack.stratigraphic_unit( unit1 ) )
    stack_builder.add_horizon_above( units_stack.horizon( hor2 ), units_stack.stratigraphic_unit( unit1 ) )

    for unit in units_stack.stratigraphic_units():
        unit_id = unit.id()
        if not units_stack.above( unit_id ):
            raise ValueError( "[TEST] Unit should have a horizon above." )
        if not units_stack.under( unit_id ):
            raise ValueError("[TEST] Unit should have a horizon under." )
    if units_stack.above( units_stack.above( unit0 ) ).string() != unit1.string():
        raise ValueError( "[TEST] Horizon 1 should be found from horizon 0." )

    stack_path = "test_SUStack." + units_stack.native_extension()
    geode_imp.save_stratigraphic_units_stack3D( units_stack, stack_path )
    reloaded_stack = geode_imp.load_stratigraphic_units_stack3D( stack_path )

if __name__ == '__main__':
    print("Starting test")
    geode_imp.GeosciencesImplicitLibrary.initialize()

    test_stratigraphic_stack()