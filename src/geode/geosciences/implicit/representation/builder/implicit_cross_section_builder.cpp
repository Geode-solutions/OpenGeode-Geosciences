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

#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.hpp>

#include <geode/basic/variable_attribute.hpp>

#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/detail/copy.hpp>

#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_cross_section.hpp>

namespace geode
{
    ImplicitCrossSectionBuilder::ImplicitCrossSectionBuilder(
        ImplicitCrossSection& implicit_section )
        : CrossSectionBuilder( implicit_section ),
          implicit_section_( implicit_section )
    {
    }

    ModelCopyMapping ImplicitCrossSectionBuilder::copy(
        const ImplicitCrossSection& other_model )
    {
        auto mapping = CrossSectionBuilder::copy( other_model );
        reinitialize_implicit_query_trees();
        copy_implicit_information( mapping, other_model );
        copy_implicit_attribute_values( mapping, other_model );
        return mapping;
    }

    void ImplicitCrossSectionBuilder::
        import_old_implicit_attribute_values_from_attribute_name(
            std::string_view old_attribute_name )
    {
        for( const auto& surface : implicit_section_.surfaces() )
        {
            auto& surface_vertex_attribute_manager =
                surface.mesh().vertex_attribute_manager();
            const auto old_attribute_id =
                surface_vertex_attribute_manager
                    .attribute_ids_matching_name( old_attribute_name )
                    .value()
                    .front();
            const auto old_attribute =
                surface_vertex_attribute_manager
                    .find_read_only_attribute< double >( old_attribute_id );
            auto new_attribute =
                surface_vertex_attribute_manager
                    .find_attribute< VariableAttribute, double >(
                        implicit_section_.implicit_attribute_id() );
            for( const auto vertex :
                geode::Range{ surface.mesh().nb_vertices() } )
            {
                new_attribute->set_value(
                    vertex, old_attribute->value( vertex ) );
            }
            surface_vertex_attribute_manager.delete_attribute(
                old_attribute_id );
        }
    }

    void ImplicitCrossSectionBuilder::
        import_old_implicit_attribute_values_from_attribute_id(
            const uuid& old_attribute_id )
    {
        for( const auto& surface : implicit_section_.surfaces() )
        {
            auto& surface_vertex_attribute_manager =
                surface.mesh().vertex_attribute_manager();
            const auto old_attribute =
                surface_vertex_attribute_manager
                    .find_read_only_attribute< double >( old_attribute_id );
            auto new_attribute =
                surface_vertex_attribute_manager
                    .find_attribute< VariableAttribute, double >(
                        implicit_section_.implicit_attribute_id() );
            for( const auto vertex :
                geode::Range{ surface.mesh().nb_vertices() } )
            {
                new_attribute->set_value(
                    vertex, old_attribute->value( vertex ) );
            }
            surface_vertex_attribute_manager.delete_attribute(
                old_attribute_id );
        }
    }

    void ImplicitCrossSectionBuilder::copy_implicit_information(
        ModelCopyMapping& mapping, const ImplicitCrossSection& other_model )
    {
        HorizonsStackBuilder2D{
            implicit_section_.modifiable_horizons_stack(
                ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} )
        }
            .copy( mapping, other_model.horizons_stack() );
        const auto& horizon_mapping =
            mapping.at( Horizon2D::component_type_static() );
        for( const auto& horizon : other_model.horizons() )
        {
            if( const auto isovalue =
                    other_model.horizon_implicit_value( horizon ) )
            {
                const auto& new_horizon = implicit_section_.horizon(
                    horizon_mapping.in2out( horizon.id() ) );
                implicit_section_.set_horizon_implicit_value( new_horizon,
                    isovalue.value(),
                    ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} );
            }
        }
    }

    void ImplicitCrossSectionBuilder::copy_implicit_attribute_values(
        ModelCopyMapping& mapping, const ImplicitCrossSection& other_model )
    {
        const auto& surface_mapping =
            mapping.at( Surface3D::component_type_static() );
        for( const auto& old_surface : other_model.surfaces() )
        {
            auto& old_surface_vertex_attribute_manager =
                old_surface.mesh().vertex_attribute_manager();
            const auto old_attribute =
                old_surface_vertex_attribute_manager
                    .find_read_only_attribute< double >(
                        other_model.implicit_attribute_id() );
            auto& new_surface = implicit_section_.surface(
                surface_mapping.in2out( old_surface.id() ) );
            auto& new_surface_vertex_attribute_manager =
                new_surface.mesh().vertex_attribute_manager();
            auto new_attribute =
                new_surface_vertex_attribute_manager
                    .find_attribute< VariableAttribute, double >(
                        implicit_section_.implicit_attribute_id() );
            for( const auto vertex :
                geode::Range{ new_surface.mesh().nb_vertices() } )
            {
                new_attribute->set_value(
                    vertex, old_attribute->value( vertex ) );
            }
        }
    }

    void ImplicitCrossSectionBuilder::reinitialize_implicit_query_trees()
    {
        implicit_section_.initialize_implicit_query_trees(
            ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} );
    }

    void ImplicitCrossSectionBuilder::
        instantiate_implicit_attribute_on_surfaces()
    {
        implicit_section_.instantiate_implicit_attribute_on_surfaces(
            ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} );
    }

    void ImplicitCrossSectionBuilder::set_implicit_value(
        const Surface2D& surface, index_t vertex_id, double value )
    {
        implicit_section_.set_implicit_value( surface, vertex_id, value,
            ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} );
    }

    void ImplicitCrossSectionBuilder::set_horizons_stack(
        HorizonsStack2D&& stack )
    {
        implicit_section_.set_horizons_stack( std::move( stack ),
            ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} );
    }

    void ImplicitCrossSectionBuilder::set_horizon_implicit_value(
        const Horizon2D& horizon, double isovalue )
    {
        implicit_section_.set_horizon_implicit_value( horizon, isovalue,
            ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} );
    }

    HorizonsStackBuilder2D ImplicitCrossSectionBuilder::horizons_stack_builder()
    {
        return HorizonsStackBuilder2D{
            implicit_section_.modifiable_horizons_stack(
                ImplicitCrossSection::ImplicitCrossSectionBuilderKey{} )
        };
    }
} // namespace geode
