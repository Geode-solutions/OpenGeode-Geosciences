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

"""OpenGeode-Geosciences simulation: PhysicalProblem, the simulator independent description of a flow, solid mechanics or poromechanics simulation, its builder and file format, and the checks that a simulator can simulate it on a model. Importing this module initializes the library."""

import opengeode

from opengeode_geosciences_py_simulation import *

OpenGeodeGeosciencesSimulationLibrary.initialize()

# The generic functions of opengeode (load, save, builder...) use the types
# registered here
opengeode.register_type(
    PhysicalProblem,
    load=load_physical_problem,
    save=save_physical_problem,
    is_loadable=is_physical_problem_loadable,
    is_saveable=is_physical_problem_saveable,
    input_extensions=PhysicalProblemInputFactory.list_creators,
    output_extensions=PhysicalProblemOutputFactory.list_creators,
    additional_files=physical_problem_additional_files,
    builder=PhysicalProblemBuilder,
)
