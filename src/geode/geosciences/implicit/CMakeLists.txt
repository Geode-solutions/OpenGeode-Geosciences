#
# Copyright (c) 2019 - 2025 Geode-solutions
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
#

add_geode_library(
    NAME implicit
    FOLDER "geode/geosciences/implicit"
    SOURCES
        "common.cpp"
        "mixin/builder/stratigraphic_relationships_builder.cpp"
        "mixin/core/stratigraphic_relationships.cpp"
        "representation/builder/implicit_cross_section_builder.cpp"
        "representation/builder/implicit_structural_model_builder.cpp"
        "representation/builder/stratigraphic_model_builder.cpp"
        "representation/builder/stratigraphic_section_builder.cpp"
        "representation/builder/horizons_stack_builder.cpp"
        "representation/builder/helpers/implicit_structural_model_stratigraphic_blocks_builder.cpp"
        "representation/core/detail/helpers.cpp"
        "representation/core/implicit_cross_section.cpp"
        "representation/core/implicit_structural_model.cpp"
        "representation/core/stratigraphic_model.cpp"
        "representation/core/stratigraphic_section.cpp"
        "representation/core/horizons_stack.cpp"
        "representation/io/geode/geode_horizons_stack_input.cpp"
        "representation/io/geode/geode_implicit_cross_section_input.cpp"
        "representation/io/geode/geode_implicit_cross_section_output.cpp"
        "representation/io/geode/geode_implicit_structural_model_input.cpp"
        "representation/io/geode/geode_implicit_structural_model_output.cpp"
        "representation/io/implicit_cross_section_input.cpp"
        "representation/io/implicit_cross_section_output.cpp"
        "representation/io/implicit_structural_model_input.cpp"
        "representation/io/implicit_structural_model_output.cpp"
        "representation/io/stratigraphic_model_input.cpp"
        "representation/io/stratigraphic_model_output.cpp"
        "representation/io/stratigraphic_section_input.cpp"
        "representation/io/stratigraphic_section_output.cpp"
        "representation/io/horizons_stack_input.cpp"
        "representation/io/horizons_stack_output.cpp"
    PUBLIC_HEADERS
        "common.hpp"
        "geometry/stratigraphic_point.hpp"
        "mixin/builder/stratigraphic_relationships_builder.hpp"
        "mixin/core/stratigraphic_relationships.hpp"
        "representation/builder/implicit_cross_section_builder.hpp"
        "representation/builder/implicit_structural_model_builder.hpp"
        "representation/builder/stratigraphic_model_builder.hpp"
        "representation/builder/stratigraphic_section_builder.hpp"
        "representation/builder/horizons_stack_builder.hpp"
        "representation/builder/helpers/implicit_structural_model_stratigraphic_blocks_builder.hpp"
        "representation/core/detail/helpers.hpp"
        "representation/core/implicit_cross_section.hpp"
        "representation/core/implicit_structural_model.hpp"
        "representation/core/stratigraphic_model.hpp"
        "representation/core/stratigraphic_section.hpp"
        "representation/core/horizons_stack.hpp"
        "representation/io/geode/geode_horizons_stack_input.hpp"
        "representation/io/geode/geode_horizons_stack_output.hpp"
        "representation/io/geode/geode_implicit_cross_section_input.hpp"
        "representation/io/geode/geode_implicit_cross_section_output.hpp"
        "representation/io/geode/geode_implicit_structural_model_input.hpp"
        "representation/io/geode/geode_implicit_structural_model_output.hpp"
        "representation/io/implicit_cross_section_input.hpp"
        "representation/io/implicit_cross_section_output.hpp"
        "representation/io/implicit_structural_model_input.hpp"
        "representation/io/implicit_structural_model_output.hpp"
        "representation/io/horizons_stack_input.hpp"
        "representation/io/horizons_stack_output.hpp"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
        ${PROJECT_NAME}::explicit
    PRIVATE_DEPENDENCIES
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
        Async++
)