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

add_geode_library(
    NAME explicit
    FOLDER "geode/geosciences/explicit"
    SOURCES
        "common.cpp"
        "geometry/geographic_coordinate_system.cpp"
        "geometry/geographic_coordinate_system_helper.cpp"
        "mixin/builder/faults_builder.cpp"
        "mixin/builder/fault_blocks_builder.cpp"
        "mixin/builder/horizons_builder.cpp"
        "mixin/builder/stratigraphic_units_builder.cpp"
        "mixin/core/bitsery_archive.cpp"
        "mixin/core/fault.cpp"
        "mixin/core/faults.cpp"
        "mixin/core/fault_block.cpp"
        "mixin/core/fault_blocks.cpp"
        "mixin/core/horizon.cpp"
        "mixin/core/horizons.cpp"
        "mixin/core/stratigraphic_unit.cpp"
        "mixin/core/stratigraphic_units.cpp"
        "representation/builder/cross_section_builder.cpp"
        "representation/builder/structural_model_builder.cpp"
        "representation/core/detail/clone.cpp"
        "representation/core/cross_section.cpp"
        "representation/core/structural_model.cpp"
        "representation/io/cross_section_input.cpp"
        "representation/io/cross_section_output.cpp"
        "representation/io/structural_model_input.cpp"
        "representation/io/structural_model_output.cpp"
        "representation/io/geode/geode_cross_section_input.cpp"
        "representation/io/geode/geode_cross_section_output.cpp"
        "representation/io/geode/geode_structural_model_input.cpp"
        "representation/io/geode/geode_structural_model_output.cpp"
    PUBLIC_HEADERS
        "common.h"
        "geometry/geographic_coordinate_system.h"
        "geometry/geographic_coordinate_system_helper.h"
        "mixin/builder/faults_builder.h"
        "mixin/builder/fault_blocks_builder.h"
        "mixin/builder/horizons_builder.h"
        "mixin/builder/stratigraphic_units_builder.h"
        "mixin/core/bitsery_archive.h"
        "mixin/core/fault.h"
        "mixin/core/faults.h"
        "mixin/core/fault_block.h"
        "mixin/core/fault_blocks.h"
        "mixin/core/horizon.h"
        "mixin/core/horizons.h"
        "mixin/core/stratigraphic_unit.h"
        "mixin/core/stratigraphic_units.h"
        "representation/builder/cross_section_builder.h"
        "representation/builder/structural_model_builder.h"
        "representation/core/detail/clone.h"
        "representation/core/cross_section.h"
        "representation/core/structural_model.h"
        "representation/io/cross_section_input.h"
        "representation/io/cross_section_output.h"
        "representation/io/structural_model_input.h"
        "representation/io/structural_model_output.h"
    ADVANCED_HEADERS
        "representation/builder/detail/copy.h"
        "representation/io/geode/geode_cross_section_input.h"
        "representation/io/geode/geode_cross_section_output.h"
        "representation/io/geode/geode_structural_model_input.h"
        "representation/io/geode/geode_structural_model_output.h"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
        OpenGeode::model
    PRIVATE_DEPENDENCIES
        Async++
        GDAL::GDAL
)
