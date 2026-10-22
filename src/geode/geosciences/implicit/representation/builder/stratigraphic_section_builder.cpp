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

#include <geode/geosciences/implicit/representation/builder/stratigraphic_section_builder.hpp>

#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.hpp>

namespace geode
{
    StratigraphicSectionBuilder::StratigraphicSectionBuilder(
        StratigraphicSection& stratigraphic_section )
        : ImplicitCrossSectionBuilder( stratigraphic_section ),
          stratigraphic_section_( stratigraphic_section )
    {
    }

    ModelCopyMapping StratigraphicSectionBuilder::copy(
        const StratigraphicSection& stratigraphic_section )
    {
        auto mappings =
            ImplicitCrossSectionBuilder::copy( stratigraphic_section );
        stratigraphic_section_.initialize_stratigraphic_query_trees(
            StratigraphicSection::StratigraphicSectionBuilderKey{} );
        copy_stratigraphic_attribute_values( mappings, stratigraphic_section );
        return mappings;
    }

    void StratigraphicSectionBuilder::reinitialize_stratigraphic_query_trees()
    {
        this->reinitialize_implicit_query_trees();
        stratigraphic_section_.initialize_stratigraphic_query_trees(
            StratigraphicSection::StratigraphicSectionBuilderKey{} );
    }

    void StratigraphicSectionBuilder::
        instantiate_stratigraphic_attribute_on_surfaces()
    {
        this->instantiate_implicit_attribute_on_surfaces();
        stratigraphic_section_.instantiate_stratigraphic_location_on_surfaces(
            StratigraphicSection::StratigraphicSectionBuilderKey{} );
    }

    void StratigraphicSectionBuilder::set_stratigraphic_location(
        const Surface2D& surface, index_t vertex_id, Point1D value )
    {
        stratigraphic_section_.set_stratigraphic_location( surface, vertex_id,
            std::move( value ),
            StratigraphicSection::StratigraphicSectionBuilderKey{} );
    }

    void StratigraphicSectionBuilder::set_stratigraphic_coordinates(
        const Surface2D& surface,
        index_t vertex_id,
        const StratigraphicPoint2D& value )
    {
        this->set_implicit_value( surface, vertex_id, value.implicit_value() );
        stratigraphic_section_.set_stratigraphic_location( surface, vertex_id,
            value.stratigraphic_location(),
            StratigraphicSection::StratigraphicSectionBuilderKey{} );
    }

    void StratigraphicSectionBuilder::
        import_old_stratigraphic_attribute_values_from_attribute_name(
            std::string_view old_attribute_name )
    {
        for( const auto& surface : stratigraphic_section_.surfaces() )
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
                    .find_read_only_attribute< Point1D >( old_attribute_id );
            auto new_attribute =
                surface_vertex_attribute_manager
                    .find_attribute< VariableAttribute, Point1D >(
                        stratigraphic_section_
                            .stratigraphic_location_attribute_id() );
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

    void StratigraphicSectionBuilder::
        import_old_stratigraphic_attribute_values_from_attribute_id(
            const uuid& old_attribute_id )
    {
        for( const auto& surface : stratigraphic_section_.surfaces() )
        {
            auto& surface_vertex_attribute_manager =
                surface.mesh().vertex_attribute_manager();
            const auto old_attribute =
                surface_vertex_attribute_manager
                    .find_read_only_attribute< Point1D >( old_attribute_id );
            auto new_attribute =
                surface_vertex_attribute_manager
                    .find_attribute< VariableAttribute, Point1D >(
                        stratigraphic_section_
                            .stratigraphic_location_attribute_id() );
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

    void StratigraphicSectionBuilder::copy_stratigraphic_attribute_values(
        ModelCopyMapping& mapping, const StratigraphicSection& other_section )
    {
        const auto& surface_mapping =
            mapping.at( Surface2D::component_type_static() );
        const auto& old_attribute_id =
            other_section.stratigraphic_location_attribute_id();
        const auto& new_attribute_id =
            stratigraphic_section_.stratigraphic_location_attribute_id();
        for( const auto& old_surface : other_section.surfaces() )
        {
            if( !surface_mapping.has_mapping_input( old_surface.id() ) )
            {
                continue;
            }
            const auto& old_surface_vertex_attribute_manager =
                old_surface.mesh().vertex_attribute_manager();
            if( !old_surface_vertex_attribute_manager.attribute_exists(
                    old_attribute_id ) )
            {
                continue;
            }
            const auto& new_surface = stratigraphic_section_.surface(
                surface_mapping.in2out( old_surface.id() ) );
            auto& new_surface_vertex_attribute_manager =
                new_surface.mesh().vertex_attribute_manager();
            if( !new_surface_vertex_attribute_manager.attribute_exists(
                    new_attribute_id ) )
            {
                continue;
            }
            const auto old_attribute =
                old_surface_vertex_attribute_manager
                    .find_read_only_attribute< Point1D >( old_attribute_id );
            auto new_attribute =
                new_surface_vertex_attribute_manager
                    .find_attribute< VariableAttribute, Point1D >(
                        new_attribute_id );
            for( const auto vertex :
                geode::Range{ new_surface.mesh().nb_vertices() } )
            {
                new_attribute->set_value(
                    vertex, old_attribute->value( vertex ) );
            }
            if( old_attribute_id != new_attribute_id
                && new_surface_vertex_attribute_manager.attribute_exists(
                    old_attribute_id ) )
            {
                new_surface_vertex_attribute_manager.delete_attribute(
                    old_attribute_id );
            }
        }
    }
} // namespace geode
