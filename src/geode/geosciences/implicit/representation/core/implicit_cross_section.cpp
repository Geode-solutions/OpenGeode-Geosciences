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

#include <geode/geosciences/implicit/representation/core/implicit_cross_section.h>

#include <async++.h>

#include <geode/basic/attribute_manager.h>
#include <geode/basic/bitsery_archive.h>
#include <geode/basic/cached_value.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>

#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>
#include <geode/mesh/helpers/triangulated_surface_scalar_function.h>

#include <geode/model/mixin/core/surface.h>

#include <geode/geosciences/implicit/representation/core/horizons_stack.h>

namespace geode
{
    class ImplicitCrossSection::Impl
    {
    public:
        Impl() = default;

        void initialize_implicit_query_trees(
            const ImplicitCrossSection& model )
        {
            implicit_attributes_.reserve( model.nb_surfaces() );
            instantiate_implicit_attribute_on_surfaces( model );
            surface_mesh_aabb_trees_.reserve( model.nb_surfaces() );
            for( const auto& surface : model.surfaces() )
            {
                surface_mesh_aabb_trees_.try_emplace( surface.id() );
            }
            surface_distance_to_triangles_.reserve( model.nb_surfaces() );
            build_model_distance_to_mesh_elements( model );
        }

        double implicit_value(
            const Surface2D& surface, index_t vertex_id ) const
        {
            return implicit_attributes_.at( surface.id() ).value( vertex_id );
        }

        absl::optional< double > implicit_value(
            const Surface2D& surface, const Point2D& point ) const
        {
            if( const auto containing_triangle =
                    containing_polygon( surface, point ) )
            {
                return implicit_value(
                    surface, point, containing_triangle.value() );
            }
            return absl::nullopt;
        }

        double implicit_value( const Surface2D& surface,
            const Point2D& point,
            index_t triangle_id ) const
        {
            return implicit_attributes_.at( surface.id() )
                .value( point, triangle_id );
        }

        absl::optional< index_t > containing_polygon(
            const Surface2D& surface, const Point2D& point ) const
        {
            const auto closest_triangle = std::get< 0 >(
                surface_mesh_aabb_trees_
                    .at( surface.id() )( create_aabb_tree, surface.mesh() )
                    .closest_element_box( point,
                        surface_distance_to_triangles_.at( surface.id() ) ) );
            if( std::get< 0 >( surface_distance_to_triangles_.at(
                    surface.id() )( point, closest_triangle ) )
                < global_epsilon )
            {
                return closest_triangle;
            }
            return absl::nullopt;
        }

        const HorizonsStack2D& horizons_stack() const
        {
            return horizons_stack_;
        }

        HorizonsStack2D& modifiable_horizons_stack()
        {
            return horizons_stack_;
        }

        absl::optional< double > horizon_implicit_value(
            const Horizon2D& horizon ) const
        {
            OPENGEODE_EXCEPTION( horizons_stack_.has_horizon( horizon.id() ),
                "[horizon_implicit_value] You cannot access the isovalue of "
                "Horizon ",
                horizon.id().string(),
                " because the horizon is not defined in the "
                "HorizonsStack." );
            const auto value = horizon_isovalues_.find( horizon.id() );
            if( value == horizon_isovalues_.end() )
            {
                return absl::nullopt;
            }
            return value->second;
        }

        bool implicit_value_is_above_horizon(
            double implicit_function_value, const Horizon2D& horizon ) const
        {
            const auto increasing = increasing_stack_isovalues();
            OPENGEODE_EXCEPTION( increasing.has_value(),
                "[implicit_value_is_above_horizon] Could not define if "
                "implicit values were "
                "increasing or decreasing in the horizon stack." );
            const auto it = horizon_isovalues_.find( horizon.id() );
            OPENGEODE_EXCEPTION( it != horizon_isovalues_.end(),
                "[implicit_value_is_above_horizon] Cannot find horizon "
                "implicit value in the horizon stack." );
            return increasing.value()
                   == ( implicit_function_value >= it->second );
        }

        absl::optional< uuid > containing_stratigraphic_unit(
            double implicit_function_value ) const
        {
            if( horizon_isovalues_.empty() )
            {
                return absl::nullopt;
            }
            const auto increasing = increasing_stack_isovalues();
            if( !increasing.has_value() )
            {
                return absl::nullopt;
            }
            auto horizon_id = horizon_isovalues_.begin()->first;
            while( true )
            {
                if( increasing.value()
                    == ( implicit_function_value
                         >= horizon_isovalues_.at( horizon_id ) ) )
                {
                    const auto unit_above = horizons_stack_.above( horizon_id );
                    if( !unit_above )
                    {
                        return absl::nullopt;
                    }
                    const auto horizon_above =
                        horizons_stack_.above( unit_above.value() );
                    if( !horizon_above
                        || increasing.value()
                               == ( implicit_function_value
                                    < horizon_isovalues_.at( horizon_id ) ) )
                    {
                        return unit_above.value();
                    }
                    horizon_id = horizon_above.value();
                    continue;
                }
                const auto unit_under = horizons_stack_.under( horizon_id );
                if( !unit_under )
                {
                    return absl::nullopt;
                }
                const auto horizon_under =
                    horizons_stack_.under( unit_under.value() );
                if( !horizon_under
                    || increasing.value()
                           == ( implicit_function_value
                                > horizon_isovalues_.at( horizon_id ) ) )
                {
                    return unit_under.value();
                }
                horizon_id = horizon_under.value();
            }
        }

        void instantiate_implicit_attribute_on_surfaces(
            const ImplicitCrossSection& model )
        {
            for( const auto& surface : model.surfaces() )
            {
                OPENGEODE_EXCEPTION(
                    ( surface.mesh().type_name()
                        == TriangulatedSurface2D::type_name_static() ),
                    "[ImplicitCrossSection::instantiate_implicit_"
                    "attribute_on_surfaces] Surfaces must be meshed as "
                    "TriangulatedSurface2D, which is not the case for surface "
                    "with uuid '",
                    surface.id().string(), "'." );
                if( !surface.mesh().vertex_attribute_manager().attribute_exists(
                        implicit_attribute_name ) )
                {
                    implicit_attributes_.try_emplace( surface.id(),
                        TriangulatedSurfaceScalarFunction2D::create(
                            surface.mesh< TriangulatedSurface2D >(),
                            implicit_attribute_name, 0 ) );
                }
                else
                {
                    implicit_attributes_.try_emplace( surface.id(),
                        TriangulatedSurfaceScalarFunction2D::find(
                            surface.mesh< TriangulatedSurface2D >(),
                            implicit_attribute_name ) );
                }
            }
        }

        void set_implicit_value(
            const Surface2D& surface, index_t vertex_id, double value )
        {
            OPENGEODE_EXCEPTION( implicit_attributes_.find( surface.id() )
                                     != implicit_attributes_.end(),
                "[ImplicitCrossSection::set_implicit_value] Couldn't find "
                "surface uuid in the attributes registered - Try instantiating "
                "your attribute first." );
            implicit_attributes_.at( surface.id() )
                .set_value( vertex_id, value );
        }

        void set_horizons_stack( HorizonsStack2D&& stack )
        {
            horizons_stack_ = std::move( stack );
        }

        void set_horizon_implicit_value(
            const Horizon2D& horizon, double isovalue )
        {
            OPENGEODE_EXCEPTION( horizons_stack_.has_horizon( horizon.id() ),
                "[horizon_implicit_value] You cannot change the isovalue of "
                "Horizon ",
                horizon.id().string(),
                " because the horizon is not defined in the "
                "HorizonsStack." );
            horizon_isovalues_[horizon.id()] = isovalue;
        }

    private:
        void build_model_distance_to_mesh_elements(
            const ImplicitCrossSection& model )
        {
            for( const auto& surface : model.surfaces() )
            {
                surface_distance_to_triangles_.try_emplace( surface.id(),
                    DistanceToTriangle2D{
                        surface.mesh< TriangulatedSurface2D >() } );
            }
        }

        absl::optional< bool > increasing_stack_isovalues() const
        {
            for( const auto& unit : horizons_stack_.stratigraphic_units() )
            {
                const auto above = horizons_stack_.above( unit.id() );
                const auto under = horizons_stack_.under( unit.id() );
                if( above && under )
                {
                    const auto it0 = horizon_isovalues_.find( above.value() );
                    const auto it1 = horizon_isovalues_.find( under.value() );
                    if( it0 == horizon_isovalues_.end()
                        || it1 == horizon_isovalues_.end() )
                    {
                        return absl::nullopt;
                    }
                    return it0->second > it1->second;
                }
            }
            return absl::nullopt;
        }

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive )
        {
            archive.ext( *this, Growable< Archive, Impl >{ { []( Archive& a,
                                                                 Impl& impl ) {
                a.ext( impl.horizon_isovalues_,
                    bitsery::ext::StdMap{ impl.horizon_isovalues_.max_size() },
                    []( Archive& a2, uuid& id, double item ) {
                        a2.object( id );
                        a2.value8b( item );
                    } );
            } } } );
        }

    private:
        absl::flat_hash_map< uuid, TriangulatedSurfaceScalarFunction2D >
            implicit_attributes_;
        HorizonsStack2D horizons_stack_;
        absl::flat_hash_map< uuid, double > horizon_isovalues_;
        absl::flat_hash_map< uuid, CachedValue< AABBTree2D > >
            surface_mesh_aabb_trees_;
        absl::flat_hash_map< uuid, DistanceToTriangle2D >
            surface_distance_to_triangles_;
    };

    ImplicitCrossSection::ImplicitCrossSection()
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    ImplicitCrossSection::ImplicitCrossSection(
        ImplicitCrossSection&& ) noexcept = default;

    ImplicitCrossSection::ImplicitCrossSection(
        CrossSection&& cross_section ) noexcept
        : CrossSection{ std::move( cross_section ) }
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    ImplicitCrossSection::~ImplicitCrossSection() = default;

    double ImplicitCrossSection::implicit_value(
        const Surface2D& surface, index_t vertex_id ) const
    {
        return impl_->implicit_value( surface, vertex_id );
    }

    absl::optional< double > ImplicitCrossSection::implicit_value(
        const Surface2D& surface, const Point2D& point ) const
    {
        return impl_->implicit_value( surface, point );
    }

    double ImplicitCrossSection::implicit_value( const Surface2D& surface,
        const Point2D& point,
        index_t polygon_id ) const
    {
        return impl_->implicit_value( surface, point, polygon_id );
    }

    absl::optional< index_t > ImplicitCrossSection::containing_polygon(
        const Surface2D& surface, const Point2D& point ) const
    {
        return impl_->containing_polygon( surface, point );
    }

    const HorizonsStack2D& ImplicitCrossSection::horizons_stack() const
    {
        return impl_->horizons_stack();
    }

    absl::optional< double > ImplicitCrossSection::horizon_implicit_value(
        const Horizon2D& horizon ) const
    {
        return horizon_implicit_value( horizon );
    }

    bool ImplicitCrossSection::implicit_value_is_above_horizon(
        double implicit_function_value, const Horizon2D& horizon ) const
    {
        return impl_->implicit_value_is_above_horizon(
            implicit_function_value, horizon );
    }

    absl::optional< uuid > ImplicitCrossSection::containing_stratigraphic_unit(
        double implicit_function_value ) const
    {
        return impl_->containing_stratigraphic_unit( implicit_function_value );
    }

    void ImplicitCrossSection::initialize_implicit_query_trees(
        ImplicitCrossSectionBuilderKey )
    {
        impl_->initialize_implicit_query_trees( *this );
    }

    void ImplicitCrossSection::instantiate_implicit_attribute_on_surfaces(
        ImplicitCrossSectionBuilderKey )
    {
        impl_->instantiate_implicit_attribute_on_surfaces( *this );
    }

    void ImplicitCrossSection::set_implicit_value( const Surface2D& surface,
        index_t vertex_id,
        double value,
        ImplicitCrossSectionBuilderKey )
    {
        do_set_implicit_value( surface, vertex_id, value );
    }

    void ImplicitCrossSection::set_horizons_stack(
        HorizonsStack2D&& stack, ImplicitCrossSectionBuilderKey )
    {
        impl_->set_horizons_stack( std::move( stack ) );
    }

    void ImplicitCrossSection::set_horizon_implicit_value(
        const Horizon2D& horizon,
        double isovalue,
        ImplicitCrossSectionBuilderKey )
    {
        impl_->set_horizon_implicit_value( horizon, isovalue );
    }

    HorizonsStack2D& ImplicitCrossSection::modifiable_horizons_stack(
        ImplicitCrossSectionBuilderKey )
    {
        return impl_->modifiable_horizons_stack();
    }

    void ImplicitCrossSection::do_set_implicit_value(
        const Surface2D& surface, index_t vertex_id, double value )
    {
        impl_->set_implicit_value( surface, vertex_id, value );
    }

    template < typename Archive >
    void ImplicitCrossSection::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, ImplicitCrossSection >{
                       { []( Archive& a, ImplicitCrossSection& model ) {
                           a.object( model.impl_ );
                       } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_implicit_api, ImplicitCrossSection );
} // namespace geode