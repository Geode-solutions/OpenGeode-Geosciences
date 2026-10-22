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

#include <geode/geosciences/simulation/io/geode/geode_physical_problem_input.hpp>

#include <fstream>

#include <geode/basic/bitsery_archive.hpp>

#include <geode/geosciences/simulation/core/physical_problem.hpp>

namespace geode
{
    PhysicalProblem OpenGeodePhysicalProblemInput::read()
    {
        std::ifstream file{ to_string( this->filename() ),
            std::ifstream::binary };
        OpenGeodeGeosciencesSimulationException::check_exception( !file.fail(),
            nullptr, OpenGeodeException::TYPE::data,
            "[PhysicalProblemInput] Failed to open file: ", this->filename() );
        TContext context{};
        BitseryExtensions::register_deserialize_pcontext(
            std::get< 0 >( context ) );
        Deserializer archive{ context, file };
        PhysicalProblem problem;
        archive.object( problem );
        const auto& adapter = archive.adapter();
        OpenGeodeGeosciencesSimulationException::check_exception(
            adapter.error() == bitsery::ReaderError::NoError
                && adapter.isCompletedSuccessfully()
                && std::get< 1 >( context ).isValid(),
            nullptr, OpenGeodeException::TYPE::internal,
            "[Bitsery::read] Error while reading file: ", this->filename() );
        return problem;
    }
} // namespace geode
