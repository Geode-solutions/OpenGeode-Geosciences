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

#pragma once

#include <geode/basic/pimpl.h>

#include <geode/geosciences/explicit/representation/core/structural_model.h>
#include <geode/geosciences/implicit/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( StratigraphicUnitsStack );
    ALIAS_3D( Point );
    ALIAS_3D( StratigraphicUnitsStack );
    class ImplicitStructuralModelBuilder;
} // namespace geode

namespace geode
{
    /*!
     * An Implicit Model is a Structural model where each block has a specific
     * attribute to store the implicit value on its vertices.
     */
    class opengeode_geosciences_implicit_api ImplicitStructuralModel
        : public StructuralModel
    {
        PASSKEY(
            ImplicitStructuralModelBuilder, ImplicitStructuralModelBuilderKey );

    public:
        static constexpr auto implicit_attribute_name =
            "geode_implicit_attribute";
        using implicit_attribute_type = double;
        ImplicitStructuralModel();
        ImplicitStructuralModel( ImplicitStructuralModel&& implicit_model );
        ImplicitStructuralModel( StructuralModel&& structural_model );
        virtual ~ImplicitStructuralModel();

        static constexpr absl::string_view native_extension_static()
        {
            return "og_istrm";
        }

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        /*!
         * Return the implicit value at the given vertex of the given block.
         */
        implicit_attribute_type implicit_value(
            const Block3D& block, index_t vertex_id ) const;

        /*!
         * Return the implicit value on the point, computed in the polyhedron
         * containing the given point in the given block, if there is any.
         */
        absl::optional< implicit_attribute_type > implicit_value(
            const Block3D& block, const Point3D& point ) const;

        /*!
         * Return the implicit value on the point, computed in the given
         * polyhedron of the given block.
         */
        implicit_attribute_type implicit_value( const Block3D& block,
            const Point3D& point,
            index_t polyhedron_id ) const;

        /*!
         * Returns the block polyhedron containing the given point, if there is
         * any.
         */
        absl::optional< index_t > containing_polyhedron(
            const Block3D& block, const Point3D& point ) const;

        const StratigraphicUnitsStack3D& stratigraphic_units_stack() const;

        double horizon_implicit_value( const Horizon3D& horizon ) const;

    public:
        void instantiate_implicit_attribute_on_blocks(
            ImplicitStructuralModelBuilderKey );

        void set_implicit_value( const Block3D& block,
            index_t vertex_id,
            implicit_attribute_type value,
            ImplicitStructuralModelBuilderKey );

        void set_stratigraphic_units_stack( StratigraphicUnitsStack3D&& stack,
            ImplicitStructuralModelBuilderKey );

        void set_horizon_isovalue( const Horizon3D& horizon,
            double isovalue,
            ImplicitStructuralModelBuilderKey );

    protected:
        virtual void do_set_implicit_value( const Block3D& block,
            index_t vertex_id,
            implicit_attribute_type value );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
