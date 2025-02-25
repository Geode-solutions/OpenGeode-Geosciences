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

#include <geode/geosciences/explicit/representation/core/detail/helpers.hpp>

#include <geode/geosciences/explicit/mixin/core/fault.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>
#include <geode/geosciences/explicit/mixin/core/horizon.hpp>
#include <geode/geosciences/explicit/mixin/core/stratigraphic_unit.hpp>
#include <geode/geosciences/explicit/representation/builder/cross_section_builder.hpp>
#include <geode/geosciences/explicit/representation/builder/structural_model_builder.hpp>
#include <geode/geosciences/explicit/representation/core/cross_section.hpp>
#include <geode/geosciences/explicit/representation/core/structural_model.hpp>

namespace
{
    void copy_horizon_item_relations( const geode::CrossSection& initial_model,
        const geode::CrossSection& modified_model,
        geode::CrossSectionBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >& horizon_mapping,
        const geode::GenericMapping< geode::uuid >& lines_mapping )
    {
        for( const auto& initial_horizon : initial_model.horizons() )
        {
            const auto& horizon = modified_model.horizon(
                horizon_mapping.in2out( initial_horizon.id() ) );
            for( const auto& initial_line_in_horizon :
                initial_model.horizon_items( initial_horizon ) )
            {
                for( const auto& line_id :
                    lines_mapping.in2out( initial_line_in_horizon.id() ) )
                {
                    builder.add_line_in_horizon(
                        modified_model.line( line_id ), horizon );
                }
            }
        }
    }
    void copy_horizon_item_relations(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& modified_model,
        geode::StructuralModelBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >& horizon_mapping,
        const geode::GenericMapping< geode::uuid >& surfaces_mapping )
    {
        for( const auto& initial_horizon : initial_model.horizons() )
        {
            const auto& horizon = modified_model.horizon(
                horizon_mapping.in2out( initial_horizon.id() ) );
            for( const auto& initial_surface_in_horizon :
                initial_model.horizon_items( initial_horizon ) )
            {
                for( const auto& surface_id :
                    surfaces_mapping.in2out( initial_surface_in_horizon.id() ) )
                {
                    builder.add_surface_in_horizon(
                        modified_model.surface( surface_id ), horizon );
                }
            }
        }
    }

    void copy_fault_item_relations( const geode::CrossSection& initial_model,
        const geode::CrossSection& modified_model,
        geode::CrossSectionBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >& fault_mapping,
        const geode::GenericMapping< geode::uuid >& lines_mapping )
    {
        for( const auto& initial_fault : initial_model.faults() )
        {
            const auto& fault = modified_model.fault(
                fault_mapping.in2out( initial_fault.id() ) );
            for( const auto& initial_line_in_fault :
                initial_model.fault_items( initial_fault ) )
            {
                for( const auto& line_id :
                    lines_mapping.in2out( initial_line_in_fault.id() ) )
                {
                    builder.add_line_in_fault(
                        modified_model.line( line_id ), fault );
                }
            }
        }
    }

    void copy_fault_item_relations( const geode::StructuralModel& initial_model,
        const geode::StructuralModel& modified_model,
        geode::StructuralModelBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >& fault_mapping,
        const geode::GenericMapping< geode::uuid >& surfaces_mapping )
    {
        for( const auto& initial_fault : initial_model.faults() )
        {
            const auto& fault = modified_model.fault(
                fault_mapping.in2out( initial_fault.id() ) );
            for( const auto& initial_surface_in_fault :
                initial_model.fault_items( initial_fault ) )
            {
                for( const auto& surface_id :
                    surfaces_mapping.in2out( initial_surface_in_fault.id() ) )
                {
                    builder.add_surface_in_fault(
                        modified_model.surface( surface_id ), fault );
                }
            }
        }
    }

    void copy_stratigraphic_unit_item_relations(
        const geode::CrossSection& initial_model,
        const geode::CrossSection& modified_model,
        geode::CrossSectionBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >&
            stratigraphic_unit_mapping,
        const geode::GenericMapping< geode::uuid >& surfaces_mapping )
    {
        for( const auto& initial_stratigraphic_unit :
            initial_model.stratigraphic_units() )
        {
            const auto& stratigraphic_unit = modified_model.stratigraphic_unit(
                stratigraphic_unit_mapping.in2out(
                    initial_stratigraphic_unit.id() ) );
            for( const auto& initial_surface_in_stratigraphic_unit :
                initial_model.stratigraphic_unit_items(
                    initial_stratigraphic_unit ) )
            {
                for( const auto& surface_id : surfaces_mapping.in2out(
                         initial_surface_in_stratigraphic_unit.id() ) )
                {
                    builder.add_surface_in_stratigraphic_unit(
                        modified_model.surface( surface_id ),
                        stratigraphic_unit );
                }
            }
        }
    }

    void copy_stratigraphic_unit_item_relations(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& modified_model,
        geode::StructuralModelBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >&
            stratigraphic_unit_mapping,
        const geode::GenericMapping< geode::uuid >& blocks_mapping )
    {
        for( const auto& initial_stratigraphic_unit :
            initial_model.stratigraphic_units() )
        {
            const auto& stratigraphic_unit = modified_model.stratigraphic_unit(
                stratigraphic_unit_mapping.in2out(
                    initial_stratigraphic_unit.id() ) );
            for( const auto& initial_block_in_stratigraphic_unit :
                initial_model.stratigraphic_unit_items(
                    initial_stratigraphic_unit ) )
            {
                for( const auto& block_id : blocks_mapping.in2out(
                         initial_block_in_stratigraphic_unit.id() ) )
                {
                    builder.add_block_in_stratigraphic_unit(
                        modified_model.block( block_id ), stratigraphic_unit );
                }
            }
        }
    }

    void copy_fault_block_item_relations(
        const geode::CrossSection& initial_model,
        const geode::CrossSection& modified_model,
        geode::CrossSectionBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >& fault_block_mapping,
        const geode::GenericMapping< geode::uuid >& surfaces_mapping )
    {
        for( const auto& initial_fault_block : initial_model.fault_blocks() )
        {
            const auto& fault_block = modified_model.fault_block(
                fault_block_mapping.in2out( initial_fault_block.id() ) );
            for( const auto& initial_surface_in_fault_block :
                initial_model.fault_block_items( initial_fault_block ) )
            {
                for( const auto& surface_id : surfaces_mapping.in2out(
                         initial_surface_in_fault_block.id() ) )
                {
                    builder.add_surface_in_fault_block(
                        modified_model.surface( surface_id ), fault_block );
                }
            }
        }
    }

    void copy_fault_block_item_relations(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& modified_model,
        geode::StructuralModelBuilder& builder,
        const geode::BijectiveMapping< geode::uuid >& fault_block_mapping,
        const geode::GenericMapping< geode::uuid >& blocks_mapping )
    {
        for( const auto& initial_fault_block : initial_model.fault_blocks() )
        {
            const auto& fault_block = modified_model.fault_block(
                fault_block_mapping.in2out( initial_fault_block.id() ) );
            for( const auto& initial_block_in_fault_block :
                initial_model.fault_block_items( initial_fault_block ) )
            {
                for( const auto& block_id :
                    blocks_mapping.in2out( initial_block_in_fault_block.id() ) )
                {
                    builder.add_block_in_fault_block(
                        modified_model.block( block_id ), fault_block );
                }
            }
        }
    }

    template < typename Model >
    void copy_geological_relationships( const Model& initial_model,
        const Model& modified_model,
        typename Model::Builder& builder,
        const geode::ModelGenericMapping& component_mappings,
        const geode::ModelCopyMapping& geological_mappings );

    void copy_geological_relationships(
        const geode::CrossSection& initial_model,
        const geode::CrossSection& modified_model,
        geode::CrossSectionBuilder& builder,
        const geode::ModelGenericMapping& component_mappings,
        const geode::ModelCopyMapping& geological_mappings )
    {
        const auto& lines_mapping =
            component_mappings.at( geode::Line3D::component_type_static() );
        copy_horizon_item_relations( initial_model, modified_model, builder,
            geological_mappings.at( geode::Horizon3D::component_type_static() ),
            lines_mapping );
        copy_fault_item_relations( initial_model, modified_model, builder,
            geological_mappings.at( geode::Fault3D::component_type_static() ),
            lines_mapping );
        const auto& surfaces_mapping =
            component_mappings.at( geode::Surface3D::component_type_static() );
        copy_stratigraphic_unit_item_relations( initial_model, modified_model,
            builder,
            geological_mappings.at(
                geode::StratigraphicUnit3D::component_type_static() ),
            surfaces_mapping );
        copy_fault_block_item_relations( initial_model, modified_model, builder,
            geological_mappings.at(
                geode::FaultBlock3D::component_type_static() ),
            surfaces_mapping );
    }

    void copy_geological_relationships(
        const geode::StructuralModel& initial_model,
        const geode::StructuralModel& modified_model,
        geode::StructuralModelBuilder& builder,
        const geode::ModelGenericMapping& component_mappings,
        const geode::ModelCopyMapping& geological_mappings )
    {
        const auto& surfaces_mapping =
            component_mappings.at( geode::Surface3D::component_type_static() );
        copy_horizon_item_relations( initial_model, modified_model, builder,
            geological_mappings.at( geode::Horizon3D::component_type_static() ),
            surfaces_mapping );
        copy_fault_item_relations( initial_model, modified_model, builder,
            geological_mappings.at( geode::Fault3D::component_type_static() ),
            surfaces_mapping );
        const auto& blocks_mapping =
            component_mappings.at( geode::Block3D::component_type_static() );
        copy_stratigraphic_unit_item_relations( initial_model, modified_model,
            builder,
            geological_mappings.at(
                geode::StratigraphicUnit3D::component_type_static() ),
            blocks_mapping );
        copy_fault_block_item_relations( initial_model, modified_model, builder,
            geological_mappings.at(
                geode::FaultBlock3D::component_type_static() ),
            blocks_mapping );
    }

    void add_geological_component_mapping(
        const geode::ModelCopyMapping& geological_mappings,
        geode::ModelGenericMapping& component_mappings,
        const geode::ComponentType& type )
    {
        geode::GenericMapping< geode::uuid > component_mapping;
        for( const auto& in2out : geological_mappings.at( type ).in2out_map() )
        {
            component_mapping.map( in2out.first, in2out.second );
        }
        component_mappings.emplace( type, std::move( component_mapping ) );
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < typename Model >
        ModelCopyMapping transfer_geological_information(
            const Model& initial_model,
            const Model& modified_model,
            typename Model::Builder& model_builder,
            ModelGenericMapping& components_mappings )
        {
            ModelCopyMapping geological_mappings;
            detail::add_geology_clone_mapping< Model >(
                geological_mappings, initial_model );
            model_builder.copy_geological_components(
                geological_mappings, initial_model );
            copy_geological_relationships( initial_model, modified_model,
                model_builder, components_mappings, geological_mappings );
            return geological_mappings;
        }

        template < index_t dimension >
        void add_geological_components_mappings(
            const ModelCopyMapping& geological_mappings,
            ModelGenericMapping& component_mappings )
        {
            add_geological_component_mapping( geological_mappings,
                component_mappings,
                Horizon< dimension >::component_type_static() );
            add_geological_component_mapping( geological_mappings,
                component_mappings,
                Fault< dimension >::component_type_static() );
            add_geological_component_mapping( geological_mappings,
                component_mappings,
                FaultBlock< dimension >::component_type_static() );
            add_geological_component_mapping( geological_mappings,
                component_mappings,
                StratigraphicUnit< dimension >::component_type_static() );
        }

        template ModelCopyMapping opengeode_geosciences_explicit_api
            transfer_geological_information( const CrossSection&,
                const CrossSection&,
                CrossSectionBuilder&,
                ModelGenericMapping& );
        template ModelCopyMapping opengeode_geosciences_explicit_api
            transfer_geological_information( const StructuralModel&,
                const StructuralModel&,
                StructuralModelBuilder&,
                ModelGenericMapping& );

        template void add_geological_components_mappings< 2 >(
            const ModelCopyMapping&, ModelGenericMapping& );
        template void add_geological_components_mappings< 3 >(
            const ModelCopyMapping&, ModelGenericMapping& );
    } // namespace detail
} // namespace geode
