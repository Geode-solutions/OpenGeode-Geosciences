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

#include <geode/geosciences/implicit/representation/core/stratigraphic_section.h>

#include <async++.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/cached_value.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/mensuration.h>
#include <geode/geometry/point.h>

#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>
#include <geode/mesh/helpers/triangulated_surface_point_function.h>

#include <geode/model/helpers/component_mesh_polygons.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.h>

namespace geode
{
    class StratigraphicSection::Impl
    {
    public:
        void initialize_stratigraphic_query_trees(
            const StratigraphicSection& model )
        {
            stratigraphic_location_attributes_.reserve( model.nb_surfaces() );
            instantiate_stratigraphic_location_on_surfaces( model );
            surface_stratigraphic_aabb_trees_.reserve( model.nb_surfaces() );
            for( const auto& surface : model.surfaces() )
            {
                surface_stratigraphic_aabb_trees_.try_emplace( surface.id() );
            }
            surface_stratigraphic_distance_to_triangles_.reserve(
                model.nb_surfaces() );
            build_model_stratigraphic_distance_to_mesh_elements( model );
        }

        StratigraphicPoint2D stratigraphic_coordinates(
            const StratigraphicSection& model,
            const Surface2D& surface,
            index_t vertex_id ) const
        {
            return { stratigraphic_location_attributes_.at( surface.id() )
                         .value( vertex_id ),
                model.implicit_value( surface, vertex_id ) };
        }

        std::optional< StratigraphicPoint2D > stratigraphic_coordinates(
            const StratigraphicSection& model,
            const Surface2D& surface,
            const Point2D& geometric_point ) const
        {
            if( const auto containing_polygon =
                    model.containing_polygon( surface, geometric_point ) )
            {
                return stratigraphic_coordinates( model, surface,
                    geometric_point, containing_polygon.value() );
            }
            return std::nullopt;
        }

        StratigraphicPoint2D stratigraphic_coordinates(
            const StratigraphicSection& model,
            const Surface2D& surface,
            const Point2D& geometric_point,
            index_t triangle_id ) const
        {
            return { stratigraphic_location_attributes_.at( surface.id() )
                         .value( geometric_point, triangle_id ),
                model.implicit_value( surface, geometric_point, triangle_id ) };
        }

        std::optional< Point2D > geometric_coordinates(
            const StratigraphicSection& model,
            const Surface2D& surface,
            const StratigraphicPoint2D& stratigraphic_point ) const
        {
            if( const auto containing_polygon =
                    stratigraphic_containing_polygon(
                        model, surface, stratigraphic_point ) )
            {
                return geometric_coordinates( model, surface,
                    stratigraphic_point, containing_polygon.value() );
            }
            return std::nullopt;
        }

        Point2D geometric_coordinates( const StratigraphicSection& model,
            const Surface2D& surface,
            const StratigraphicPoint2D& stratigraphic_point,
            index_t triangle_id ) const
        {
            PositiveStratigraphicTriangle pos_area_strati_triangle{ model,
                surface, triangle_id };
            const auto barycentric_coords =
                triangle_barycentric_coordinates< 2 >(
                    stratigraphic_point.stratigraphic_coordinates(),
                    pos_area_strati_triangle.positive_triangle_ );
            Point2D geometric_point;
            for( const auto node_id : LIndices{ barycentric_coords } )
            {
                geometric_point +=
                    surface.mesh().point(
                        pos_area_strati_triangle.indices_[node_id] )
                    * barycentric_coords[node_id];
            }
            return geometric_point;
        }

        std::optional< index_t > stratigraphic_containing_polygon(
            const StratigraphicSection& model,
            const Surface2D& surface,
            const StratigraphicPoint2D& stratigraphic_point ) const
        {
            const auto closest_triangle = std::get< 0 >(
                surface_stratigraphic_aabb_trees_
                    .at( surface.id() )(
                        create_stratigraphic_aabb_tree, model, surface )
                    .closest_element_box(
                        stratigraphic_point.stratigraphic_coordinates(),
                        surface_stratigraphic_distance_to_triangles_.at(
                            surface.id() ) ) );
            if( std::get< 0 >( surface_stratigraphic_distance_to_triangles_.at(
                    surface.id() )( stratigraphic_point, closest_triangle ) )
                < GLOBAL_EPSILON )
            {
                return closest_triangle;
            }
            return std::nullopt;
        }

        absl::InlinedVector< std::unique_ptr< EdgedCurve2D >, 2 >
            stratigraphic_line( const StratigraphicSection& model,
                const Surface2D& surface,
                const Line2D& line ) const
        {
            if( model.is_boundary( line, surface ) )
            {
                absl::InlinedVector< std::unique_ptr< EdgedCurve2D >, 2 >
                    boundary_line;
                boundary_line.emplace_back(
                    stratigraphic_boundary_line( model, surface, line ) );
                return boundary_line;
            }
            else if( model.is_internal( line, surface ) )
            {
                return stratigraphic_internal_line( model, surface, line );
            }
            OPENGEODE_ASSERT_NOT_REACHED(
                "[StratigraphicSection::stratigraphic_line] The given "
                "line is not boundary nor internal of the given surface." );
            return {};
        }

        BoundingBox2D stratigraphic_bounding_box(
            const StratigraphicSection& model ) const
        {
            BoundingBox2D box;
            for( const auto& stratigraphic_tree :
                surface_stratigraphic_aabb_trees_ )
            {
                box.add_box( stratigraphic_tree
                                 .second( create_stratigraphic_aabb_tree, model,
                                     model.surface( stratigraphic_tree.first ) )
                                 .bounding_box() );
            }
            return box;
        }

        void instantiate_stratigraphic_location_on_surfaces(
            const StratigraphicSection& model )
        {
            for( const auto& surface : model.surfaces() )
            {
                OPENGEODE_EXCEPTION(
                    ( surface.mesh().type_name()
                        == TriangulatedSurface2D::type_name_static() ),
                    "[StratigraphicSection::instantiate_stratigraphic_"
                    "attribute_on_surfaces] Surfaces must be meshed as "
                    "TriangulatedSurface2D, which is not the case for surface "
                    "with uuid '",
                    surface.id().string(), "'." );
                if( !surface.mesh().vertex_attribute_manager().attribute_exists(
                        STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME ) )
                {
                    stratigraphic_location_attributes_.try_emplace(
                        surface.id(),
                        TriangulatedSurfacePointFunction< 2, 1 >::create(
                            surface.mesh< TriangulatedSurface2D >(),
                            STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME,
                            Point1D{ { 0 } } ) );
                }
                else
                {
                    stratigraphic_location_attributes_.try_emplace(
                        surface.id(),
                        TriangulatedSurfacePointFunction< 2, 1 >::find(
                            surface.mesh< TriangulatedSurface2D >(),
                            STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME ) );
                }
            }
        }

        void set_stratigraphic_location(
            const Surface2D& surface, index_t vertex_id, Point1D value )
        {
            OPENGEODE_EXCEPTION(
                stratigraphic_location_attributes_.find( surface.id() )
                    != stratigraphic_location_attributes_.end(),
                "[StratigraphicSection::set_stratigraphic_location] "
                "Couldn't find surface uuid in the attributes registered - Try "
                "instantiating your attribute first." );
            stratigraphic_location_attributes_.at( surface.id() )
                .set_value( vertex_id, std::move( value ) );
            reset_stratigraphic_aabb_tree( surface );
        }

        void reset_stratigraphic_aabb_tree( const Surface2D& surface )
        {
            surface_stratigraphic_aabb_trees_.at( surface.id() ).reset();
        }

    private:
        struct PositiveStratigraphicTriangle
        {
            PositiveStratigraphicTriangle() = delete;
            PositiveStratigraphicTriangle( const StratigraphicSection& model,
                const Surface2D& surface,
                index_t triangle_id )
                : indices_{ surface.mesh().polygon_vertices( triangle_id ) },
                  positive_triangle_{
                      model.stratigraphic_coordinates( surface, indices_[0] )
                          .stratigraphic_coordinates(),
                      model.stratigraphic_coordinates( surface, indices_[1] )
                          .stratigraphic_coordinates(),
                      model.stratigraphic_coordinates( surface, indices_[2] )
                          .stratigraphic_coordinates()
                  }
            {
                if( triangle_signed_area( positive_triangle_ ) < 0 )
                {
                    std::swap( indices_[0], indices_[1] );
                    auto vertex0 = positive_triangle_.vertices()[0];
                    positive_triangle_.set_point(
                        0, positive_triangle_.vertices()[1] );
                    positive_triangle_.set_point( 1, std::move( vertex0 ) );
                }
            }

            PolygonVertices indices_;
            OwnerTriangle2D positive_triangle_;
        };

        class StratigraphicDistanceToTriangle
        {
        public:
            StratigraphicDistanceToTriangle(
                const StratigraphicSection& model, const Surface2D& surface )
                : model_( model ), surface_( surface )
            {
            }

            std::tuple< double, Point2D > operator()(
                const StratigraphicPoint2D& query, index_t cur_box ) const
            {
                return point_triangle_distance< 2 >(
                    query.stratigraphic_coordinates(),
                    PositiveStratigraphicTriangle{ model_, surface_, cur_box }
                        .positive_triangle_ );
            }

        private:
            const StratigraphicSection& model_;
            const Surface2D& surface_;
        };

        static AABBTree2D create_stratigraphic_aabb_tree(
            const StratigraphicSection& model, const Surface2D& surface )
        {
            const auto& surface_mesh = surface.mesh();
            absl::FixedArray< BoundingBox2D > box_vector(
                surface_mesh.nb_polygons() );
            async::parallel_for(
                async::irange( index_t{ 0 }, surface_mesh.nb_polygons() ),
                [&box_vector, &surface, &surface_mesh, &model]( index_t p ) {
                    BoundingBox2D bbox;
                    for( const auto v :
                        LRange{ surface_mesh.nb_polygon_vertices( p ) } )
                    {
                        bbox.add_point(
                            model
                                .stratigraphic_coordinates( surface,
                                    surface_mesh.polygon_vertex( { p, v } ) )
                                .stratigraphic_coordinates() );
                    }
                    box_vector[p] = std::move( bbox );
                } );
            return AABBTree2D{ std::move( box_vector ) };
        }

        void build_model_stratigraphic_distance_to_mesh_elements(
            const StratigraphicSection& model )
        {
            for( const auto& surface : model.surfaces() )
            {
                surface_stratigraphic_distance_to_triangles_.try_emplace(
                    surface.id(), model, surface );
            }
        }

        std::unique_ptr< EdgedCurve2D > stratigraphic_boundary_line(
            const StratigraphicSection& model,
            const Surface2D& surface,
            const Line2D& line ) const
        {
            auto strati_line = line.mesh().clone();
            auto strati_line_builder =
                EdgedCurveBuilder2D::create( *strati_line );
            auto associated_polygon_edge_attribute =
                strati_line->edge_attribute_manager()
                    .find_or_create_attribute< VariableAttribute, PolygonEdge >(
                        STRATIGRAPHIC_LINE_POLYGON_EDGE_ATTRIBUTE_NAME, {} );
            const auto& line_mesh = line.mesh();
            std::vector< bool > vertices_checked(
                line_mesh.nb_vertices(), false );
            for( const auto edge_id : Range{ line_mesh.nb_edges() } )
            {
                const auto edge_surface_vertices =
                    surface_vertices_from_line_edge(
                        model, surface, line, edge_id );
                OPENGEODE_EXCEPTION( edge_surface_vertices.size() == 1,
                    "[StratigraphicSection::stratigraphic_line] Did not "
                    "find one polygon in the given surface from an edge of "
                    "the given line." );
                for( const auto edge_vertex_id : LRange{ 2 } )
                {
                    const auto vertex_id =
                        line_mesh.edge_vertex( { edge_id, edge_vertex_id } );
                    if( vertices_checked[vertex_id] )
                    {
                        continue;
                    }
                    vertices_checked[vertex_id] = true;
                    strati_line_builder->set_point(
                        vertex_id, stratigraphic_coordinates( model, surface,
                                       edge_surface_vertices.front()
                                           .vertices[edge_vertex_id] )
                                       .stratigraphic_coordinates() );
                }
                associated_polygon_edge_attribute->set_value(
                    edge_id, edge_surface_vertices.front().edge );
            }
            return strati_line;
        }

        absl::InlinedVector< std::unique_ptr< EdgedCurve2D >, 2 >
            stratigraphic_internal_line( const StratigraphicSection& model,
                const Surface2D& surface,
                const Line2D& line ) const
        {
            absl::InlinedVector< std::unique_ptr< EdgedCurve2D >, 2 >
                strati_lines;
            std::array< std::unique_ptr< EdgedCurveBuilder2D >, 2 >
                strati_line_builders;
            std::array< std::shared_ptr< VariableAttribute< PolygonEdge > >, 2 >
                associated_polygon_edge_attributes;
            const auto& line_mesh = line.mesh();
            for( const auto i : LRange{ 2 } )
            {
                strati_lines.emplace_back( line_mesh.clone() );
                strati_line_builders[i] =
                    EdgedCurveBuilder2D::create( *strati_lines[i] );
                associated_polygon_edge_attributes[i] =
                    strati_lines[i]
                        ->edge_attribute_manager()
                        .find_or_create_attribute< VariableAttribute,
                            PolygonEdge >(
                            STRATIGRAPHIC_LINE_POLYGON_EDGE_ATTRIBUTE_NAME,
                            {} );
            }
            std::vector< bool > vertices_checked(
                line_mesh.nb_vertices(), false );
            for( const auto edge_id : Range{ line_mesh.nb_edges() } )
            {
                const auto edge_surface_vertices =
                    oriented_surface_vertices_from_line_edge(
                        model, surface, line, edge_id );
                OPENGEODE_EXCEPTION( edge_surface_vertices.nb_edges() == 2,
                    "[StratigraphicSection::stratigraphic_line] Did not "
                    "find two polygons in the given surface from an edge of "
                    "the given line." );
                const auto edge_vertices_id =
                    line_mesh.edge_vertices( edge_id );
                for( const auto edge_vertex_id : LRange{ 2 } )
                {
                    const auto vertex_id = edge_vertices_id[edge_vertex_id];
                    if( vertices_checked[vertex_id] )
                    {
                        continue;
                    }
                    vertices_checked[vertex_id] = true;
                    strati_line_builders[0]->set_point( vertex_id,
                        stratigraphic_coordinates( model, surface,
                            edge_surface_vertices.oriented_edge.value()
                                .vertices[edge_vertex_id] )
                            .stratigraphic_coordinates() );
                    strati_line_builders[1]->set_point( vertex_id,
                        stratigraphic_coordinates( model, surface,
                            edge_surface_vertices.opposite_edge.value()
                                .vertices[edge_vertex_id] )
                            .stratigraphic_coordinates() );
                }
                associated_polygon_edge_attributes[0]->set_value(
                    edge_id, edge_surface_vertices.oriented_edge.value().edge );
                associated_polygon_edge_attributes[1]->set_value(
                    edge_id, edge_surface_vertices.opposite_edge.value().edge );
            }
            return strati_lines;
        }

    private:
        absl::flat_hash_map< uuid, TriangulatedSurfacePointFunction< 2, 1 > >
            stratigraphic_location_attributes_;
        absl::flat_hash_map< uuid, CachedValue< AABBTree2D > >
            surface_stratigraphic_aabb_trees_;
        absl::flat_hash_map< uuid, StratigraphicDistanceToTriangle >
            surface_stratigraphic_distance_to_triangles_;
    };

    StratigraphicSection::StratigraphicSection()
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicSection::StratigraphicSection(
        StratigraphicSection&& other ) noexcept
        : ImplicitCrossSection{ std::move( other ) }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicSection::StratigraphicSection(
        ImplicitCrossSection&& implicit_section ) noexcept
        : ImplicitCrossSection{ std::move( implicit_section ) }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicSection::StratigraphicSection(
        CrossSection&& cross_section ) noexcept
        : ImplicitCrossSection{ std::move( cross_section ) }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicSection::StratigraphicSection(
        const StratigraphicSection& initial_model,
        Section&& section,
        const ModelGenericMapping& initial_to_section_mappings ) noexcept
        : ImplicitCrossSection{ initial_model, std::move( section ),
              initial_to_section_mappings }
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    StratigraphicSection::~StratigraphicSection() = default;

    StratigraphicSection StratigraphicSection::clone() const
    {
        return StratigraphicSection{ ImplicitCrossSection::clone() };
    }

    StratigraphicPoint2D StratigraphicSection::stratigraphic_coordinates(
        const Surface2D& surface, index_t vertex_id ) const
    {
        return impl_->stratigraphic_coordinates( *this, surface, vertex_id );
    }

    std::optional< StratigraphicPoint2D >
        StratigraphicSection::stratigraphic_coordinates(
            const Surface2D& surface, const Point2D& geometric_point ) const
    {
        return impl_->stratigraphic_coordinates(
            *this, surface, geometric_point );
    }

    StratigraphicPoint2D StratigraphicSection::stratigraphic_coordinates(
        const Surface2D& surface,
        const Point2D& geometric_point,
        index_t polygon_id ) const
    {
        return impl_->stratigraphic_coordinates(
            *this, surface, geometric_point, polygon_id );
    }

    std::optional< Point2D > StratigraphicSection::geometric_coordinates(
        const Surface2D& surface,
        const StratigraphicPoint2D& stratigraphic_point ) const
    {
        return impl_->geometric_coordinates(
            *this, surface, stratigraphic_point );
    }

    Point2D StratigraphicSection::geometric_coordinates(
        const Surface2D& surface,
        const StratigraphicPoint2D& stratigraphic_point,
        index_t polygon_id ) const
    {
        return impl_->geometric_coordinates(
            *this, surface, stratigraphic_point, polygon_id );
    }

    std::optional< index_t >
        StratigraphicSection::stratigraphic_containing_polygon(
            const Surface2D& surface,
            const StratigraphicPoint2D& stratigraphic_point ) const
    {
        return impl_->stratigraphic_containing_polygon(
            *this, surface, stratigraphic_point );
    }

    absl::InlinedVector< std::unique_ptr< EdgedCurve2D >, 2 >
        StratigraphicSection::stratigraphic_line(
            const Surface2D& surface, const Line2D& line ) const
    {
        return impl_->stratigraphic_line( *this, surface, line );
    }

    BoundingBox2D StratigraphicSection::stratigraphic_bounding_box() const
    {
        return impl_->stratigraphic_bounding_box( *this );
    }

    void StratigraphicSection::initialize_stratigraphic_query_trees(
        StratigraphicSectionBuilderKey )
    {
        impl_->initialize_stratigraphic_query_trees( *this );
    }

    void StratigraphicSection::instantiate_stratigraphic_location_on_surfaces(
        StratigraphicSectionBuilderKey )
    {
        impl_->instantiate_stratigraphic_location_on_surfaces( *this );
    }

    void StratigraphicSection::do_set_implicit_value(
        const Surface2D& surface, index_t vertex_id, double value )
    {
        ImplicitCrossSection::do_set_implicit_value(
            surface, vertex_id, value );
        impl_->reset_stratigraphic_aabb_tree( surface );
    }

    void StratigraphicSection::set_stratigraphic_location(
        const Surface2D& surface,
        index_t vertex_id,
        Point1D value,
        StratigraphicSectionBuilderKey )
    {
        impl_->set_stratigraphic_location( surface, vertex_id, value );
    }
} // namespace geode