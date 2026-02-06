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
                builder_.set_stratigraphic_unit_name(
                    stratigraphic_unit.id(), stratigraphic_unit.name() );
            }
        }

        ~BlockToStratigraphicUnitBuilder()
        {
            std::vector< geode::uuid > to_remove;
            for( const auto& stratigraphic_unit : model_.stratigraphic_units() )
            {
                if( model_.nb_items( stratigraphic_unit.id() ) == 0 )
                {
                    to_remove.push_back( stratigraphic_unit.id() );
                }
            }
            for( const auto& stratigraphic_unit_id : to_remove )
            {
                builder_.remove_stratigraphic_unit(
                    model_.stratigraphic_unit( stratigraphic_unit_id ) );
            }
        }

        geode::StratigraphicUnitToBlockResult
            assign_blocks_to_stratigraphic_units()
        {
            bool run{ false };
            for( const auto& block : model_.blocks() )
            {
                if( is_in_stratigraphic_unit( block ) )
                {
                    result_.already_assigned_blocks.push_back( block.id() );
                }
                else
                {
                    run = true;
                }
            }
            while( run )
            {
                run = one_step_assign_blocks_to_stratigraphic_units();
            }
            for( const auto& block : model_.blocks() )
            {
                if( !is_in_stratigraphic_unit( block ) )
                {
                    result_.unassigned_blocks.push_back( block.id() );
                }
            }
            return result_;
        }

    private:
        bool one_step_assign_blocks_to_stratigraphic_units()
        {
            bool block_assigned_in_step{ false };
            for( const auto& block : model_.blocks() )
            {
                if( is_in_stratigraphic_unit( block ) )
                {
                    continue;
                }
                if( process_block( block ) )
                {
                    result_.assigned_blocks.push_back( block.id() );
                    block_assigned_in_step = true;
                }
            }
            return block_assigned_in_step;
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

        bool is_in_stratigraphic_unit( const geode::Block3D& block )
        {
            for( const auto& block_collection :
                model_.collections( block.id() ) )
            {
                if( block_collection.type()
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
                horizons_stack_.above( conformal_horizon.id() ).value();
            const auto under_stratigraphic_unit =
                horizons_stack_.under( conformal_horizon.id() ).value();
            if( model_.is_item( neighbor.value(), above_stratigraphic_unit ) )
            {
                builder_.add_block_in_stratigraphic_unit( block,
                    model_.stratigraphic_unit( under_stratigraphic_unit ) );
                return true;
            }
            if( model_.is_item( neighbor.value(), under_stratigraphic_unit ) )
            {
                builder_.add_block_in_stratigraphic_unit( block,
                    model_.stratigraphic_unit( above_stratigraphic_unit ) );
                return true;
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
                horizons_stack_.under( erosion.id() ).value();
            if( model_.is_item( neighbor.value(), under_stratigraphic_unit ) )
            {
                const auto above_stratigraphic_unit =
                    horizons_stack_.above( erosion.id() ).value();
                builder_.add_block_in_stratigraphic_unit( block,
                    model_.stratigraphic_unit( above_stratigraphic_unit ) );
                return true;
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
                horizons_stack_.above( erosion.id() ).value();
            if( model_.is_item( neighbor.value(), above_stratigraphic_unit ) )
            {
                const auto under_stratigraphic_unit =
                    horizons_stack_.under( erosion.id() ).value();
                builder_.add_block_in_stratigraphic_unit( block,
                    model_.stratigraphic_unit( under_stratigraphic_unit ) );
                return true;
            }
            return false;
        }

        bool assign_between_two_conformal_horizons( const geode::Block3D& block,
            const geode::SmallSet< geode::uuid >& conformal_horizons )
        {
            const auto above_stratigraphic_unit =
                horizons_stack_.above( conformal_horizons.at( 0 ) ).value();
            if( above_stratigraphic_unit
                == horizons_stack_.under( conformal_horizons.at( 1 ) ).value() )
            {
                builder_.add_block_in_stratigraphic_unit( block,
                    model_.stratigraphic_unit( above_stratigraphic_unit ) );
                return true;
            }
            const auto under_stratigraphic_unit =
                horizons_stack_.under( conformal_horizons.at( 0 ) ).value();
            if( under_stratigraphic_unit
                == horizons_stack_.above( conformal_horizons.at( 1 ) ).value() )
            {
                builder_.add_block_in_stratigraphic_unit( block,
                    model_.stratigraphic_unit( under_stratigraphic_unit ) );
                return true;
            }
            return false;
        }

        bool assign_between_one_conformal_and_one_erosion(
            const geode::Block3D& block,
            const geode::uuid& conformal_horizon_id,
            const geode::uuid& erosion_id )
        {
            if( horizons_stack_.is_above( erosion_id, conformal_horizon_id ) )
            {
                builder_.add_block_in_stratigraphic_unit(
                    block, model_.stratigraphic_unit(
                               horizons_stack_.above( conformal_horizon_id )
                                   .value() ) );
                return true;
            }
            if( horizons_stack_.is_directly_above(
                    horizons_stack_.above( conformal_horizon_id ).value(),
                    erosion_id ) )
            {
                builder_.add_block_in_stratigraphic_unit(
                    block, model_.stratigraphic_unit(
                               horizons_stack_.under( conformal_horizon_id )
                                   .value() ) );
                return true;
            }
            return false;
        }

        bool assign_between_one_conformal_and_one_baselap(
            const geode::Block3D& block,
            const geode::uuid& conformal_horizon_id,
            const geode::uuid& baselap_id )
        {
            if( horizons_stack_.is_above( conformal_horizon_id, baselap_id ) )
            {
                builder_.add_block_in_stratigraphic_unit(
                    block, model_.stratigraphic_unit(
                               horizons_stack_.under( conformal_horizon_id )
                                   .value() ) );
                return true;
            }
            if( horizons_stack_.is_directly_above(
                    horizons_stack_.above( baselap_id ).value(),
                    conformal_horizon_id ) )
            {
                builder_.add_block_in_stratigraphic_unit(
                    block, model_.stratigraphic_unit(
                               horizons_stack_.above( conformal_horizon_id )
                                   .value() ) );
                return true;
            }
            return false;
        }

        std::optional< HorizonsData > assign_or_collect_horizons_data(
            const geode::Block3D& block )
        {
            std::optional< HorizonsData > horizons_data{ std::in_place };
            for( const auto& boundary : model_.boundaries( block ) )
            {
                for( const auto& collection :
                    model_.collections( boundary.id() ) )
                {
                    if( collection.type()
                        != geode::Horizon3D::component_type_static() )
                    {
                        continue;
                    }
                    const auto& horizon = model_.horizon( collection.id() );
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
        OPENGEODE_DATA_EXCEPTION( horizons_stack.nb_stratigraphic_units() > 0,
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Horizons stack is empty (no stratigraphic units)." );
        OPENGEODE_DATA_EXCEPTION( horizons_stack.nb_horizons() > 0,
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Horizons stack is empty (no horizons)." );
        OPENGEODE_DATA_EXCEPTION(
            horizons_stack.top_horizon().has_value()
                && horizons_stack.bottom_horizon().has_value(),
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Horizons stack is empty (no top and bottom horizons)." );
        OPENGEODE_DATA_EXCEPTION( implicit_model.nb_horizons() > 0,
            "[complete_stratigraphic_unit_block_relationships_from_available_"
            "data] Implicit structural model has no horizons." );
        BlockToStratigraphicUnitBuilder builder{ implicit_model };
        return builder.assign_blocks_to_stratigraphic_units();
    }
} // namespace geode