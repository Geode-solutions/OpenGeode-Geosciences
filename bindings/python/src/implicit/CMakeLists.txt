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
    NAME "py_implicit"
    SOURCES
        "implicit.cpp"
        "geometry/stratigraphic_point.hpp"
        "mixin/builder/stratigraphic_relationships_builder.hpp"
        "mixin/core/stratigraphic_relationships.hpp"
        "representation/builder/implicit_cross_section_builder.hpp"
        "representation/builder/implicit_structural_model_builder.hpp"
        "representation/builder/stratigraphic_model_builder.hpp"
        "representation/builder/stratigraphic_section_builder.hpp"
        "representation/builder/horizons_stack_builder.hpp"
        "representation/core/helpers.hpp"
        "representation/core/implicit_cross_section.hpp"
        "representation/core/implicit_structural_model.hpp"
        "representation/core/stratigraphic_model.hpp"
        "representation/core/stratigraphic_section.hpp"
        "representation/core/horizons_stack.hpp"
        "representation/io/implicit_cross_section.hpp"
        "representation/io/implicit_structural_model.hpp"
        "representation/io/stratigraphic_model.hpp"
        "representation/io/stratigraphic_section.hpp"
        "representation/io/horizons_stack.hpp"
    DEPENDENCIES
        ${PROJECT_NAME}::implicit
)
