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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>

#include <geode/tests_config.hpp>

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_output.hpp>

#include <geode/geosciences/explicit/representation/builder/helpers/structural_model_fault_blocks_builder.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_output.hpp>

void test_structural_model_fault_blocks_builder()
{
    auto model_A2 = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "modelA2.og_strm" ) );
    geode::build_structural_model_fault_blocks( model_A2 );
    OPENGEODE_EXCEPTION( model_A2.nb_fault_blocks() == 3,
        "[Test] Number of fault blocks in model should be 3" );
    geode::save_structural_model(
        model_A2, "modelA2_with_fault_blocks.og_strm" );
}

int main()
{
    try
    {
        geode::GeosciencesExplicitLibrary::initialize();
        test_structural_model_fault_blocks_builder();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
