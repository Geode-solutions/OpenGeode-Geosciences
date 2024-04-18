# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2024 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_geosciences_py_explicit
import opengeode_geosciences_py_implicit as geode_imp

def test_horizons_stack():
    horizons_stack = geode_imp.HorizonsStack3D()
    stack_builder = geode_imp.HorizonsStackBuilder3D( horizons_stack )

    hor0 = stack_builder.add_horizon()
    hor1 = stack_builder.add_horizon()
    hor2 = stack_builder.add_horizon()
    unit0 = stack_builder.add_stratigraphic_unit()
    unit1 = stack_builder.add_stratigraphic_unit()
    if horizons_stack.nb_horizons() != 3:
        raise ValueError( "[TEST] Stratigraphic Units Stack should have 3 horizons." )
    if horizons_stack.nb_stratigraphic_units() != 2:
        raise ValueError( "[TEST] Stratigraphic Units Stack should have 2 Stratigraphic Units." )
    stack_builder.add_horizon_under( horizons_stack.horizon( hor0 ), horizons_stack.stratigraphic_unit( unit0 ) )
    stack_builder.add_horizon_above( horizons_stack.horizon( hor1 ), horizons_stack.stratigraphic_unit( unit0 ) )
    stack_builder.add_horizon_under( horizons_stack.horizon( hor1 ), horizons_stack.stratigraphic_unit( unit1 ) )
    stack_builder.add_horizon_above( horizons_stack.horizon( hor2 ), horizons_stack.stratigraphic_unit( unit1 ) )

    for unit in horizons_stack.stratigraphic_units():
        unit_id = unit.id()
        if not horizons_stack.above( unit_id ):
            raise ValueError( "[TEST] Unit should have a horizon above." )
        if not horizons_stack.under( unit_id ):
            raise ValueError("[TEST] Unit should have a horizon under." )
    if horizons_stack.above( horizons_stack.above( unit0 ) ).string() != unit1.string():
        raise ValueError( "[TEST] Horizon 1 should be found from horizon 0." )

    stack_path = "test_HorizonStack." + horizons_stack.native_extension()
    geode_imp.save_horizons_stack3D( horizons_stack, stack_path )
    reloaded_stack = geode_imp.load_horizons_stack3D( stack_path )

    stack_from_name_lists = geode_imp.horizons_stack_from_name_list_3d(["hor1","hor2"],["su1","su2","su3"])

if __name__ == '__main__':
    print("Starting test")
    geode_imp.GeosciencesImplicitLibrary.initialize()

    test_horizons_stack()