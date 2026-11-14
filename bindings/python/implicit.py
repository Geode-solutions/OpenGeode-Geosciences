# Copyright (c) 2019 - 2026 Geode-solutions
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

"""OpenGeode-Geosciences implicit models: HorizonsStack, ImplicitStructuralModel, ImplicitCrossSection, StratigraphicModel and StratigraphicSection, their builders and file formats. Importing this module initializes the library, with the explicit library it depends on, and imports the explicit extension module (opengeode_geosciences_py_explicit); the explicit module of the package is only imported with the package."""

import opengeode

from opengeode_geosciences_py_implicit import *

OpenGeodeGeosciencesImplicitLibrary.initialize()

# The generic functions of opengeode (load, save, builder...) use the types
# registered here
for _cls, _builder, _name in [
    (ImplicitStructuralModel, ImplicitStructuralModelBuilder, "implicit_structural_model"),
    (ImplicitCrossSection, ImplicitCrossSectionBuilder, "implicit_cross_section"),
    (StratigraphicModel, StratigraphicModelBuilder, "stratigraphic_model"),
    (StratigraphicSection, StratigraphicSectionBuilder, "stratigraphic_section"),
]:
    opengeode.register_type(
        _cls,
        load=globals()["load_" + _name],
        save=globals()["save_" + _name],
        is_loadable=globals()["is_" + _name + "_loadable"],
        is_saveable=globals()["is_" + _name + "_saveable"],
        input_extensions=globals()[_cls.__name__ + "InputFactory"].list_creators,
        output_extensions=globals()[_cls.__name__ + "OutputFactory"].list_creators,
        additional_files=globals()[_name + "_additional_files"],
        builder=_builder,
    )
for _dimension in ["2D", "3D"]:
    opengeode.register_type(
        globals()["HorizonsStack" + _dimension],
        load=globals()["load_horizons_stack" + _dimension],
        save=globals()["save_horizons_stack" + _dimension],
        is_loadable=globals()["is_horizons_stack_loadable" + _dimension],
        is_saveable=globals()["is_horizons_stack_saveable" + _dimension],
        input_extensions=globals()["HorizonsStackInputFactory" + _dimension].list_creators,
        output_extensions=globals()["HorizonsStackOutputFactory" + _dimension].list_creators,
        additional_files=globals()["horizons_stack_additional_files" + _dimension],
        builder=globals()["HorizonsStackBuilder" + _dimension],
    )
