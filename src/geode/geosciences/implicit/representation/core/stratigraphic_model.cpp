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

#include <geode/geosciences/implicit/representation/core/stratigraphic_model.h>

#include <async++.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/cached_value.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/tetrahedron.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/mensuration.h>
#include <geode/geometry/point.h>

#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_solid_helpers.h>
#include <geode/mesh/helpers/tetrahedral_solid_point_function.h>

#include <geode/model/helpers/component_mesh_polygons.h>
#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>

namespace geode
{
    class StratigraphicModel::Impl
    {
    public:
        void initialize_stratigraphic_query_trees(
            const StratigraphicModel& model )
        {
            instantiate_stratigraphic_location_on_blocks( model );
            block_stratigraphic_aabb_trees_.reserve( model.nb_blocks() );
            for( const auto& block : model.blocks() )
            {
                block_stratigraphic_aabb_trees_.try_emplace( block.id() );
            }
            block_stratigraphic_distance_to_tetras_.reserve(
                model.nb_blocks() );
            build_model_stratigraphic_distance_to_mesh_elements( model );
        }

        StratigraphicPoint3D stratigraphic_coordinates(
            const StratigraphicModel& model,
            const Block3D& block,
            index_t vertex_id ) const
        {
            return { stratigraphic_location_attributes_.at( block.id() )
                         .value( vertex_id ),
                model.implicit_value( block, vertex_id ) };
        }

        std::optional< StratigraphicPoint3D > stratigraphic_coordinates(
            const StratigraphicModel& model,
            const Block3D& block,
            const Point3D& geometric_point ) const
        {
            if( const auto containing_tetra =
                    model.containing_polyhedron( block, geometric_point ) )
            {
                return stratigraphic_coordinates(
                    model, block, geometric_point, containing_tetra.value() );
            }
            return std::nullopt;
        }

        StratigraphicPoint3D stratigraphic_coordinates(
            const StratigraphicModel& model,
            const Block3D& block,
            const Point3D& geometric_point,
            index_t tetrahedron_id ) const
        {
            return { stratigraphic_location_attributes_.at( block.id() )
                         .value( geometric_point, tetrahedron_id ),
                model.implicit_value(
                    block, geometric_point, tetrahedron_id ) };
        }

        std::optional< Point3D > geometric_coordinates(
            const StratigraphicModel& model,
            const Block3D& block,
            const StratigraphicPoint3D& stratigraphic_point ) const
        {
            if( const auto containing_tetra =
                    stratigraphic_containing_polyhedron(
                        model, block, stratigraphic_point ) )
            {
                return geometric_coordinates( model, block, stratigraphic_point,
                    containing_tetra.value() );
            }
            return std::nullopt;
        }

        Point3D geometric_coordinates( const StratigraphicModel& model,
            const Block3D& block,
            const StratigraphicPoint3D& stratigraphic_point,
            index_t tetrahedron_id ) const
        {
            PositiveStratigraphicTetrahedron pos_volume_strati_tetrahedron{
                model, block, tetrahedron_id
            };
            const auto barycentric_coords = tetrahedron_barycentric_coordinates(
                stratigraphic_point.stratigraphic_coordinates(),
                pos_volume_strati_tetrahedron.positive_tetra_ );
            Point3D geometric_point;
            for( const auto node_id : LIndices{ barycentric_coords } )
            {
                geometric_point +=
                    block.mesh().point(
                        pos_volume_strati_tetrahedron.indices_[node_id] )
                    * barycentric_coords[node_id];
            }
            return geometric_point;
        }

        std::optional< index_t > stratigraphic_containing_polyhedron(
            const StratigraphicModel& model,
            const Block3D& block,
            const StratigraphicPoint3D& stratigraphic_point ) const
        {
            const auto& block_stratigraphic_aabb =
                block_stratigraphic_aabb_trees_.at( block.id() )(
                    create_stratigraphic_aabb_tree, model, block );
            auto closest_tetrahedron =
                std::get< 0 >( block_stratigraphic_aabb.closest_element_box(
                    stratigraphic_point.stratigraphic_coordinates(),
                    block_stratigraphic_distance_to_tetras_.at(
                        block.id() ) ) );
            if( std::get< 0 >( block_stratigraphic_distance_to_tetras_.at(
                    block.id() )( stratigraphic_point, closest_tetrahedron ) )
                < GLOBAL_EPSILON )
            {
                return closest_tetrahedron;
            }
            return std::nullopt;
        }

        absl::InlinedVector< std::unique_ptr< TriangulatedSurface3D >, 2 >
            stratigraphic_surface( const StratigraphicModel& model,
                const Block3D& block,
                const Surface3D& surface ) const
        {
            if( model.is_boundary( surface, block ) )
            {
                absl::InlinedVector< std::unique_ptr< TriangulatedSurface3D >,
                    2 >
                    boundary_surface;
                boundary_surface.emplace_back(
                    stratigraphic_boundary_surface( model, block, surface ) );
                return boundary_surface;
            }
            else if( model.is_internal( surface, block ) )
            {
                return stratigraphic_internal_surface( model, block, surface );
            }
            OPENGEODE_ASSERT_NOT_REACHED(
                "[StratigraphicModel::stratigraphic_surface] The given "
                "surface is not boundary nor internal of the given block." );
            return {};
        }

        void instantiate_stratigraphic_location_on_blocks(
            const StratigraphicModel& model )
        {
            for( const auto& block : model.blocks() )
            {
                OPENGEODE_EXCEPTION(
                    ( block.mesh().type_name()
                        == TetrahedralSolid3D::type_name_static() ),
                    "[StratigraphicModel::instantiate_stratigraphic_"
                    "attribute_on_blocks] Blocks must be meshed as "
                    "TetrahedralSolids, which is not the case for block "
                    "with uuid '",
                    block.id().string(), "'." );
                if( !block.mesh().vertex_attribute_manager().attribute_exists(
                        stratigraphic_location_attribute_name ) )
                {
                    stratigraphic_location_attributes_.try_emplace( block.id(),
                        TetrahedralSolidPointFunction< 3, 2 >::create(
                            block.mesh< TetrahedralSolid3D >(),
                            stratigraphic_location_attribute_name,
                            Point2D{ { 0, 0 } } ) );
                }
                else
                {
                    stratigraphic_location_attributes_.try_emplace( block.id(),
                        TetrahedralSolidPointFunction< 3, 2 >::find(
                            block.mesh< TetrahedralSolid3D >(),
                            stratigraphic_location_attribute_name ) );
                }
            }
        }

        BoundingBox3D stratigraphic_bounding_box(
            const StratigraphicModel& model ) const
        {
            BoundingBox3D box;
            for( const auto& stratigraphic_tree :
                block_stratigraphic_aabb_trees_ )
            {
                box.add_box( stratigraphic_tree
                                 .second( create_stratigraphic_aabb_tree, model,
                                     model.block( stratigraphic_tree.first ) )
                                 .bounding_box() );
            }
            return box;
        }

        void set_stratigraphic_location(
            const Block3D& block, index_t vertex_id, Point2D value )
        {
            OPENGEODE_EXCEPTION(
                stratigraphic_location_attributes_.find( block.id() )
                    != stratigraphic_location_attributes_.end(),
                "[StratigraphicModel::set_stratigraphic_location] "
                "Couldn't find block uuid in the attributes registered - Try "
                "instantiating your attribute first." );
            stratigraphic_location_attributes_.at( block.id() )
                .set_value( vertex_id, std::move( value ) );
            reset_stratigraphic_aabb_tree( block );
        }

        void reset_stratigraphic_aabb_tree( const Block3D& block )
        {
            block_stratigraphic_aabb_trees_.at( block.id() ).reset();
        }

    private:
        struct PositiveStratigraphicTetrahedron
        {
            PositiveStratigraphicTetrahedron() = delete;
            PositiveStratigraphicTetrahedron( const StratigraphicModel& model,
                const Block3D& block,
                index_t tetrahedron_id )
                : indices_{ block.mesh().polyhedron_vertices(
                    tetrahedron_id ) },
                  positive_tetra_{
                      model.stratigraphic_coordinates( block, indices_[0] )
                          .stratigraphic_coordinates(),
                      model.stratigraphic_coordinates( block, indices_[1] )
                          .stratigraphic_coordinates(),
                      model.stratigraphic_coordinates( block, indices_[2] )
                          .stratigraphic_coordinates(),
                      model.stratigraphic_coordinates( block, indices_[3] )
                          .stratigraphic_coordinates()
                  }
            {
                if( tetrahedron_signed_volume( positive_tetra_ ) < 0 )
                {
                    std::swap( indices_[0], indices_[1] );
                    auto vertex0 = positive_tetra_.vertices()[0];
                    positive_tetra_.set_point(
                        0, positive_tetra_.vertices()[1] );
                    positive_tetra_.set_point( 1, std::move( vertex0 ) );
                }
            }

            PolyhedronVertices indices_;
            OwnerTetrahedron positive_tetra_;
        };

        class StratigraphicDistanceToTetrahedron
        {
        public:
            StratigraphicDistanceToTetrahedron(
                const StratigraphicModel& model, const Block3D& block )
                : model_( model ), block_( block )
            {
            }

            std::tuple< double, Point3D > operator()(
                const StratigraphicPoint3D& query, index_t cur_box ) const
            {
                auto positive_tetra =
                    PositiveStratigraphicTetrahedron{ model_, block_, cur_box };
                auto dist = point_tetrahedron_distance(
                    query.stratigraphic_coordinates(),
                    positive_tetra.positive_tetra_ );
                return dist;
            }

        private:
            const StratigraphicModel& model_;
            const Block3D& block_;
        };

        static AABBTree3D create_stratigraphic_aabb_tree(
            const StratigraphicModel& model, const Block3D& block )
        {
            const auto& block_mesh = block.mesh();
            absl::FixedArray< BoundingBox3D > box_vector(
                block_mesh.nb_polyhedra() );
            async::parallel_for(
                async::irange( index_t{ 0 }, block_mesh.nb_polyhedra() ),
                [&box_vector, &block, &block_mesh, &model]( index_t p ) {
                    BoundingBox3D bbox;
                    for( const auto v :
                        LRange{ block_mesh.nb_polyhedron_vertices( p ) } )
                    {
                        bbox.add_point(
                            model
                                .stratigraphic_coordinates( block,
                                    block_mesh.polyhedron_vertex( { p, v } ) )
                                .stratigraphic_coordinates() );
                    }
                    box_vector[p] = std::move( bbox );
                } );
            return AABBTree3D{ std::move( box_vector ) };
        }

        void build_model_stratigraphic_distance_to_mesh_elements(
            const StratigraphicModel& model )
        {
            for( const auto& block : model.blocks() )
            {
                block_stratigraphic_distance_to_tetras_.try_emplace(
                    block.id(), model, block );
            }
        }

        std::unique_ptr< TriangulatedSurface3D > stratigraphic_boundary_surface(
            const StratigraphicModel& model,
            const Block3D& block,
            const Surface3D& surface ) const
        {
            auto strati_surface =
                surface.mesh< TriangulatedSurface3D >().clone();
            auto strati_surface_builder =
                SurfaceMeshBuilder3D::create( *strati_surface );
            auto associated_polyhedron_facet_attribute =
                strati_surface->polygon_attribute_manager()
                    .find_or_create_attribute< VariableAttribute,
                        PolyhedronFacet >(
                        stratigraphic_surface_polyhedron_facet_attribute_name,
                        {} );
            const auto& surface_mesh = surface.mesh< TriangulatedSurface3D >();
            std::vector< bool > vertices_checked(
                surface_mesh.nb_vertices(), false );
            for( const auto polygon_id : Range{ surface_mesh.nb_polygons() } )
            {
                const auto polygon_block_vertices =
                    block_vertices_from_surface_polygon(
                        model, block, surface, polygon_id );
                OPENGEODE_EXCEPTION( polygon_block_vertices.size() == 1,
                    "[StratigraphicModel::stratigraphic_surface] Did not "
                    "find one polyhedron in the given block from a polygon of "
                    "the given surface." );
                for( const auto polygon_vertex_id :
                    LRange{ surface_mesh.nb_polygon_vertices( polygon_id ) } )
                {
                    const auto vertex_id = surface_mesh.polygon_vertex(
                        { polygon_id, polygon_vertex_id } );
                    if( vertices_checked[vertex_id] )
                    {
                        continue;
                    }
                    vertices_checked[vertex_id] = true;
                    strati_surface_builder->set_point(
                        vertex_id, stratigraphic_coordinates( model, block,
                                       polygon_block_vertices.front()
                                           .vertices[polygon_vertex_id] )
                                       .stratigraphic_coordinates() );
                }
                associated_polyhedron_facet_attribute->set_value(
                    polygon_id, polygon_block_vertices.front().facet );
            }
            return strati_surface;
        }

        absl::InlinedVector< std::unique_ptr< TriangulatedSurface3D >, 2 >
            stratigraphic_internal_surface( const StratigraphicModel& model,
                const Block3D& block,
                const Surface3D& surface ) const
        {
            absl::InlinedVector< std::unique_ptr< TriangulatedSurface3D >, 2 >
                strati_surfaces;
            std::array< std::unique_ptr< SurfaceMeshBuilder3D >, 2 >
                strati_surface_builders;
            std::array< std::shared_ptr< VariableAttribute< PolyhedronFacet > >,
                2 >
                associated_polyhedron_facet_attributes;
            const auto& surface_mesh = surface.mesh< TriangulatedSurface3D >();
            for( const auto i : LRange{ 2 } )
            {
                strati_surfaces.emplace_back( surface_mesh.clone() );
                strati_surface_builders[i] =
                    SurfaceMeshBuilder3D::create( *strati_surfaces[i] );
                associated_polyhedron_facet_attributes[i] =
                    strati_surfaces[i]
                        ->polygon_attribute_manager()
                        .find_or_create_attribute< VariableAttribute,
                            PolyhedronFacet >(
                            stratigraphic_surface_polyhedron_facet_attribute_name,
                            {} );
            }
            std::vector< bool > vertices_checked(
                surface_mesh.nb_vertices(), false );
            for( const auto polygon_id : Range{ surface_mesh.nb_polygons() } )
            {
                const auto polygon_block_vertices =
                    oriented_block_vertices_from_surface_polygon(
                        model, block, surface, polygon_id );
                OPENGEODE_EXCEPTION( polygon_block_vertices.nb_facets() == 2,
                    "[StratigraphicModel::stratigraphic_surface] Did not "
                    "find two polyhedra in the given block from a polygon of "
                    "the given surface." );
                for( const auto polygon_vertex_id :
                    LRange{ surface_mesh.nb_polygon_vertices( polygon_id ) } )
                {
                    const auto vertex_id = surface_mesh.polygon_vertex(
                        { polygon_id, polygon_vertex_id } );
                    if( vertices_checked[vertex_id] )
                    {
                        continue;
                    }
                    vertices_checked[vertex_id] = true;
                    strati_surface_builders[0]->set_point( vertex_id,
                        stratigraphic_coordinates( model, block,
                            polygon_block_vertices.oriented_polyhedron_facet
                                .value()
                                .vertices[polygon_vertex_id] )
                            .stratigraphic_coordinates() );
                    strati_surface_builders[1]->set_point( vertex_id,
                        stratigraphic_coordinates( model, block,
                            polygon_block_vertices.opposite_polyhedron_facet
                                .value()
                                .vertices[polygon_vertex_id] )
                            .stratigraphic_coordinates() );
                }
                associated_polyhedron_facet_attributes[0]->set_value(
                    polygon_id,
                    polygon_block_vertices.oriented_polyhedron_facet.value()
                        .facet );
                associated_polyhedron_facet_attributes[1]->set_value(
                    polygon_id,
                    polygon_block_vertices.opposite_polyhedron_facet.value()
                        .facet );
            }
            return strati_surfaces;
        }

    private:
        absl::flat_hash_map< uuid, TetrahedralSolidPointFunction< 3, 2 > >
            stratigraphic_location_attributes_;
        absl::flat_hash_map< uuid, CachedValue< AABBTree3D > >
            block_stratigraphic_aabb_trees_;
        absl::flat_hash_map< uuid, StratigraphicDistanceToTetrahedron >
            block_stratigraphic_distance_to_tetras_;
    };

    StratigraphicModel::StratigraphicModel()
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicModel::StratigraphicModel(
        StratigraphicModel&& other ) noexcept
        : ImplicitStructuralModel{ std::move( other ) }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicModel::StratigraphicModel(
        ImplicitStructuralModel&& implicit_model ) noexcept
        : ImplicitStructuralModel{ std::move( implicit_model ) }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicModel::StratigraphicModel(
        StructuralModel&& structural_model ) noexcept
        : ImplicitStructuralModel{ std::move( structural_model ) }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicModel::StratigraphicModel(
        const StratigraphicModel& initial_model,
        BRep&& brep,
        const ModelGenericMapping& initial_to_brep_mappings ) noexcept
        : ImplicitStructuralModel{ initial_model, std::move( brep ),
              initial_to_brep_mappings }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicModel::~StratigraphicModel() = default;

    StratigraphicModel StratigraphicModel::clone() const
    {
        return StratigraphicModel{ ImplicitStructuralModel::clone() };
    }

    StratigraphicPoint3D StratigraphicModel::stratigraphic_coordinates(
        const Block3D& block, index_t vertex_id ) const
    {
        return impl_->stratigraphic_coordinates( *this, block, vertex_id );
    }

    std::optional< StratigraphicPoint3D >
        StratigraphicModel::stratigraphic_coordinates(
            const Block3D& block, const Point3D& geometric_point ) const
    {
        return impl_->stratigraphic_coordinates(
            *this, block, geometric_point );
    }

    StratigraphicPoint3D StratigraphicModel::stratigraphic_coordinates(
        const Block3D& block,
        const Point3D& geometric_point,
        index_t polyhedron_id ) const
    {
        return impl_->stratigraphic_coordinates(
            *this, block, geometric_point, polyhedron_id );
    }

    std::optional< Point3D > StratigraphicModel::geometric_coordinates(
        const Block3D& block,
        const StratigraphicPoint3D& stratigraphic_point ) const
    {
        return impl_->geometric_coordinates(
            *this, block, stratigraphic_point );
    }

    Point3D StratigraphicModel::geometric_coordinates( const Block3D& block,
        const StratigraphicPoint3D& stratigraphic_point,
        index_t polyhedron_id ) const
    {
        return impl_->geometric_coordinates(
            *this, block, stratigraphic_point, polyhedron_id );
    }

    std::optional< index_t >
        StratigraphicModel::stratigraphic_containing_polyhedron(
            const Block3D& block,
            const StratigraphicPoint3D& stratigraphic_point ) const
    {
        return impl_->stratigraphic_containing_polyhedron(
            *this, block, stratigraphic_point );
    }

    absl::InlinedVector< std::unique_ptr< TriangulatedSurface3D >, 2 >
        StratigraphicModel::stratigraphic_surface(
            const Block3D& block, const Surface3D& surface ) const
    {
        return impl_->stratigraphic_surface( *this, block, surface );
    }

    BoundingBox3D StratigraphicModel::stratigraphic_bounding_box() const
    {
        return impl_->stratigraphic_bounding_box( *this );
    }

    void StratigraphicModel::initialize_stratigraphic_query_trees(
        StratigraphicModelBuilderKey )
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    void StratigraphicModel::instantiate_stratigraphic_location_on_blocks(
        StratigraphicModelBuilderKey )
    {
        impl_->instantiate_stratigraphic_location_on_blocks( *this );
    }

    void StratigraphicModel::do_set_implicit_value(
        const Block3D& block, index_t vertex_id, implicit_attribute_type value )
    {
        ImplicitStructuralModel::do_set_implicit_value(
            block, vertex_id, value );
        impl_->reset_stratigraphic_aabb_tree( block );
    }

    void StratigraphicModel::set_stratigraphic_location( const Block3D& block,
        index_t vertex_id,
        Point2D value,
        StratigraphicModelBuilderKey )
    {
        impl_->set_stratigraphic_location( block, vertex_id, value );
    }
} // namespace geode