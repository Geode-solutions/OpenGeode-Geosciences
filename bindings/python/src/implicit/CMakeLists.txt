# Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.

add_geode_python_binding(
    NAME "py_implicit"
    SOURCES
        "implicit.cpp"
        "geometry/stratigraphic_point.h"
        "mixin/builder/stratigraphic_relationships_builder.h"
        "mixin/core/stratigraphic_relationships.h"
        "representation/builder/implicit_cross_section_builder.h"
        "representation/builder/implicit_structural_model_builder.h"
        "representation/builder/stratigraphic_model_builder.h"
        "representation/builder/stratigraphic_section_builder.h"
        "representation/builder/horizons_stack_builder.h"
        "representation/core/helpers.h"
        "representation/core/implicit_cross_section.h"
        "representation/core/implicit_structural_model.h"
        "representation/core/stratigraphic_model.h"
        "representation/core/stratigraphic_section.h"
        "representation/core/horizons_stack.h"
        "representation/io/implicit_cross_section.h"
        "representation/io/implicit_structural_model.h"
        "representation/io/stratigraphic_model.h"
        "representation/io/stratigraphic_section.h"
        "representation/io/horizons_stack.h"
    DEPENDENCIES
        ${PROJECT_NAME}::implicit
)
