/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/implicit/representation/core/detail/helpers.hpp>

#include <geode/basic/attribute_manager.hpp>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/sign.hpp>

#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/tetrahedral_solid_output.hpp>
#include <geode/mesh/io/triangulated_surface_output.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.hpp>
#include <geode/geosciences/implicit/representation/builder/horizons_stack_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_cross_section_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.hpp>
#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_cross_section.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_section.hpp>

namespace
{
    void check_number_of_horizons_and_stratigraphic_units(
        geode::index_t nb_horizons, geode::index_t nb_units )
    {
        OPENGEODE_EXCEPTION( nb_horizons <= nb_units + 1,
            "[repair_horizon_stack_if_possible] Too many horizons compared "
            "to stratigraphic units (",
            nb_horizons, ", should be less than ", nb_units, ")" );
        OPENGEODE_EXCEPTION( nb_units <= nb_horizons + 1,
            "[repair_horizon_stack_if_possible] Too many stratigraphic "
            "units compared to horizons (",
            nb_units, ", should be less than ", nb_horizons, ")" );
    }
} // namespace

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
            std::string_view prefix )
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
                            "geode_xyz", Point2D{ { 0, 0 } }, { false, true } );
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
            std::string_view prefix )
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
                            "geode_xyz", Point3D{ { 0, 0, 0 } },
                            { false, true } );
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
            CrossSection&& section, std::string_view scalar_attribute_name )
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
                            ImplicitCrossSection::IMPLICIT_ATTRIBUTE_NAME, 0,
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
                std::string_view scalar_attribute_name )
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
                            ImplicitStructuralModel::IMPLICIT_ATTRIBUTE_NAME, 0,
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
                                STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME,
                            Point2D{ { 0, 0 } }, { false, true } );
                for( const auto vertex_id : Range{ block_mesh.nb_vertices() } )
                {
                    const auto& vertex_point = block_mesh.point( vertex_id );
                    strati_location_attribute->set_value(
                        vertex_id, Point2D{ { vertex_point.value( first_axis ),
                                       vertex_point.value( second_axis ) } } );
                }
            }
            return { std::move( implicit_model ) };
        }

        template < index_t dimension >
        HorizonsStack< dimension > horizons_stack_from_name_list(
            absl::Span< const std::string > horizons_names,
            absl::Span< const std::string > units_names )
        {
            OPENGEODE_EXCEPTION( !horizons_names.empty(),
                "[horizons_stack_from_name_list] Cannot create HorizonsStack: "
                "horizons_names list is empty." );
            const auto nb_horizons = horizons_names.size();
            const auto nb_units = units_names.size();
            check_number_of_horizons_and_stratigraphic_units(
                nb_horizons, nb_units );
            HorizonsStack< dimension > stack;
            HorizonsStackBuilder< dimension > builder{ stack };
            auto current_horizon = builder.add_horizon();
            builder.set_horizon_name( current_horizon, horizons_names[0] );
            const auto& su_under = builder.add_stratigraphic_unit();
            builder.add_horizon_above( stack.horizon( current_horizon ),
                stack.stratigraphic_unit( su_under ) );
            bool lowest_unit_to_create{ nb_units < nb_horizons };
            if( !lowest_unit_to_create )
            {
                builder.set_stratigraphic_unit_name( su_under, units_names[0] );
            }
            for( const auto counter : Range{ 1, horizons_names.size() } )
            {
                const auto& su_above = builder.add_stratigraphic_unit();
                builder.set_stratigraphic_unit_name(
                    su_above, units_names[lowest_unit_to_create ? counter - 1
                                                                : counter] );
                builder.add_horizon_under( stack.horizon( current_horizon ),
                    stack.stratigraphic_unit( su_above ) );
                current_horizon = builder.add_horizon();
                builder.set_horizon_name(
                    current_horizon, horizons_names[counter] );
                builder.add_horizon_above( stack.horizon( current_horizon ),
                    stack.stratigraphic_unit( su_above ) );
            }
            const auto& su_above = builder.add_stratigraphic_unit();
            builder.add_horizon_under( stack.horizon( current_horizon ),
                stack.stratigraphic_unit( su_above ) );
            if( nb_units > nb_horizons )
            {
                builder.set_stratigraphic_unit_name(
                    su_above, units_names.back() );
            }
            return stack;
        }

        template < index_t dimension >
        void repair_horizon_stack_if_possible(
            HorizonsStack< dimension >& horizon_stack )
        {
            const auto nb_horizons = horizon_stack.nb_horizons();
            check_number_of_horizons_and_stratigraphic_units(
                nb_horizons, horizon_stack.nb_stratigraphic_units() );
            const auto bottom_horizon = horizon_stack.bottom_horizon();
            if( !horizon_stack.under( bottom_horizon ) )
            {
                HorizonsStackBuilder< dimension > builder{ horizon_stack };
                const auto& unit_under = builder.add_stratigraphic_unit();
                builder.add_horizon_above(
                    horizon_stack.horizon( bottom_horizon ),
                    horizon_stack.stratigraphic_unit( unit_under ) );
            }
            index_t horizon_counter{ 1 };
            auto su_above = horizon_stack.above( bottom_horizon );
            std::optional< uuid > current_horizon = bottom_horizon;
            while( su_above )
            {
                current_horizon = horizon_stack.above( su_above.value() );
                if( !current_horizon )
                {
                    break;
                }
                su_above = horizon_stack.above( current_horizon.value() );
                horizon_counter++;
            }
            OPENGEODE_EXCEPTION( horizon_counter == nb_horizons,
                "[repair_horizon_stack_if_possible] Missing or wrong "
                "above/under relations between horizons and stratigraphic "
                "units." );
            if( !su_above )
            {
                HorizonsStackBuilder< dimension > builder{ horizon_stack };
                const auto& unit_above = builder.add_stratigraphic_unit();
                builder.add_horizon_under(
                    horizon_stack.horizon( current_horizon.value() ),
                    horizon_stack.stratigraphic_unit( unit_above ) );
            }
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

        template HorizonsStack< 2 > opengeode_geosciences_implicit_api
            horizons_stack_from_name_list< 2 >(
                absl::Span< const std::string > horizons_names,
                absl::Span< const std::string > units_names );
        template HorizonsStack< 3 > opengeode_geosciences_implicit_api
            horizons_stack_from_name_list< 3 >(
                absl::Span< const std::string > horizons_names,
                absl::Span< const std::string > units_names );

        template void opengeode_geosciences_implicit_api
            repair_horizon_stack_if_possible< 2 >(
                HorizonsStack< 2 >& horizon_stack );
        template void opengeode_geosciences_implicit_api
            repair_horizon_stack_if_possible< 3 >(
                HorizonsStack< 3 >& horizon_stack );
    } // namespace detail
} // namespace geode