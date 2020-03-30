/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/geosciences/representation/io/structural_model_input.h>

#include <geode/geosciences/representation/core/structural_model.h>

namespace geode
{
    void load_structural_model(
        StructuralModel& structural_model, absl::string_view filename )
    {
        try
        {
            auto input = StructuralModelInputFactory::create(
                extension_from_filename( filename ).data(), structural_model,
                filename );
            input->read();
            Logger::info( "StructuralModel loaded from ", filename );
            Logger::info( "StructuralModel has: ", structural_model.nb_blocks(),
                " Blocks, ", structural_model.nb_surfaces(), " Surfaces, ",
                structural_model.nb_lines(), " Lines, ",
                structural_model.nb_corners(), " Corners, ",
                structural_model.nb_faults(), " Faults and ",
                structural_model.nb_horizons(), " Horizons" );
        }
        catch( const OpenGeodeException& e )
        {
            Logger::error( e.what() );
            throw OpenGeodeException{ "[load_structural_model] Cannot load "
                                      "StructuralModel from file: ",
                filename };
        }
    }

    StructuralModelInput::StructuralModelInput(
        StructuralModel& structural_model, absl::string_view filename )
        : Input{ filename }, structural_model_( structural_model )
    {
    }
} // namespace geode
