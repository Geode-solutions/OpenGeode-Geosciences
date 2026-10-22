/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_output.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/representation/builder/helpers/structural_model_fault_blocks_builder.hpp>
#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_input.hpp>
#include <geode/geosciences/explicit/representation/io/structural_model_output.hpp>

std::vector< geode::uuid > block_fault_blocks(
    const geode::StructuralModel& model, const geode::Block3D& block )
{
    std::vector< geode::uuid > result;
    for( const auto& collection : model.collections( block.id() ) )
    {
        if( collection.type == geode::FaultBlock3D::component_type_static() )
        {
            result.push_back( collection.id );
        }
    }
    return result;
}

bool is_fault_surface(
    const geode::StructuralModel& model, const geode::Surface3D& surface )
{
    for( const auto& collection : model.collections( surface.id() ) )
    {
        if( collection.type == geode::Fault3D::component_type_static() )
        {
            return true;
        }
    }
    return false;
}

void check_fault_blocks( const geode::StructuralModel& model,
    absl::Span< const geode::index_t > expected_sizes )
{
    for( const auto& block : model.blocks() )
    {
        const auto fault_blocks = block_fault_blocks( model, block );
        geode::OpenGeodeGeosciencesExplicitException::test(
            fault_blocks.size() == 1, "Block ", block.id().string(),
            " should be in exactly one FaultBlock" );
        for( const auto& surface : model.boundaries( block ) )
        {
            if( is_fault_surface( model, surface ) )
            {
                continue;
            }
            for( const auto& other : model.incidences( surface ) )
            {
                geode::OpenGeodeGeosciencesExplicitException::test(
                    block_fault_blocks( model, model.block( other.id() ) )
                        == fault_blocks,
                    "Blocks sharing a non-fault Surface should be in the "
                    "same FaultBlock" );
            }
        }
    }
    std::vector< geode::index_t > sizes;
    for( const auto& fault_block : model.fault_blocks() )
    {
        sizes.push_back( model.nb_items( fault_block.id() ) );
    }
    absl::c_sort( sizes );
    geode::OpenGeodeGeosciencesExplicitException::test(
        sizes
            == std::vector< geode::index_t >{ expected_sizes.begin(),
                expected_sizes.end() },
        "Wrong FaultBlock sizes" );
}

void test_structural_model_fault_blocks_builder()
{
    auto model_A2 = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "modelA2.og_strm" ) );
    geode::build_structural_model_fault_blocks( model_A2 );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model_A2.nb_fault_blocks() == 3,
        "Number of fault blocks in model should be 3" );
    check_fault_blocks( model_A2, { 4, 4, 4 } );
    geode::save_structural_model(
        model_A2, "modelA2_with_fault_blocks.og_strm" );
}

void test_structural_model_without_faults_fault_blocks_builder()
{
    auto model = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) );
    geode::build_structural_model_fault_blocks( model );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_fault_blocks() == 1,
        "Number of fault blocks in model without faults should be 1" );
    check_fault_blocks( model, { 2 } );
}

void test_fault_blocks_builder_idempotency()
{
    auto model = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "modelA2.og_strm" ) );
    geode::build_structural_model_fault_blocks( model );
    geode::build_structural_model_fault_blocks( model );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_fault_blocks() == 3,
        "Building fault blocks twice should not add fault blocks" );
    check_fault_blocks( model, { 4, 4, 4 } );

    geode::StructuralModelBuilder builder{ model };
    builder.remove_fault_block( *model.fault_blocks().begin() );
    geode::build_structural_model_fault_blocks( model );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model.nb_fault_blocks() == 3,
        "Rebuilding a removed fault block should give 3 fault blocks" );
    check_fault_blocks( model, { 4, 4, 4 } );

    auto model_without_faults = geode::load_structural_model(
        absl::StrCat( geode::DATA_PATH, "vri2.og_strm" ) );
    geode::build_structural_model_fault_blocks( model_without_faults );
    geode::build_structural_model_fault_blocks( model_without_faults );
    geode::OpenGeodeGeosciencesExplicitException::test(
        model_without_faults.nb_fault_blocks() == 1,
        "Building fault blocks twice in a model without faults should not add "
        "fault blocks" );
    check_fault_blocks( model_without_faults, { 2 } );
}

int main()
{
    try
    {
        geode::OpenGeodeGeosciencesExplicitLibrary::initialize();
        test_structural_model_fault_blocks_builder();
        test_structural_model_without_faults_fault_blocks_builder();
        test_fault_blocks_builder_idempotency();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
