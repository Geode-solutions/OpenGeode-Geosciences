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

#include <geode/basic/common.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/common.hpp>

#include <geode/geosciences/explicit/common.hpp>
#include <geode/geosciences/explicit/mixin/core/bitsery_archive.hpp>
#include <geode/geosciences/explicit/representation/io/geode/geode_cross_section_input.hpp>
#include <geode/geosciences/explicit/representation/io/geode/geode_cross_section_output.hpp>
#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/geode/geode_structural_model_output.hpp>

namespace
{
    void register_cross_section_input()
    {
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
} // namespace

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( GeosciencesExplicit )
    {
        OpenGeodeModelLibrary::initialize();
        register_cross_section_input();
        register_cross_section_output();
        register_structural_model_input();
        register_structural_model_output();
        BitseryExtensions::register_functions(
            register_explicit_serialize_pcontext,
            register_explicit_deserialize_pcontext );
    }
} // namespace geode
