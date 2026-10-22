/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <geode/geosciences/simulation/common.hpp>

#include <geode/basic/bitsery_archive.hpp>

#include <geode/geometry/common.hpp>

#include <geode/geosciences/simulation/core/bitsery_archive.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/io/geode/geode_physical_problem_input.hpp>
#include <geode/geosciences/simulation/io/geode/geode_physical_problem_output.hpp>

namespace
{
    void register_physical_problem_input()
    {
        geode::PhysicalProblemInputFactory::register_creator<
            geode::OpenGeodePhysicalProblemInput >(
            geode::PhysicalProblem::native_extension_static().data() );
    }

    void register_physical_problem_output()
    {
        geode::PhysicalProblemOutputFactory::register_creator<
            geode::OpenGeodePhysicalProblemOutput >(
            geode::PhysicalProblem::native_extension_static().data() );
    }
} // namespace

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( OpenGeodeGeosciences, Simulation )
    {
        OpenGeodeGeometryLibrary::initialize();
        register_physical_problem_input();
        register_physical_problem_output();
        BitseryExtensions::register_functions(
            register_simulation_serialize_pcontext,
            register_simulation_deserialize_pcontext );
    }
} // namespace geode
