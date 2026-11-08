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

#include <geode/geosciences/simulation/io/physical_problem_output.hpp>

#include <geode/basic/detail/geode_output_impl.hpp>

#include <geode/geosciences/simulation/core/physical_problem.hpp>

namespace geode
{
    std::vector< std::string > save_physical_problem(
        const PhysicalProblem& problem, std::string_view filename )
    {
        constexpr auto TYPE = "PhysicalProblem";
        try
        {
            return detail::geode_object_output_impl<
                PhysicalProblemOutputFactory >( TYPE, problem, filename );
        }
        catch( const std::exception& exception )
        {
            detail::throw_geode_object_output_exception<
                OpenGeodeGeosciencesSimulationException,
                PhysicalProblemOutputFactory >( TYPE, filename, exception );
        }
    }

    bool is_physical_problem_saveable(
        const PhysicalProblem& problem, std::string_view filename )
    {
        try
        {
            const auto output = detail::geode_object_output_writer<
                PhysicalProblemOutputFactory >( filename );
            return output->is_saveable( problem );
        }
        catch( ... )
        {
            return false;
        }
    }
} // namespace geode
