/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_output.hpp>

#include <string>
#include <vector>

#include <async++.h>

#include <fstream>

#include <geode/basic/uuid.hpp>
#include <geode/basic/zip_file.hpp>

#include <geode/model/representation/io/geode/geode_brep_output.hpp>

namespace geode
{
    OpenGeodeStructuralModelOutput::OpenGeodeStructuralModelOutput(
        std::string_view filename )
        : StructuralModelOutput( filename )
    {
    }

    void OpenGeodeStructuralModelOutput::save_structural_model_files(
        const StructuralModel& structural_model,
        std::string_view directory ) const
    {
        async::parallel_invoke(
            [&directory, &structural_model] {
                OpenGeodeBRepOutput brep_output{ "" };
                brep_output.save_brep_files( structural_model, directory );
            },
            [&directory, &structural_model] {
                structural_model.save_faults( directory );
            },
            [&directory, &structural_model] {
                structural_model.save_horizons( directory );
            },
            [&directory, &structural_model] {
                structural_model.save_fault_blocks( directory );
            },
            [&directory, &structural_model] {
                structural_model.save_stratigraphic_units( directory );
            } );
    }

    void OpenGeodeStructuralModelOutput::archive_structural_model_files(
        const ZipFile& zip_writer ) const
    {
        OpenGeodeBRepOutput brep_output{ "" };
        brep_output.archive_brep_files( zip_writer );
    }

    std::vector< std::string > OpenGeodeStructuralModelOutput::write(
        const StructuralModel& structural_model ) const
    {
        const ZipFile zip_writer{ filename(), uuid{}.string() };
        save_structural_model_files(
            structural_model, to_string( zip_writer.directory() ) );
        archive_structural_model_files( zip_writer );
        return { to_string( filename() ) };
    }
} // namespace geode
