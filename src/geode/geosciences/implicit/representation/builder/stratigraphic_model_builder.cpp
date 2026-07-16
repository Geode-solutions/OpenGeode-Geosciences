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

#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.hpp>

#include <geode/geometry/point.hpp>

#include <geode/basic/variable_attribute.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>

namespace geode
{
    StratigraphicModelBuilder::StratigraphicModelBuilder(
        StratigraphicModel& stratigraphic_model )
        : ImplicitStructuralModelBuilder( stratigraphic_model ),
          stratigraphic_model_( stratigraphic_model )
    {
    }

    ModelCopyMapping StratigraphicModelBuilder::copy(
        const StratigraphicModel& implicit_model )
    {
        auto mappings = ImplicitStructuralModelBuilder::copy( implicit_model );
        stratigraphic_model_.initialize_stratigraphic_query_trees( {} );
        copy_stratigraphic_attribute_values( mappings, implicit_model );
        return mappings;
    }

    void StratigraphicModelBuilder::reinitialize_stratigraphic_query_trees()
    {
        this->reinitialize_implicit_query_trees();
        stratigraphic_model_.initialize_stratigraphic_query_trees( {} );
    }

    void StratigraphicModelBuilder::
        instantiate_stratigraphic_attribute_on_blocks()
    {
        this->instantiate_implicit_attribute_on_blocks();
        stratigraphic_model_.instantiate_stratigraphic_location_on_blocks( {} );
    }

    void StratigraphicModelBuilder::set_stratigraphic_location(
        const Block3D& block, index_t vertex_id, Point2D value )
    {
        stratigraphic_model_.set_stratigraphic_location(
            block, vertex_id, std::move( value ), {} );
    }

    void StratigraphicModelBuilder::set_stratigraphic_coordinates(
        const Block3D& block,
        index_t vertex_id,
        const StratigraphicPoint3D& value )
    {
        this->set_implicit_value( block, vertex_id, value.implicit_value() );
        stratigraphic_model_.set_stratigraphic_location(
            block, vertex_id, value.stratigraphic_location(), {} );
    }

    void StratigraphicModelBuilder::
        import_old_stratigraphic_attribute_values_from_attribute_name(
            std::string_view old_attribute_name )
    {
        for( const auto& block : stratigraphic_model_.blocks() )
        {
            auto& block_vertex_attribute_manager =
                block.mesh().vertex_attribute_manager();
            const auto old_attribute_id =
                block_vertex_attribute_manager
                    .attribute_ids_matching_name( old_attribute_name )
                    .value()
                    .front();
            const auto old_attribute =
                block_vertex_attribute_manager
                    .find_read_only_attribute< Point2D >( old_attribute_id );
            auto new_attribute = block_vertex_attribute_manager.find_attribute<
                VariableAttribute, Point2D >(
                stratigraphic_model_.stratigraphic_location_attribute_id() );
            for( const auto vertex :
                geode::Range{ block.mesh().nb_vertices() } )
            {
                new_attribute->set_value(
                    vertex, old_attribute->value( vertex ) );
            }
        }
    }

    void StratigraphicModelBuilder::copy_stratigraphic_attribute_values(
        ModelCopyMapping& mapping, const StratigraphicModel& other_model )
    {
        const auto& block_mapping =
            mapping.at( Block3D::component_type_static() );
        for( const auto& old_block : other_model.blocks() )
        {
            auto& old_block_vertex_attribute_manager =
                old_block.mesh().vertex_attribute_manager();
            const auto old_attribute =
                old_block_vertex_attribute_manager
                    .find_read_only_attribute< Point2D >(
                        other_model.stratigraphic_location_attribute_id() );
            auto& new_block = stratigraphic_model_.block(
                block_mapping.in2out( old_block.id() ) );
            auto& new_block_vertex_attribute_manager =
                new_block.mesh().vertex_attribute_manager();
            auto new_attribute =
                new_block_vertex_attribute_manager
                    .find_attribute< VariableAttribute, Point2D >(
                        stratigraphic_model_
                            .stratigraphic_location_attribute_id() );
            for( const auto vertex :
                geode::Range{ new_block.mesh().nb_vertices() } )
            {
                new_attribute->set_value(
                    vertex, old_attribute->value( vertex ) );
            }
        }
    }

} // namespace geode
