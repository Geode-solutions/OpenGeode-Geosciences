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

"""OpenGeode-Geosciences explicit models: StructuralModel and CrossSection (a BRep and a Section with Faults, Horizons, FaultBlocks and StratigraphicUnits), their builders and file formats, and the geographic coordinate systems. Importing this module initializes the library."""

import opengeode

from opengeode_geosciences_py_explicit import *

OpenGeodeGeosciencesExplicitLibrary.initialize()

# The generic functions of opengeode (load, save, builder...) use the types
# registered here
opengeode.register_type(
    StructuralModel,
    load=load_structural_model,
    save=save_structural_model,
    is_loadable=is_structural_model_loadable,
    is_saveable=is_structural_model_saveable,
    input_extensions=StructuralModelInputFactory.list_creators,
    output_extensions=StructuralModelOutputFactory.list_creators,
    additional_files=structural_model_additional_files,
    builder=StructuralModelBuilder,
)
opengeode.register_type(
    CrossSection,
    load=load_cross_section,
    save=save_cross_section,
    is_loadable=is_cross_section_loadable,
    is_saveable=is_cross_section_saveable,
    input_extensions=CrossSectionInputFactory.list_creators,
    output_extensions=CrossSectionOutputFactory.list_creators,
    additional_files=cross_section_additional_files,
    builder=CrossSectionBuilder,
)
