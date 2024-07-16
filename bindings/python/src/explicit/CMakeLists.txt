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

add_geode_python_binding(
    NAME "py_explicit"
    SOURCES
        "explicit.cpp"
        "geometry/crs.hpp"
        "geometry/crs_helper.hpp"
        "mixin/builder/fault_blocks_builder.hpp"
        "mixin/builder/faults_builder.hpp"
        "mixin/builder/horizons_builder.hpp"
        "mixin/builder/stratigraphic_units_builder.hpp"
        "mixin/core/fault.hpp"
        "mixin/core/fault_block.hpp"
        "mixin/core/fault_blocks.hpp"
        "mixin/core/faults.hpp"
        "mixin/core/horizon.hpp"
        "mixin/core/horizons.hpp"
        "mixin/core/stratigraphic_unit.hpp"
        "mixin/core/stratigraphic_units.hpp"
        "representation/builder/cross_section_builder.hpp"
        "representation/builder/structural_model_builder.hpp"
        "representation/core/cross_section.hpp"
        "representation/core/structural_model.hpp"
        "representation/io/cross_section.hpp"
        "representation/io/structural_model.hpp"
    DEPENDENCIES
        ${PROJECT_NAME}::explicit
)
