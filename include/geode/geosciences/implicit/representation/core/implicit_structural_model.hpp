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

#include <optional>

#include <geode/basic/pimpl.hpp>

#include <geode/geosciences/explicit/representation/core/structural_model.hpp>
#include <geode/geosciences/implicit/common.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( HorizonsStack );
    ALIAS_3D( Point );
    ALIAS_3D( HorizonsStack );
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
        static constexpr auto IMPLICIT_ATTRIBUTE_NAME =
            "geode_implicit_attribute";
        using implicit_attribute_type = double;
        ImplicitStructuralModel();
        ImplicitStructuralModel(
            ImplicitStructuralModel&& implicit_model ) noexcept;
        ImplicitStructuralModel( StructuralModel&& structural_model ) noexcept;
        ImplicitStructuralModel( const ImplicitStructuralModel& initial_model,
            BRep&& brep,
            const ModelGenericMapping& initial_to_brep_mappings ) noexcept;
        virtual ~ImplicitStructuralModel();

        ImplicitStructuralModel clone() const;

        static constexpr std::string_view native_extension_static()
        {
            return "og_istrm";
        }

        std::string_view native_extension() const
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
        std::optional< implicit_attribute_type > implicit_value(
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
        std::optional< index_t > containing_polyhedron(
            const Block3D& block, const Point3D& point ) const;

        const HorizonsStack3D& horizons_stack() const;

        std::optional< implicit_attribute_type > horizon_implicit_value(
            const Horizon3D& horizon ) const;

        bool implicit_value_is_above_horizon(
            double implicit_function_value, const Horizon3D& horizon ) const;

        std::optional< uuid > containing_stratigraphic_unit(
            implicit_attribute_type implicit_function_value ) const;

    public:
        void initialize_implicit_query_trees(
            ImplicitStructuralModelBuilderKey );

        void instantiate_implicit_attribute_on_blocks(
            ImplicitStructuralModelBuilderKey );

        void set_implicit_value( const Block3D& block,
            index_t vertex_id,
            implicit_attribute_type value,
            ImplicitStructuralModelBuilderKey );

        void set_horizons_stack(
            HorizonsStack3D&& stack, ImplicitStructuralModelBuilderKey );

        void set_horizon_implicit_value( const Horizon3D& horizon,
            implicit_attribute_type isovalue,
            ImplicitStructuralModelBuilderKey );

        HorizonsStack3D& modifiable_horizons_stack(
            ImplicitStructuralModelBuilderKey );

    protected:
        virtual void do_set_implicit_value( const Block3D& block,
            index_t vertex_id,
            implicit_attribute_type value );

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode
