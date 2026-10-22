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

#include <geode/geosciences/simulation/io/geode/geode_physical_problem_output.hpp>

#include <exception>
#include <filesystem>
#include <fstream>

#include <geode/basic/bitsery_archive.hpp>

#include <geode/geosciences/simulation/core/physical_problem.hpp>

namespace
{
    void write_physical_problem(
        const geode::PhysicalProblem& problem, std::string_view filename )
    {
        std::ofstream file{ geode::to_string( filename ),
            std::ofstream::binary };
        geode::TContext context{};
        geode::BitseryExtensions::register_serialize_pcontext(
            std::get< 0 >( context ) );
        geode::Serializer archive{ context, file };
        archive.object( problem );
        archive.adapter().flush();
        geode::OpenGeodeGeosciencesSimulationException::check_exception(
            std::get< 1 >( context ).isValid(), nullptr,
            geode::OpenGeodeException::TYPE::internal,
            "[Bitsery::write] Error while writing file: ", filename );
    }
} // namespace

namespace geode
{
    std::vector< std::string > OpenGeodePhysicalProblemOutput::write(
        const PhysicalProblem& problem ) const
    {
        try
        {
            write_physical_problem( problem, this->filename() );
        }
        catch( const OpenGeodeException& )
        {
            std::filesystem::remove( to_string( this->filename() ) );
            throw;
        }
        catch( const std::exception& exception )
        {
            // Bitsery throws a standard exception when a polymorphic object
            // has an unregistered type (see register_simulation_leaf)
            std::filesystem::remove( to_string( this->filename() ) );
            throw OpenGeodeGeosciencesSimulationException{ nullptr,
                OpenGeodeException::TYPE::data,
                "[PhysicalProblemOutput] Cannot write file ", this->filename(),
                ": ", exception.what(),
                ". Classes derived from FluidModel, FlowModel, "
                "RelativePermeability, CapillaryPressure or "
                "InitialCondition must be registered with "
                "register_simulation_leaf to be saved." };
        }
        return { to_string( this->filename() ) };
    }
} // namespace geode
