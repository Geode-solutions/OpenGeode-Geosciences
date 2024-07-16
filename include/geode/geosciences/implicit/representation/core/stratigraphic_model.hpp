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

#pragma once

#include <geode/basic/pimpl.hpp>

#include <geode/geosciences/implicit/common.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicPoint );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
    ALIAS_2D_AND_3D( Point );
    ALIAS_2D_AND_3D( StratigraphicPoint );
    ALIAS_3D( TriangulatedSurface );
    class StratigraphicModelBuilder;
} // namespace geode

namespace geode
{
    /*!
     * A Stratigraphic Model is an ImplicitStructuralModel where each block also
     * has a specific attribute to store the stratigraphic coordinates of its
     * vertices.
     */
    class opengeode_geosciences_implicit_api StratigraphicModel
        : public ImplicitStructuralModel
    {
        PASSKEY( StratigraphicModelBuilder, StratigraphicModelBuilderKey );

    public:
        static constexpr auto STRATIGRAPHIC_LOCATION_ATTRIBUTE_NAME =
            "geode_stratigraphic_location";
        static constexpr auto
            STRATIGRAPHIC_SURFACE_POLYHEDRON_FACET_ATTRIBUTE_NAME =
                "geode_associated_block_polyhedron_facet";
        using stratigraphic_location_type = Point2D;
        StratigraphicModel();
        StratigraphicModel( StratigraphicModel&& implicit_model ) noexcept;
        StratigraphicModel(
            ImplicitStructuralModel&& structural_model ) noexcept;
        StratigraphicModel( StructuralModel&& structural_model ) noexcept;
        StratigraphicModel( const StratigraphicModel& initial_model,
            BRep&& brep,
            const ModelGenericMapping& initial_to_brep_mappings ) noexcept;
        ~StratigraphicModel();

        StratigraphicModel clone() const;

        static constexpr std::string_view native_extension_static()
        {
            return "og_stgm";
        }

        std::string_view native_extension() const
        {
            return native_extension_static();
        }

        /*!
         * Return the stratigraphic coordinates of the point at the given
         * vertex of the given block.
         */
        StratigraphicPoint3D stratigraphic_coordinates(
            const Block3D& block, index_t vertex_id ) const;

        /*!
         * Return the stratigraphic coordinates of the point, computed in the
         * polyhedron containing the given point in the given block, if there is
         * any.
         */
        std::optional< StratigraphicPoint3D > stratigraphic_coordinates(
            const Block3D& block, const Point3D& geometric_point ) const;

        /*!
         * Return the stratigraphic coordinates of the point, computed in the
         * given polyhedron of the given block.
         */
        StratigraphicPoint3D stratigraphic_coordinates( const Block3D& block,
            const Point3D& geometric_point,
            index_t polyhedron_id ) const;

        /*!
         * Return the geometric coordinates of the point, computed from its
         * stratigraphic coordinates in the polyhedron containing the given
         * coordinates in the stratigraphic space in the given block, if there
         * is any.
         */
        std::optional< Point3D > geometric_coordinates( const Block3D& block,
            const StratigraphicPoint3D& stratigraphic_point ) const;

        /*!
         * Return the geometric coordinates of the point, computed from its
         * stratigraphic coordinates in the given polyhedron of the given block.
         */
        Point3D geometric_coordinates( const Block3D& block,
            const StratigraphicPoint3D& stratigraphic_point,
            index_t polyhedron_id ) const;

        /*!
         * Returns the block polyhedron containing the given stratigraphic
         * point, if there is any.
         */
        std::optional< index_t > stratigraphic_containing_polyhedron(
            const Block3D& block,
            const StratigraphicPoint3D& stratigraphic_point ) const;

        absl::InlinedVector< std::unique_ptr< TriangulatedSurface3D >, 2 >
            stratigraphic_surface(
                const Block3D& block, const Surface3D& surface ) const;

        BoundingBox3D stratigraphic_bounding_box() const;

    public:
        void initialize_stratigraphic_query_trees(
            StratigraphicModelBuilderKey );

        void instantiate_stratigraphic_location_on_blocks(
            StratigraphicModelBuilderKey );

        void set_stratigraphic_location( const Block3D& block,
            index_t vertex_id,
            stratigraphic_location_type value,
            StratigraphicModelBuilderKey );

    private:
        void do_set_implicit_value( const Block3D& block,
            index_t vertex_id,
            implicit_attribute_type value ) override;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
