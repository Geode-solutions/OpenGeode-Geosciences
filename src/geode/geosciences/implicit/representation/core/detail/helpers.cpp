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

#include <geode/geosciences/implicit/representation/core/detail/helpers.h>

#include <geode/basic/attribute_manager.h>

#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/bounding_box.h>
#include <geode/geometry/information.h>
#include <geode/geometry/point.h>
#include <geode/geometry/sign.h>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/tetrahedral_solid_output.h>
#include <geode/mesh/io/triangulated_surface_output.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>
#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.h>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.h>
#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.h>
#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>
#include <geode/geosciences/implicit/representation/core/implicit_structural_model.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

namespace geode
{
    namespace detail
    {
        void rescale_implicit_value(
            ImplicitCrossSection& section, double scaling_factor )
        {
            ImplicitCrossSectionBuilder builder{ section };
            for( const auto& surface : section.surfaces() )
            {
                for( const auto vertex_id :
                    Range{ surface.mesh().nb_vertices() } )
                {
                    builder.set_implicit_value( surface, vertex_id,
                        scaling_factor
                            * section.implicit_value( surface, vertex_id ) );
                }
            }
        }

        void rescale_implicit_value_to_bbox_scale(
            StratigraphicSection& section )
        {
            const auto section_bbox = section.bounding_box();
            const auto section_lengths =
                section_bbox.max() - section_bbox.min();
            const auto strati_bbox = section.stratigraphic_bounding_box();
            const auto strati_lengths = strati_bbox.max() - strati_bbox.min();
            double scaling_factor =
                strati_lengths.value( 0 ) * section_lengths.value( 1 )
                / ( section_lengths.value( 0 ) * strati_lengths.value( 1 ) );
            rescale_implicit_value( section, scaling_factor );
        }

        void rescale_implicit_value(
            ImplicitStructuralModel& model, double scaling_factor )
        {
            ImplicitStructuralModelBuilder builder{ model };
            for( const auto& block : model.blocks() )
            {
                for( const auto vertex_id :
                    Range{ block.mesh().nb_vertices() } )
                {
                    builder.set_implicit_value( block, vertex_id,
                        scaling_factor
                            * model.implicit_value( block, vertex_id ) );
                }
            }
        }

        void rescale_implicit_value_to_bbox_scale( StratigraphicModel& model )
        {
            const auto model_bbox = model.bounding_box();
            const auto model_lengths = model_bbox.diagonal();
            const auto strati_bbox = model.stratigraphic_bounding_box();
            const auto strati_lengths = strati_bbox.diagonal();
            auto scaling_factor =
                strati_lengths.value( 0 ) * model_lengths.value( 2 )
                / ( model_lengths.value( 0 ) * strati_lengths.value( 2 ) );
            rescale_implicit_value( model, scaling_factor );
        }

        void save_stratigraphic_surfaces(
            const StratigraphicSection& implicit_model,
            absl::string_view prefix )
        {
            index_t counter{ 0 };
            for( const auto& surface : implicit_model.surfaces() )
            {
                auto strati_surface =
                    surface.mesh< TriangulatedSurface2D >().clone();
                auto surface_builder =
                    TriangulatedSurfaceBuilder2D::create( *strati_surface );
                auto xyz_attribute =
                    strati_surface->vertex_attribute_manager()
                        .find_or_create_attribute< VariableAttribute, Point2D >(
                            "geode_xyz", { { 0, 0 } }, { false, true } );
                for( const auto pt_id : Range{ strati_surface->nb_vertices() } )
                {
                    xyz_attribute->set_value(
                        pt_id, strati_surface->point( pt_id ) );
                    const auto strati_coords =
                        implicit_model.stratigraphic_coordinates(
                            surface, pt_id );
                    surface_builder->set_point(
                        pt_id, strati_coords.stratigraphic_coordinates() );
                }
                save_triangulated_surface( *strati_surface,
                    absl::StrCat( prefix, counter, ".og_tsf2d" ) );
                counter++;
            }
        }

        void save_stratigraphic_blocks(
            const geode::StratigraphicModel& implicit_model,
            absl::string_view prefix )
        {
            index_t counter{ 0 };
            for( const auto& block : implicit_model.blocks() )
            {
                auto strati_solid = block.mesh< TetrahedralSolid3D >().clone();
                auto surface_builder =
                    TetrahedralSolidBuilder3D::create( *strati_solid );
                auto xyz_attribute =
                    strati_solid->vertex_attribute_manager()
                        .find_or_create_attribute< VariableAttribute, Point3D >(
                            "geode_xyz", { { 0, 0, 0 } }, { false, true } );
                for( const auto pt_id : Range{ strati_solid->nb_vertices() } )
                {
                    xyz_attribute->set_value(
                        pt_id, strati_solid->point( pt_id ) );
                    const auto strati_coords =
                        implicit_model.stratigraphic_coordinates(
                            block, pt_id );
                    surface_builder->set_point(
                        pt_id, strati_coords.stratigraphic_coordinates() );
                }
                save_tetrahedral_solid( *strati_solid,
                    absl::StrCat( prefix, counter, ".og_tso3d" ) );
                counter++;
            }
        }

        ImplicitCrossSection implicit_section_from_cross_section_scalar_field(
            CrossSection&& section, absl::string_view scalar_attribute_name )
        {
            for( const auto& surface : section.surfaces() )
            {
                const auto& surface_mesh = surface.mesh();
                auto scalar_attribute =
                    surface_mesh.vertex_attribute_manager()
                        .find_attribute<
                            ImplicitCrossSection::implicit_attribute_type >(
                            scalar_attribute_name );
                auto implicit_attribute =
                    surface_mesh.vertex_attribute_manager()
                        .find_or_create_attribute< VariableAttribute,
                            ImplicitCrossSection::implicit_attribute_type >(
                            ImplicitCrossSection::implicit_attribute_name, 0,
                            { false, true } );
                for( const auto vertex_id :
                    Range{ surface_mesh.nb_vertices() } )
                {
                    implicit_attribute->set_value(
                        vertex_id, scalar_attribute->value( vertex_id ) );
                }
            }
            return { std::move( section ) };
        }

        ImplicitStructuralModel
            implicit_model_from_structural_model_scalar_field(
                StructuralModel&& model,
                absl::string_view scalar_attribute_name )
        {
            for( const auto& block : model.blocks() )
            {
                const auto& block_mesh = block.mesh();
                auto scalar_attribute =
                    block_mesh.vertex_attribute_manager()
                        .find_attribute<
                            ImplicitStructuralModel::implicit_attribute_type >(
                            scalar_attribute_name );
                auto implicit_attribute =
                    block_mesh.vertex_attribute_manager()
                        .find_or_create_attribute< VariableAttribute,
                            ImplicitStructuralModel::implicit_attribute_type >(
                            ImplicitStructuralModel::implicit_attribute_name, 0,
                            { false, true } );
                for( const auto vertex_id : Range{ block_mesh.nb_vertices() } )
                {
                    implicit_attribute->set_value(
                        vertex_id, scalar_attribute->value( vertex_id ) );
                }
            }
            return { std::move( model ) };
        }

        StratigraphicModel stratigraphic_model_from_implicit_model_and_coords(
            ImplicitStructuralModel&& implicit_model,
            local_index_t implicit_axis )
        {
            OPENGEODE_EXCEPTION( implicit_axis < 3,
                "[create_stratigraphic_model_from_brep_attribute_and_coords] "
                "Give a valid axis (0, 1, or 2)." );

            const local_index_t first_axis = implicit_axis == 0 ? 1 : 0;
            const local_index_t second_axis = implicit_axis == 1 ? 2 : 1;
            for( const auto& block : implicit_model.blocks() )
            {
                const auto& block_mesh = block.mesh();
                auto strati_location_attribute =
                    block_mesh.vertex_attribute_manager()
                        .find_or_create_attribute< VariableAttribute,
                            StratigraphicModel::stratigraphic_location_type >(
                            StratigraphicModel::
                                stratigraphic_location_attribute_name,
                            { { 0, 0 } }, { false, true } );
                for( const auto vertex_id : Range{ block_mesh.nb_vertices() } )
                {
                    const auto& vertex_point = block_mesh.point( vertex_id );
                    strati_location_attribute->set_value(
                        vertex_id, { { vertex_point.value( first_axis ),
                                       vertex_point.value( second_axis ) } } );
                }
            }
            return { std::move( implicit_model ) };
        }

        std::vector< MeshElement > invalid_stratigraphic_tetrahedra(
            const StratigraphicModel& implicit_model )
        {
            std::vector< MeshElement > invalid_tetrahedra;
            for( const auto& block : implicit_model.blocks() )
            {
                const auto& block_mesh = block.mesh< TetrahedralSolid3D >();
                for( const auto tetra_id : Range{ block_mesh.nb_polyhedra() } )
                {
                    const auto tetra_vertices =
                        block_mesh.polyhedron_vertices( tetra_id );
                    const OwnerTetrahedron strati_tetra{
                        implicit_model
                            .stratigraphic_coordinates(
                                block, tetra_vertices[0] )
                            .stratigraphic_coordinates(),
                        implicit_model
                            .stratigraphic_coordinates(
                                block, tetra_vertices[1] )
                            .stratigraphic_coordinates(),
                        implicit_model
                            .stratigraphic_coordinates(
                                block, tetra_vertices[2] )
                            .stratigraphic_coordinates(),
                        implicit_model
                            .stratigraphic_coordinates(
                                block, tetra_vertices[3] )
                            .stratigraphic_coordinates()
                    };
                    if( tetrahedron_volume_sign( strati_tetra )
                        == Sign::negative )
                    {
                        invalid_tetrahedra.emplace_back( block.id(), tetra_id );
                        Logger::info(
                            "[invalid_stratigraphic_tetrahedra] Tetrahedra ",
                            tetra_id, " of block ", block.id().string(),
                            " is invalid (negative stratigraphic volume)." );
                    }
                }
            }
            return invalid_tetrahedra;
        }
    } // namespace detail
} // namespace geode