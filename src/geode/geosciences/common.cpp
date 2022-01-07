/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/common.h>
#include <geode/basic/logger.h>

#include <geode/geosciences/representation/io/detail/geode_cross_section_input.h>
#include <geode/geosciences/representation/io/detail/geode_cross_section_output.h>
#include <geode/geosciences/representation/io/detail/geode_structural_model_input.h>
#include <geode/geosciences/representation/io/detail/geode_structural_model_output.h>

namespace
{
    void register_cross_section_input()
    {
        DEBUG( geode::OpenGeodeCrossSectionInput::extension().data() );
        geode::CrossSectionInputFactory::register_creator<
            geode::OpenGeodeCrossSectionInput >(
            geode::OpenGeodeCrossSectionInput::extension().data() );
    }

    void register_cross_section_output()
    {
        geode::CrossSectionOutputFactory::register_creator<
            geode::OpenGeodeCrossSectionOutput >(
            geode::OpenGeodeCrossSectionOutput::extension().data() );
    }

    void register_structural_model_input()
    {
        geode::StructuralModelInputFactory::register_creator<
            geode::OpenGeodeStructuralModelInput >(
            geode::OpenGeodeStructuralModelInput::extension().data() );
    }

    void register_structural_model_output()
    {
        geode::StructuralModelOutputFactory::register_creator<
            geode::OpenGeodeStructuralModelOutput >(
            geode::OpenGeodeStructuralModelOutput::extension().data() );
    }

    OPENGEODE_LIBRARY_INITIALIZE( geosciences )
    {
        register_cross_section_input();
        register_cross_section_output();
        register_structural_model_input();
        register_structural_model_output();
    }
} // namespace
