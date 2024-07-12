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

#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_structural_model_input.h>

#include <async++.h>

#include <ghc/filesystem.hpp>

#include <geode/basic/uuid.h>
#include <geode/basic/zip_file.h>

#include <geode/model/representation/io/geode/geode_brep_input.h>

#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_input.h>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.h>
#include <geode/geosciences/implicit/representation/core/horizons_stack.h>
#include <geode/geosciences/implicit/representation/core/implicit_structural_model.h>
#include <geode/geosciences/implicit/representation/io/horizons_stack_input.h>

namespace geode
{
    void OpenGeodeImplicitStructuralModelInput::
        load_implicit_structural_model_files(
            ImplicitStructuralModel& model, std::string_view directory )
    {
        ImplicitStructuralModelBuilder builder{ model };
        builder.set_horizons_stack( std::move( load_horizons_stack< 3 >(
            absl::StrCat( directory, "/horizons_stack.",
                HorizonsStack3D::native_extension_static() ) ) ) );
        builder.reinitialize_implicit_query_trees();
    }

    ImplicitStructuralModel OpenGeodeImplicitStructuralModelInput::read()
    {
        const UnzipFile zip_reader{ this->filename(), uuid{}.string() };
        zip_reader.extract_all();
        ImplicitStructuralModel model;
        const auto impl_filename = absl::StrCat(
            zip_reader.directory(), "/implicit_model_impl.og_istrm" );
        OPENGEODE_EXCEPTION(
            ghc::filesystem::exists( to_string( impl_filename ) ),
            "[OpenGeodeImplicitStructuralModelInput::read] Error in reading "
            "files: Could not find stored impl." );
        std::ifstream file{ impl_filename, std::ifstream::binary };
        TContext context{};
        BitseryExtensions::register_deserialize_pcontext(
            std::get< 0 >( context ) );
        Deserializer archive{ context, file };
        archive.object( model );
        const auto& adapter = archive.adapter();
        OPENGEODE_EXCEPTION( adapter.error() == bitsery::ReaderError::NoError
                                 && adapter.isCompletedSuccessfully()
                                 && std::get< 1 >( context ).isValid(),
            "[OpenGeodeImplicitStructuralModelOutput::load_model_impl] "
            "Error while reading file: ",
            impl_filename );
        OpenGeodeBRepInput brep_input{ filename() };
        brep_input.load_brep_files( model, zip_reader.directory() );
        OpenGeodeStructuralModelInput structural_model_input{ filename() };
        structural_model_input.load_structural_model_files(
            model, zip_reader.directory() );
        load_implicit_structural_model_files( model, zip_reader.directory() );
        return model;
    }
} // namespace geode
