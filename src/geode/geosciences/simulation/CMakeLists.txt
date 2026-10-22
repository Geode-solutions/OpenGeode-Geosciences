#
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

add_geode_library(
    NAME simulation
    FOLDER "geode/geosciences/simulation"
    SOURCES
        "builder/physical_problem_builder.cpp"
        "common.cpp"
        "core/bitsery_archive.cpp"
        "core/flow_model.cpp"
        "core/fluid.cpp"
        "core/initial_condition.cpp"
        "core/material.cpp"
        "core/physical_problem.cpp"
        "core/saturation_functions.cpp"
        "core/schedule.cpp"
        "core/solver_options.cpp"
        "core/source_term.cpp"
        "core/thermal_parameters.cpp"
        "helpers/physical_problem_validation.cpp"
        "io/geode/geode_physical_problem_input.cpp"
        "io/geode/geode_physical_problem_output.cpp"
        "io/physical_problem_input.cpp"
        "io/physical_problem_output.cpp"
    PUBLIC_HEADERS
        "builder/physical_problem_builder.hpp"
        "common.hpp"
        "core/bitsery_archive.hpp"
        "core/flow_model.hpp"
        "core/fluid.hpp"
        "core/initial_condition.hpp"
        "core/material.hpp"
        "core/physical_problem.hpp"
        "core/saturation_functions.hpp"
        "core/schedule.hpp"
        "core/solver_options.hpp"
        "core/source_term.hpp"
        "core/thermal_parameters.hpp"
        "helpers/physical_problem_validation.hpp"
        "io/physical_problem_input.hpp"
        "io/physical_problem_output.hpp"
    ADVANCED_HEADERS
        "io/geode/geode_physical_problem_input.hpp"
        "io/geode/geode_physical_problem_output.hpp"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
    PRIVATE_DEPENDENCIES
        OpenGeode::model
)
