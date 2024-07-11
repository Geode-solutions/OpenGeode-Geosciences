/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_structural_model_output.h>

#include <filesystem>
#include <string>
#include <vector>

#include <async++.h>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/representation/io/geode/geode_brep_output.h>

#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_output.h>
#include <geode/geosciences/implicit/representation/core/horizons_stack.h>
#include <geode/geosciences/implicit/representation/io/horizons_stack_output.h>

namespace geode
{
    void OpenGeodeImplicitStructuralModelOutput::archive_implicit_model_files(
        const ZipFile& zip_writer ) const
    {
        for( const auto& file :
            std::filesystem::directory_iterator( zip_writer.directory() ) )
        {
            zip_writer.archive_file( file.path().string() );
        }
    }

    void OpenGeodeImplicitStructuralModelOutput::save_implicit_model_files(
        const ImplicitStructuralModel& implicit_model,
        std::string_view directory ) const
    {
        async::parallel_invoke(
            [&directory, &implicit_model] {
                OpenGeodeStructuralModelOutput structural_model_output{ "" };
                structural_model_output.save_structural_model_files(
                    implicit_model, directory );
            },
            [&directory, &implicit_model] {
                save_horizons_stack( implicit_model.horizons_stack(),
                    absl::StrCat( directory, "/horizons_stack.",
                        HorizonsStack3D::native_extension_static() ) );
            },
            [&directory, &implicit_model] {
                const auto filename =
                    absl::StrCat( directory, "/implicit_model_impl.og_istrm" );
                std::ofstream file{ filename, std::ofstream::binary };
                TContext context{};
                BitseryExtensions::register_serialize_pcontext(
                    std::get< 0 >( context ) );
                Serializer archive{ context, file };
                archive.object( implicit_model );
                archive.adapter().flush();
                OPENGEODE_EXCEPTION( std::get< 1 >( context ).isValid(),
                    "[OpenGeodeImplicitStructuralModelOutput::save_model_impl] "
                    "Error while writing file: ",
                    filename );
            } );
    }

    std::vector< std::string > OpenGeodeImplicitStructuralModelOutput::write(
        const ImplicitStructuralModel& implicit_model ) const
    {
        const ZipFile zip_writer{ this->filename(), uuid{}.string() };
        save_implicit_model_files( implicit_model, zip_writer.directory() );
        archive_implicit_model_files( zip_writer );
        return { to_string( this->filename() ) };
    }
} // namespace geode
