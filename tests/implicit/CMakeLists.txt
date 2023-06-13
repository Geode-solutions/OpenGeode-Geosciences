# Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.

add_geode_test(
    SOURCE "test-stratigraphic-model.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
        ${PROJECT_NAME}::explicit
        ${PROJECT_NAME}::implicit
)

add_geode_test(
    SOURCE "test-stratigraphic-section.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
        ${PROJECT_NAME}::explicit
        ${PROJECT_NAME}::implicit
)

add_geode_test(
    SOURCE "test-horizons-stack.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
        ${PROJECT_NAME}::explicit
        ${PROJECT_NAME}::implicit
)