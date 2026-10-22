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
#include <geode/geosciences/implicit/representation/builder/helpers/implicit_structural_model_stratigraphic_blocks_builder.hpp>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <geode/basic/small_set.hpp>

#include <geode/geosciences/implicit/representation/builder/implicit_structural_model_builder.hpp>
#include <geode/geosciences/implicit/representation/core/horizons_stack.hpp>
#include <geode/geosciences/implicit/representation/core/implicit_structural_model.hpp>

namespace
{
    struct HorizonsData
    {
        geode::SmallSet< geode::uuid > conformal;
        geode::SmallSet< geode::uuid > erosion;
        geode::SmallSet< geode::uuid > baselap;
        geode::SmallSet< geode::uuid > other;
    };

    class BlockToStratigraphicUnitBuilder
    {
    public:
        BlockToStratigraphicUnitBuilder( geode::ImplicitStructuralModel& model )
            : model_{ model },
              horizons_stack_{ model.horizons_stack() },
              builder_{ model }
        {
            for( const auto& stratigraphic_unit :
                horizons_stack_.stratigraphic_units() )
            {
                if( model_.has_stratigraphic_unit( stratigraphic_unit.id() ) )
                {
                    continue;
                }
                builder_.add_stratigraphic_unit( stratigraphic_unit.id() );
                created_stratigraphic_units_.push_back(
                    stratigraphic_unit.id() );
                if( const auto name = stratigraphic_unit.name() )
                {
                    builder_.set_stratigraphic_unit_name(
                        stratigraphic_unit, name.value() );
                }
            }
        }

        /*!
         * Remove the stratigraphic units created by this builder which have
         * not been assigned any block.
         */
        void remove_empty_created_stratigraphic_units()
        {
            for( const auto& stratigraphic_unit_id :
                created_stratigraphic_units_ )
            {
                if( model_.has_stratigraphic_unit( stratigraphic_unit_id )
                    && model_.nb_items( stratigraphic_unit_id ) == 0 )
                {
                    builder_.remove_stratigraphic_unit(
                        model_.stratigraphic_unit( stratigraphic_unit_id ) );
                }
            }
            created_stratigraphic_units_.clear();
        }

        geode::StratigraphicUnitToBlockResult
            assign_blocks_to_stratigraphic_units()
        {
            for( const auto& block : model_.blocks() )
            {
                if( is_in_stratigraphic_unit( block ) )
                {
                    result_.already_assigned_blocks.push_back( block.id() );
                    assigned_blocks_.insert( block.id() );
                }
            }
            std::vector< const geode::Block3D* > to_process;
            for( const auto& block : model_.blocks() )
            {
                try_assign_block( block, to_process );
            }
            while( !to_process.empty() )
            {
                const auto& block = *to_process.back();
                to_process.pop_back();
                try_assign_block( block, to_process );
            }
            for( const auto& block : model_.blocks() )
            {
                if( !assigned_blocks_.contains( block.id() ) )
                {
                    result_.unassigned_blocks.push_back( block.id() );
                }
            }
            return result_;
        }

    private:
        struct HorizonBoundary
        {
            const geode::Surface3D& surface;
            const geode::Horizon3D& horizon;
        };

        /*!
         * Try to assign the block and, if successful, add its unassigned
         * neighbors through horizons to the blocks to process, since they may
         * now be assigned from this block.
         */
        void try_assign_block( const geode::Block3D& block,
            std::vector< const geode::Block3D* >& to_process )
        {
            if( assigned_blocks_.contains( block.id() ) )
            {
                return;
            }
            if( !process_block( block ) )
            {
                return;
            }
            result_.assigned_blocks.push_back( block.id() );
            assigned_blocks_.insert( block.id() );
            for( const auto& boundary : horizon_boundaries( block ) )
            {
                const auto neighbor = neighbor_block( block, boundary.surface );
                if( neighbor && !assigned_blocks_.contains( neighbor.value() ) )
                {
                    to_process.push_back( &model_.block( neighbor.value() ) );
                }
            }
        }

        const std::vector< HorizonBoundary >& horizon_boundaries(
            const geode::Block3D& block )
        {
            const auto [it, inserted] =
                block_horizon_boundaries_.try_emplace( block.id() );
            if( !inserted )
            {
                return it->second;
            }
            for( const auto& boundary : model_.boundaries( block ) )
            {
                for( const auto& collection :
                    model_.collections( boundary.id() ) )
                {
                    if( collection.type
                        != geode::Horizon3D::component_type_static() )
                    {
                        continue;
                    }
                    if( !horizons_stack_.has_horizon( collection.id ) )
                    {
                        continue;
                    }
                    it->second.push_back(
                        { boundary, model_.horizon( collection.id ) } );
                }
            }
            return it->second;
        }

        bool process_block( const geode::Block3D& block )
        {
            const auto horizons_data = assign_or_collect_horizons_data( block );
            if( !horizons_data )
            {
                return true;
            }
            if( process_block_with_horizon_data(
                    block, horizons_data.value() ) )
            {
                return true;
            }
            return false;
        }

        bool is_in_stratigraphic_unit( const geode::Block3D& block ) const
        {
            for( const auto& block_collection :
                model_.collections( block.id() ) )
            {
                if( block_collection.type
                    == geode::StratigraphicUnit3D::component_type_static() )
                {
                    return true;
                }
            }
            return false;
        }

        std::optional< geode::uuid > neighbor_block(
            const geode::Block3D& block,
            const geode::Surface3D& boundary ) const
        {
            for( const auto& incidence : model_.incidences( boundary ) )
            {
                if( incidence.id() != block.id() )
                {
                    return incidence.id();
                }
            }
            return std::nullopt;
        }

        bool add_block_in_stratigraphic_unit( const geode::Block3D& block,
            const std::optional< geode::uuid >& stratigraphic_unit_id )
        {
            if( !stratigraphic_unit_id )
            {
                return false;
            }
            builder_.add_block_in_stratigraphic_unit( block,
                model_.stratigraphic_unit( stratigraphic_unit_id.value() ) );
            return true;
        }

        bool assign_through_conformal_horizon( const geode::Block3D& block,
            const geode::Surface3D& block_boundary,
            const geode::Horizon3D& conformal_horizon )
        {
            const auto neighbor = neighbor_block( block, block_boundary );
            if( !neighbor )
            {
                return false;
            }
            const auto above_stratigraphic_unit =
                horizons_stack_.above( conformal_horizon.id() );
            const auto under_stratigraphic_unit =
                horizons_stack_.under( conformal_horizon.id() );
            if( above_stratigraphic_unit && under_stratigraphic_unit
                && model_.is_item(
                    neighbor.value(), above_stratigraphic_unit.value() ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, under_stratigraphic_unit );
            }
            if( above_stratigraphic_unit && under_stratigraphic_unit
                && model_.is_item(
                    neighbor.value(), under_stratigraphic_unit.value() ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, above_stratigraphic_unit );
            }
            return false;
        }

        bool assign_through_erosion( const geode::Block3D& block,
            const geode::Surface3D& block_boundary,
            const geode::Horizon3D& erosion )
        {
            const auto neighbor = neighbor_block( block, block_boundary );
            if( !neighbor )
            {
                return false;
            }
            const auto under_stratigraphic_unit =
                horizons_stack_.under( erosion.id() );
            if( under_stratigraphic_unit
                && model_.is_item(
                    neighbor.value(), under_stratigraphic_unit.value() ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, horizons_stack_.above( erosion.id() ) );
            }
            return false;
        }

        bool assign_through_baselap( const geode::Block3D& block,
            const geode::Surface3D& block_boundary,
            const geode::Horizon3D& erosion )
        {
            const auto neighbor = neighbor_block( block, block_boundary );
            if( !neighbor )
            {
                return false;
            }
            const auto above_stratigraphic_unit =
                horizons_stack_.above( erosion.id() );
            if( above_stratigraphic_unit
                && model_.is_item(
                    neighbor.value(), above_stratigraphic_unit.value() ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, horizons_stack_.under( erosion.id() ) );
            }
            return false;
        }

        bool assign_between_two_conformal_horizons( const geode::Block3D& block,
            const geode::SmallSet< geode::uuid >& conformal_horizons )
        {
            const auto above_stratigraphic_unit =
                horizons_stack_.above( conformal_horizons.at( 0 ) );
            if( above_stratigraphic_unit
                && above_stratigraphic_unit
                       == horizons_stack_.under( conformal_horizons.at( 1 ) ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, above_stratigraphic_unit );
            }
            const auto under_stratigraphic_unit =
                horizons_stack_.under( conformal_horizons.at( 0 ) );
            if( under_stratigraphic_unit
                && under_stratigraphic_unit
                       == horizons_stack_.above( conformal_horizons.at( 1 ) ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, under_stratigraphic_unit );
            }
            return false;
        }

        bool assign_between_one_conformal_and_one_erosion(
            const geode::Block3D& block,
            const geode::uuid& conformal_horizon_id,
            const geode::uuid& erosion_id )
        {
            // Block between the conformal horizon (bottom) and the erosion
            if( horizons_stack_.is_above( erosion_id, conformal_horizon_id ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, horizons_stack_.above( conformal_horizon_id ) );
            }
            // Block between the erosion (bottom) and the conformal horizon
            if( horizons_stack_.is_above( conformal_horizon_id, erosion_id ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, horizons_stack_.under( conformal_horizon_id ) );
            }
            return false;
        }

        bool assign_between_one_conformal_and_one_baselap(
            const geode::Block3D& block,
            const geode::uuid& conformal_horizon_id,
            const geode::uuid& baselap_id )
        {
            // Block between the baselap (bottom) and the conformal horizon
            if( horizons_stack_.is_above( conformal_horizon_id, baselap_id ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, horizons_stack_.under( conformal_horizon_id ) );
            }
            // Block between the conformal horizon (bottom) and the baselap
            if( horizons_stack_.is_above( baselap_id, conformal_horizon_id ) )
            {
                return add_block_in_stratigraphic_unit(
                    block, horizons_stack_.above( conformal_horizon_id ) );
            }
            return false;
        }

        std::optional< HorizonsData > assign_or_collect_horizons_data(
            const geode::Block3D& block )
        {
            std::optional< HorizonsData > horizons_data{ std::in_place };
            for( const auto& [boundary, horizon] : horizon_boundaries( block ) )
            {
                if( horizon.contact_type()
                    == geode::Horizon3D::CONTACT_TYPE::conformal )
                {
                    if( assign_through_conformal_horizon(
                            block, boundary, horizon ) )
                    {
                        return std::nullopt;
                    }
                    horizons_data->conformal.insert( horizon.id() );
                }
                else if( horizon.contact_type()
                         == geode::Horizon3D::CONTACT_TYPE::erosion )
                {
                    if( assign_through_erosion( block, boundary, horizon ) )
                    {
                        return std::nullopt;
                    }
                    horizons_data->erosion.insert( horizon.id() );
                }
                else if( horizon.contact_type()
                         == geode::Horizon3D::CONTACT_TYPE::baselap )
                {
                    if( assign_through_baselap( block, boundary, horizon ) )
                    {
                        return std::nullopt;
                    }
                    horizons_data->baselap.insert( horizon.id() );
                }
                else
                {
                    horizons_data->other.insert( horizon.id() );
                }
            }
            return horizons_data;
        }

        bool process_block_with_horizon_data(
            const geode::Block3D& block, const HorizonsData& horizons_data )
        {
            if( horizons_data.conformal.empty() )
            {
                return false;
            }
            if( horizons_data.conformal.size() == 2 )
            {
                return assign_between_two_conformal_horizons(
                    block, horizons_data.conformal );
            }
            if( horizons_data.conformal.size() > 2 )
            {
                // Ambiguous configuration: let the neighbors decide
                return false;
            }
            // only one conformal horizon
            if( !horizons_data.erosion.empty() )
            {
                return assign_between_one_conformal_and_one_erosion( block,
                    horizons_data.conformal.at( 0 ),
                    horizons_data.erosion.at( 0 ) );
            }
            if( !horizons_data.baselap.empty() )
            {
                return assign_between_one_conformal_and_one_baselap( block,
                    horizons_data.conformal.at( 0 ),
                    horizons_data.baselap.at( 0 ) );
            }
            return false;
        }

    private:
        const geode::ImplicitStructuralModel& model_;
        const geode::HorizonsStack3D& horizons_stack_;
        geode::ImplicitStructuralModelBuilder builder_;
        geode::StratigraphicUnitToBlockResult result_;
        std::vector< geode::uuid > created_stratigraphic_units_;
        absl::flat_hash_set< geode::uuid > assigned_blocks_;
        absl::flat_hash_map< geode::uuid, std::vector< HorizonBoundary > >
            block_horizon_boundaries_;
    };
} // namespace
namespace geode
{
    StratigraphicUnitToBlockResult
        complete_stratigraphic_unit_block_relationships(
            ImplicitStructuralModel& implicit_model )
    {
        if( implicit_model.nb_horizons() == 0 )
        {
            return {};
        }
        const auto& horizons_stack = implicit_model.horizons_stack();
        OpenGeodeGeosciencesImplicitException::check_exception(
            horizons_stack.nb_stratigraphic_units() > 0, nullptr,
            OpenGeodeException::TYPE::data,
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Horizons stack is empty (no stratigraphic units)." );
        OpenGeodeGeosciencesImplicitException::check_exception(
            horizons_stack.nb_horizons() > 0, nullptr,
            OpenGeodeException::TYPE::data,
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Horizons stack is empty (no horizons)." );
        OpenGeodeGeosciencesImplicitException::check_exception(
            horizons_stack.top_horizon().has_value()
                && horizons_stack.bottom_horizon().has_value(),
            nullptr, OpenGeodeException::TYPE::data,
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Horizons stack is empty (no top and bottom horizons)." );
        OpenGeodeGeosciencesImplicitException::check_exception(
            implicit_model.nb_horizons() > 0, nullptr,
            OpenGeodeException::TYPE::data,
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Implicit structural model has no horizons." );
        BlockToStratigraphicUnitBuilder builder{ implicit_model };
        try
        {
            auto result = builder.assign_blocks_to_stratigraphic_units();
            builder.remove_empty_created_stratigraphic_units();
            return result;
        }
        catch( ... )
        {
            builder.remove_empty_created_stratigraphic_units();
            throw;
        }
    }
} // namespace geode