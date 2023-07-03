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

#include <geode/geosciences/explicit/common.h>

#include <geode/geosciences/implicit/common.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_horizons_stack_input.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_horizons_stack_output.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_cross_section_input.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_cross_section_output.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_structural_model_input.h>
#include <geode/geosciences/implicit/representation/io/geode/geode_implicit_structural_model_output.h>

namespace
{
    void register_horizons_stack_input()
    {
        geode::HorizonsStackInputFactory2D::register_creator<
            geode::OpenGeodeHorizonsStackInput2D >(
            geode::OpenGeodeHorizonsStackOutput2D::extension().data() );
        geode::HorizonsStackInputFactory3D::register_creator<
            geode::OpenGeodeHorizonsStackInput3D >(
            geode::OpenGeodeHorizonsStackOutput3D::extension().data() );
    }

    void register_horizons_stack_output()
    {
        geode::HorizonsStackOutputFactory2D::register_creator<
            geode::OpenGeodeHorizonsStackOutput2D >(
            geode::OpenGeodeHorizonsStackOutput2D::extension().data() );
        geode::HorizonsStackOutputFactory3D::register_creator<
            geode::OpenGeodeHorizonsStackOutput3D >(
            geode::OpenGeodeHorizonsStackOutput3D::extension().data() );
    }

    void register_cross_section_model_input()
    {
        geode::ImplicitCrossSectionInputFactory::register_creator<
            geode::OpenGeodeImplicitCrossSectionInput >(
            geode::OpenGeodeImplicitCrossSectionInput::extension().data() );
    }

    void register_cross_section_model_output()
    {
        geode::ImplicitCrossSectionOutputFactory::register_creator<
            geode::OpenGeodeImplicitCrossSectionOutput >(
            geode::OpenGeodeImplicitCrossSectionOutput::extension().data() );
    }

    void register_implicit_structural_model_input()
    {
        geode::ImplicitStructuralModelInputFactory::register_creator<
            geode::OpenGeodeImplicitStructuralModelInput >(
            geode::OpenGeodeImplicitStructuralModelInput::extension().data() );
    }

    void register_implicit_structural_model_output()
    {
        geode::ImplicitStructuralModelOutputFactory::register_creator<
            geode::OpenGeodeImplicitStructuralModelOutput >(
            geode::OpenGeodeImplicitStructuralModelOutput::extension().data() );
    }
} // namespace

namespace geode
{
    OPENGEODE_LIBRARY_IMPLEMENTATION( GeosciencesImplicit )
    {
        GeosciencesExplicitLibrary::initialize();
        register_horizons_stack_input();
        register_horizons_stack_output();
        register_cross_section_model_input();
        register_cross_section_model_output();
        register_implicit_structural_model_input();
        register_implicit_structural_model_output();
    }
} // namespace geode
