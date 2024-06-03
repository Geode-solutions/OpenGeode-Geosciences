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

#include <geode/geosciences/explicit/representation/core/cross_section.h>

#include <geode/model/representation/core/detail/clone.h>

#include <geode/geosciences/explicit/representation/builder/cross_section_builder.h>
#include <geode/geosciences/explicit/representation/core/detail/clone.h>

namespace
{
    void copy_faults_items_from_mappings(
        const geode::CrossSection& initial_model,
        const geode::CrossSection& current_model,
        geode::CrossSectionBuilder& builder,
        const geode::GenericMapping< geode::uuid >& line_mappings ) noexcept
    {
        for( const auto& fault : initial_model.faults() )
        {
            const auto& this_fault = current_model.fault( fault.id() );
            for( const auto& fault_line : initial_model.fault_items( fault ) )
            {
                if( !line_mappings.has_mapping_input( fault_line.id() ) )
                {
                    continue;
                }
                for( const auto& out_line_id :
                    line_mappings.in2out( fault_line.id() ) )
                {
                    builder.add_line_in_fault(
                        current_model.line( out_line_id ), this_fault );
                }
            }
        }
    }

    void copy_horizons_items_from_mappings(
        const geode::CrossSection& initial_model,
        const geode::CrossSection& current_model,
        geode::CrossSectionBuilder& builder,
        const geode::GenericMapping< geode::uuid >& line_mappings ) noexcept
    {
        for( const auto& horizon : initial_model.horizons() )
        {
            const auto& this_horizon = current_model.horizon( horizon.id() );
            for( const auto& horizon_line :
                initial_model.horizon_items( horizon ) )
            {
                if( !line_mappings.has_mapping_input( horizon_line.id() ) )
                {
                    continue;
                }
                for( const auto& out_line_id :
                    line_mappings.in2out( horizon_line.id() ) )
                {
                    builder.add_line_in_horizon(
                        current_model.line( out_line_id ), this_horizon );
                }
            }
        }
    }

    void copy_fault_blocks_items_from_mappings(
        const geode::CrossSection& initial_model,
        const geode::CrossSection& current_model,
        geode::CrossSectionBuilder& builder,
        const geode::GenericMapping< geode::uuid >& surface_mappings ) noexcept
    {
        for( const auto& fault_block : initial_model.fault_blocks() )
        {
            const auto& this_fault_block =
                current_model.fault_block( fault_block.id() );
            for( const auto& fault_block_surface :
                initial_model.fault_block_items( fault_block ) )
            {
                if( !surface_mappings.has_mapping_input(
                        fault_block_surface.id() ) )
                {
                    continue;
                }
                for( const auto& out_surface_id :
                    surface_mappings.in2out( fault_block_surface.id() ) )
                {
                    builder.add_surface_in_fault_block(
                        current_model.surface( out_surface_id ),
                        this_fault_block );
                }
            }
        }
    }

    void copy_stratigraphic_units_items_from_mappings(
        const geode::CrossSection& initial_model,
        const geode::CrossSection& current_model,
        geode::CrossSectionBuilder& builder,
        const geode::GenericMapping< geode::uuid >& surface_mappings ) noexcept
    {
        for( const auto& stratigraphic_unit :
            initial_model.stratigraphic_units() )
        {
            const auto& this_stratigraphic_unit =
                current_model.stratigraphic_unit( stratigraphic_unit.id() );
            for( const auto& stratigraphic_unit_surface :
                initial_model.stratigraphic_unit_items( stratigraphic_unit ) )
            {
                if( !surface_mappings.has_mapping_input(
                        stratigraphic_unit_surface.id() ) )
                {
                    continue;
                }
                for( const auto& out_surface_id :
                    surface_mappings.in2out( stratigraphic_unit_surface.id() ) )
                {
                    builder.add_surface_in_stratigraphic_unit(
                        current_model.surface( out_surface_id ),
                        this_stratigraphic_unit );
                }
            }
        }
    }
} // namespace

namespace geode
{
    CrossSection::HorizonItemRange::HorizonItemRange(
        const CrossSection& cross_section, const Horizon2D& horizon )
        : Relationships::ItemRangeIterator( cross_section, horizon.id() ),
          cross_section_( cross_section )
    {
    }
    CrossSection::HorizonItemRange::HorizonItemRange(
        const HorizonItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::HorizonItemRange::~HorizonItemRange() = default;

    auto CrossSection::HorizonItemRange::begin() const
        -> const HorizonItemRange&
    {
        return *this;
    }

    auto CrossSection::HorizonItemRange::end() const -> const HorizonItemRange&
    {
        return *this;
    }

    const Line2D& CrossSection::HorizonItemRange::operator*() const
    {
        return cross_section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::HorizonItemRange CrossSection::horizon_items(
        const Horizon2D& horizon ) const
    {
        return { *this, horizon };
    }

    CrossSection::FaultItemRange::FaultItemRange(
        const CrossSection& cross_section, const Fault2D& fault )
        : Relationships::ItemRangeIterator( cross_section, fault.id() ),
          cross_section_( cross_section )
    {
    }

    CrossSection::FaultItemRange::FaultItemRange( const FaultItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::FaultItemRange::~FaultItemRange() = default;

    auto CrossSection::FaultItemRange::begin() const -> const FaultItemRange&
    {
        return *this;
    }

    auto CrossSection::FaultItemRange::end() const -> const FaultItemRange&
    {
        return *this;
    }

    const Line2D& CrossSection::FaultItemRange::operator*() const
    {
        return cross_section_.line(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::FaultItemRange CrossSection::fault_items(
        const Fault2D& fault ) const
    {
        return { *this, fault };
    }

    CrossSection::FaultBlockItemRange::FaultBlockItemRange(
        const CrossSection& cross_section, const FaultBlock2D& fault_block )
        : Relationships::ItemRangeIterator( cross_section, fault_block.id() ),
          cross_section_( cross_section )
    {
    }

    CrossSection::FaultBlockItemRange::FaultBlockItemRange(
        const FaultBlockItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::FaultBlockItemRange::~FaultBlockItemRange() = default;

    auto CrossSection::FaultBlockItemRange::begin() const
        -> const FaultBlockItemRange&
    {
        return *this;
    }

    auto CrossSection::FaultBlockItemRange::end() const
        -> const FaultBlockItemRange&
    {
        return *this;
    }

    const Surface2D& CrossSection::FaultBlockItemRange::operator*() const
    {
        return cross_section_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::FaultBlockItemRange CrossSection::fault_block_items(
        const FaultBlock2D& fault_block ) const
    {
        return { *this, fault_block };
    }

    CrossSection::StratigraphicUnitItemRange::StratigraphicUnitItemRange(
        const CrossSection& cross_section,
        const StratigraphicUnit2D& stratigraphic_unit )
        : Relationships::ItemRangeIterator(
            cross_section, stratigraphic_unit.id() ),
          cross_section_( cross_section )
    {
    }

    CrossSection::StratigraphicUnitItemRange::StratigraphicUnitItemRange(
        const StratigraphicUnitItemRange& range )
        : Relationships::ItemRangeIterator{ range },
          cross_section_( range.cross_section_ )
    {
    }

    CrossSection::StratigraphicUnitItemRange::~StratigraphicUnitItemRange() =
        default;

    auto CrossSection::StratigraphicUnitItemRange::begin() const
        -> const StratigraphicUnitItemRange&
    {
        return *this;
    }

    auto CrossSection::StratigraphicUnitItemRange::end() const
        -> const StratigraphicUnitItemRange&
    {
        return *this;
    }

    const Surface2D& CrossSection::StratigraphicUnitItemRange::operator*() const
    {
        return cross_section_.surface(
            Relationships::ItemRangeIterator::operator*().id() );
    }

    CrossSection::StratigraphicUnitItemRange
        CrossSection::stratigraphic_unit_items(
            const StratigraphicUnit2D& stratigraphic_unit ) const
    {
        return { *this, stratigraphic_unit };
    }

    CrossSection::CrossSection( Section&& section ) noexcept
        : Section{ std::move( section ) }
    {
    }

    CrossSection::CrossSection( const CrossSection& initial_model,
        Section&& section,
        const ModelGenericMapping& initial_to_section_mappings ) noexcept
        : Section{ std::move( section ) }
    {
        CrossSectionBuilder builder{ *this };
        ModelCopyMapping mappings;
        detail::add_geology_clone_mapping( mappings, initial_model );
        builder.copy_geological_components( mappings, initial_model );
        if( initial_to_section_mappings.has_mapping_type(
                Line2D::component_type_static() ) )
        {
            const auto& line_mappings = initial_to_section_mappings.at(
                Line2D::component_type_static() );
            copy_faults_items_from_mappings(
                initial_model, *this, builder, line_mappings );
            copy_horizons_items_from_mappings(
                initial_model, *this, builder, line_mappings );
        }
        if( initial_to_section_mappings.has_mapping_type(
                Surface2D::component_type_static() ) )
        {
            const auto& surface_mappings = initial_to_section_mappings.at(
                Surface2D::component_type_static() );
            copy_fault_blocks_items_from_mappings(
                initial_model, *this, builder, surface_mappings );
            copy_stratigraphic_units_items_from_mappings(
                initial_model, *this, builder, surface_mappings );
        }
    }

    CrossSection CrossSection::clone() const
    {
        CrossSection model_clone{ Section::clone() };
        CrossSectionBuilder clone_builder{ model_clone };
        clone_builder.copy_identifier( *this );
        auto mappings = detail::section_clone_mapping( *this );
        detail::add_geology_clone_mapping( mappings, *this );
        clone_builder.copy_components( mappings, *this );
        clone_builder.copy_component_geometry( mappings, *this );
        clone_builder.copy_geological_components( mappings, *this );
        clone_builder.copy_relationships( mappings, *this );
        return model_clone;
    }
} // namespace geode