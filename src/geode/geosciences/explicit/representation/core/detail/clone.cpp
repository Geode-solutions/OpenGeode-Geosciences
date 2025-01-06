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

#include <geode/geosciences/explicit/representation/core/detail/clone.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>
#include <geode/geosciences/explicit/representation/core/cross_section.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>

namespace
{
    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_fault_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > fault_clone_mapping;
        for( const auto& fault : model.faults() )
        {
            fault_clone_mapping.map( fault.id(), fault.id() );
        }
        return fault_clone_mapping;
    }

    template < typename Model >
    geode::BijectiveMapping< geode::uuid > clone_fault_block_mapping(
        const Model& model )
    {
        geode::BijectiveMapping< geode::uuid > fault_block_clone_mapping;
        for( const auto& fault_block : model.fault_blocks() )
        {
            fault_block_clone_mapping.map( fault_block.id(), fault_block.id() );
        }
        return fault_block_clone_mapping;
    }

    template < typename Model >
    void add_stack_geology_clone_mapping(
        geode::ModelCopyMapping& clone_mapping, const Model& model )
    {
        clone_mapping.emplace(
            geode::Horizon< Model::dim >::component_type_static(),
            geode::detail::clone_horizon_mapping( model ) );
        clone_mapping.emplace(
            geode::StratigraphicUnit< Model::dim >::component_type_static(),
            geode::detail::clone_stratigraphic_unit_mapping( model ) );
    }

    template < typename Model >
    void add_faults_geology_clone_mapping(
        geode::ModelCopyMapping& clone_mapping, const Model& model )
    {
        clone_mapping.emplace(
            geode::Fault< Model::dim >::component_type_static(),
            clone_fault_mapping( model ) );
        clone_mapping.emplace(
            geode::FaultBlock< Model::dim >::component_type_static(),
            clone_fault_block_mapping( model ) );
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < typename Model >
        void add_geology_clone_mapping(
            ModelCopyMapping& clone_mapping, const Model& model )
        {
            add_stack_geology_clone_mapping( clone_mapping, model );
            add_faults_geology_clone_mapping( clone_mapping, model );
        }

        template void opengeode_geosciences_explicit_api
            add_geology_clone_mapping(
                ModelCopyMapping& mapping, const CrossSection& model );

        template void opengeode_geosciences_explicit_api
            add_geology_clone_mapping(
                ModelCopyMapping& mapping, const StructuralModel& model );
    } // namespace detail
} // namespace geode
