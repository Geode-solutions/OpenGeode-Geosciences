/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/factory.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/geosciences/common.h>

namespace geode
{
    class StructuralModel;
    class StructuralModelBuilder;
} // namespace geode

namespace geode
{
    /*!
     * API function for loading a StructuralModel.
     * The adequate loader is called depending on the filename extension.
     * @param[out] structural_model Loaded StructuralModel.
     * @param[in] filename Path to the file to load.
     */
    void opengeode_geosciences_geosciences_api load_structural_model(
        StructuralModel& structural_model, const std::string& filename );

    class opengeode_geosciences_geosciences_api StructuralModelInput
        : public Input
    {
    public:
        virtual ~StructuralModelInput() = default;

    protected:
        StructuralModelInput(
            StructuralModel& structural_model, std::string filename );

        StructuralModel& structural_model()
        {
            return structural_model_;
        }

    private:
        StructuralModel& structural_model_;
    };

    using StructuralModelInputFactory = Factory< std::string,
        StructuralModelInput,
        StructuralModel&,
        std::string >;
} // namespace geode
