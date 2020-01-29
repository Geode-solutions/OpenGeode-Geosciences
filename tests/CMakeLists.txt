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

cmake_minimum_required(VERSION 3.7.2)

if(NOT TARGET OpenGeode-Geosciences::geosciences)
    project(OpenGeode-Geosciences CXX)
    find_package(OpenGeode REQUIRED CONFIG)
    find_package(OpenGeode-Geosciences REQUIRED CONFIG)
    enable_testing()
endif()

set(DATA_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/data)
set(test_config_file_in ${CMAKE_CURRENT_LIST_DIR}/tests_config.h.in)
set(test_config_file ${PROJECT_BINARY_DIR}/geode/tests_config.h)
configure_file(${test_config_file_in} ${test_config_file})
include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(geosciences)
