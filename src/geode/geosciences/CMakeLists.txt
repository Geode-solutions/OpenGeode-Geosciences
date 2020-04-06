# Copyright (c) 2019 - 2020 Geode-solutions
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
    NAME geosciences
    FOLDER "geode/geosciences"
    SOURCES
        "common.cpp"
        "mixin/builder/faults_builder.cpp"
        "mixin/builder/fault_blocks_builder.cpp"
        "mixin/builder/horizons_builder.cpp"
        "mixin/builder/layers_builder.cpp"
        "mixin/core/bitsery_archive.cpp"
        "mixin/core/fault.cpp"
        "mixin/core/faults.cpp"
        "mixin/core/fault_block.cpp"
        "mixin/core/fault_blocks.cpp"
        "mixin/core/horizon.cpp"
        "mixin/core/horizons.cpp"
        "mixin/core/layer.cpp"
        "mixin/core/layers.cpp"
        "representation/builder/cross_section_builder.cpp"
        "representation/builder/structural_model_builder.cpp"
        "representation/core/structural_model.cpp"
        "representation/core/cross_section.cpp"
        "representation/io/cross_section_input.cpp"
        "representation/io/cross_section_output.cpp"
        "representation/io/structural_model_input.cpp"
        "representation/io/structural_model_output.cpp"
    PUBLIC_HEADERS
        "common.h"
        "mixin/builder/faults_builder.h"
        "mixin/builder/fault_blocks_builder.h"
        "mixin/builder/horizons_builder.h"
        "mixin/builder/layers_builder.h"
        "mixin/core/bitsery_archive.h"
        "mixin/core/fault.h"
        "mixin/core/faults.h"
        "mixin/core/fault_block.h"
        "mixin/core/fault_blocks.h"
        "mixin/core/horizon.h"
        "mixin/core/horizons.h"
        "mixin/core/layer.h"
        "mixin/core/layers.h"
        "representation/builder/cross_section_builder.h"
        "representation/builder/structural_model_builder.h"
        "representation/core/cross_section.h"
        "representation/core/structural_model.h"
        "representation/io/cross_section_input.h"
        "representation/io/cross_section_output.h"
        "representation/io/structural_model_input.h"
        "representation/io/structural_model_output.h"
    ADVANCED_HEADERS
        "mixin/core/detail/components_storage.h"
        "representation/io/detail/geode_cross_section_input.h"
        "representation/io/detail/geode_cross_section_output.h"
        "representation/io/detail/geode_structural_model_input.h"
        "representation/io/detail/geode_structural_model_output.h"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
        OpenGeode::model
)
