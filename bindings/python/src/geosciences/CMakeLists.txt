# Copyright (c) 2019 - 2022 Geode-solutions
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

add_geode_python_binding(
    NAME "py_geosciences"
    SOURCES
        "geosciences.cpp"
        "factory.h"
        "mixin/builder/fault_blocks_builder.h"
        "mixin/builder/faults_builder.h"
        "mixin/builder/horizons_builder.h"
        "mixin/builder/stratigraphic_units_builder.h"
        "mixin/core/fault.h"
        "mixin/core/fault_block.h"
        "mixin/core/fault_blocks.h"
        "mixin/core/faults.h"
        "mixin/core/horizon.h"
        "mixin/core/horizons.h"
        "mixin/core/stratigraphic_unit.h"
        "mixin/core/stratigraphic_units.h"
        "representation/builder/cross_section_builder.h"
        "representation/builder/structural_model_builder.h"
        "representation/core/cross_section.h"
        "representation/core/structural_model.h"
        "representation/io/cross_section.h"
        "representation/io/structural_model.h"
    DEPENDENCIES
        ${PROJECT_NAME}::geosciences
)
