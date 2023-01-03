/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#pragma once

#include <async++.h>

#include <fstream>

#include <geode/basic/uuid.h>

#include <geode/model/representation/io/detail/geode_brep_output.h>

#include <geode/geosciences/representation/core/structural_model.h>
#include <geode/geosciences/representation/io/structural_model_output.h>

namespace geode
{
    class opengeode_geosciences_geosciences_api OpenGeodeStructuralModelOutput
        final : public StructuralModelOutput
    {
    public:
        OpenGeodeStructuralModelOutput( absl::string_view filename )
            : StructuralModelOutput( filename )
        {
        }

        static absl::string_view extension()
        {
            return StructuralModel::native_extension_static();
        }

        void save_structural_model_files(
            const StructuralModel& structural_model,
            absl::string_view directory ) const
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

        void archive_structural_model_files( const ZipFile& zip_writer ) const
        {
            OpenGeodeBRepOutput brep_output{ "" };
            brep_output.archive_brep_files( zip_writer );
        }

        void write( const StructuralModel& structural_model ) const final
        {
            const ZipFile zip_writer{ filename(), uuid{}.string() };
            save_structural_model_files(
                structural_model, to_string( zip_writer.directory() ) );
            archive_structural_model_files( zip_writer );
        }
    };
} // namespace geode
