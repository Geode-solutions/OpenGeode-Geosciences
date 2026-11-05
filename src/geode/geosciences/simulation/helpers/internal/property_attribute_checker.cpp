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

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>
#include <absl/types/span.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/helpers/internal/simulation_checker_helpers.hpp>
#include <geode/geosciences/simulation/helpers/simulation_checker.hpp>
#include <geode/geosciences/simulation/helpers/simulation_report.hpp>

namespace
{
    using geode::PHYSICAL_PROPERTY_VALUE_TYPE;
    using geode::SIMULATION_CHECK_STAGE;
    using geode::SIMULATION_ISSUE_SEVERITY;
    using geode::internal::component_location;
    using geode::internal::ComponentElements;
    using geode::internal::count_string;
    using geode::internal::for_each_component;

    geode::SimulationIssueLocation elements_location(
        const ComponentElements& component,
        std::vector< geode::index_t > elements )
    {
        auto location = component_location( component.component );
        location.element_type = component.element_type;
        location.nb_elements = component.nb_elements;
        if( elements.size() == component.nb_elements )
        {
            location.all_elements = true;
        }
        else
        {
            location.elements = std::move( elements );
        }
        return location;
    }

    geode::SimulationIssueLocation all_elements_location(
        const ComponentElements& component )
    {
        auto location = component_location( component.component );
        location.element_type = component.element_type;
        location.nb_elements = component.nb_elements;
        location.all_elements = true;
        return location;
    }

    std::optional< PHYSICAL_PROPERTY_VALUE_TYPE > value_type(
        std::string_view type )
    {
        if( type == typeid( double ).name() )
        {
            return PHYSICAL_PROPERTY_VALUE_TYPE::scalar;
        }
        if( type == typeid( std::array< double, 3 > ).name() )
        {
            return PHYSICAL_PROPERTY_VALUE_TYPE::vector3;
        }
        return std::nullopt;
    }

    std::string_view value_type_string( PHYSICAL_PROPERTY_VALUE_TYPE type )
    {
        if( type == PHYSICAL_PROPERTY_VALUE_TYPE::scalar )
        {
            return "double";
        }
        return "std::array< double, 3 >";
    }

    class PropertyAttributeChecker
    {
    public:
        PropertyAttributeChecker( const geode::BRep& model,
            const geode::PhysicalPropertyRequirement& requirement,
            geode::SimulationReport& report )
            : model_( model ),
              requirement_( requirement ),
              report_( report ),
              coverage_( requirement.property ),
              name_( geode::physical_property_string( requirement.property ) ),
              component_type_( geode::physical_property_component_type(
                  requirement.property ) ),
              valid_range_( geode::internal::physical_property_description(
                  requirement.property )
                                .valid_range )
        {
        }

        geode::PhysicalPropertyCoverage run()
        {
            if( !model_.has_physical_property( requirement_.property ) )
            {
                if( !requirement_.required_by.empty() )
                {
                    geode::SimulationIssue issue{
                        SIMULATION_ISSUE_SEVERITY::error,
                        SIMULATION_CHECK_STAGE::properties,
                        std::string{ name_ },
                        absl::StrCat(
                            "missing, ", requirement_.required_by, " needs it" )
                    };
                    issue.fix = absl::StrCat( "add a ", name_,
                        " physical property on the model ",
                        component_type_.get(), "s" );
                    report_.add_issue( std::move( issue ) );
                }
                return std::move( coverage_ );
            }
            const auto& info =
                model_.physical_property_info( requirement_.property );
            if( info.component_type != component_type_ )
            {
                geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                    SIMULATION_CHECK_STAGE::properties, std::string{ name_ },
                    absl::StrCat( "registered on ", info.component_type.get(),
                        " components, simulators read it on ",
                        component_type_.get(), " components" ) };
                issue.fix = absl::StrCat(
                    "register the property on ", component_type_.get(), "s" );
                report_.add_issue( std::move( issue ) );
                return std::move( coverage_ );
            }
            coverage_.is_registered = true;
            attribute_id_ = info.attribute_id;
            for_each_component( model_, component_type_,
                [this]( const ComponentElements& elements ) {
                    check_component( elements );
                } );
            report_issues();
            return std::move( coverage_ );
        }

    private:
        [[nodiscard]] bool needs_attribute() const
        {
            return requirement_.merged || !requirement_.required_by.empty();
        }

        void check_absent_attribute( const ComponentElements& elements )
        {
            if( elements.vertices.attribute_exists( attribute_id_ ) )
            {
                on_vertices_.push_back(
                    component_location( elements.component ) );
            }
            else if( needs_attribute() && elements.nb_elements != 0 )
            {
                auto location = all_elements_location( elements );
                location.detail = "no attribute";
                missing_.push_back( std::move( location ) );
            }
        }

        void check_component( const ComponentElements& elements )
        {
            if( !elements.elements.attribute_exists( attribute_id_ ) )
            {
                check_absent_attribute( elements );
                return;
            }
            if( elements.nb_elements == 0 )
            {
                return;
            }
            const auto type =
                value_type( elements.elements.attribute_type( attribute_id_ ) );
            if( !type
                || !absl::c_linear_search(
                    requirement_.value_types, type.value() ) )
            {
                wrong_types_.push_back(
                    component_location( elements.component ) );
                return;
            }
            if( requirement_.merged )
            {
                check_merged( elements );
            }
            if( type == PHYSICAL_PROPERTY_VALUE_TYPE::scalar )
            {
                check_values< double >( elements );
            }
            else
            {
                check_values< std::array< double, 3 > >( elements );
            }
        }

        void check_merged( const ComponentElements& elements )
        {
            const auto generic =
                elements.elements.find_generic_attribute( attribute_id_ );
            if( !generic->properties().transferable )
            {
                not_transferable_.push_back(
                    component_location( elements.component ) );
            }
            if( !generic->name() || generic->name()->empty() )
            {
                unnamed_.push_back( component_location( elements.component ) );
            }
            const auto type = elements.elements.attribute_type( attribute_id_ );
            if( !merged_type_ )
            {
                merged_type_ = type;
            }
            else if( merged_type_.value() != type )
            {
                other_types_.push_back(
                    component_location( elements.component ) );
            }
        }

        template < typename Value >
        void check_values( const ComponentElements& elements )
        {
            const auto values =
                elements.elements.find_read_only_attribute< Value >(
                    attribute_id_ );
            // Missing elements are only listed when reported
            const auto list_missing = requirement_.value_on_every_element;
            geode::index_t nb_missing{ 0 };
            std::vector< geode::index_t > missing;
            std::vector< geode::index_t > invalid;
            std::optional< double > first_invalid;
            for( const auto element : geode::Range{ elements.nb_elements } )
            {
                if( !values->has_value( element ) )
                {
                    nb_missing++;
                    if( list_missing )
                    {
                        missing.push_back( element );
                    }
                    continue;
                }
                if( !check_element_value(
                        values->value( element ), first_invalid ) )
                {
                    invalid.push_back( element );
                }
            }
            if( !missing.empty() )
            {
                missing_.push_back(
                    elements_location( elements, std::move( missing ) ) );
            }
            if( !invalid.empty() )
            {
                auto location =
                    elements_location( elements, std::move( invalid ) );
                location.detail =
                    absl::StrCat( "first value ", first_invalid.value() );
                invalid_.push_back( std::move( location ) );
            }
            if( nb_missing < elements.nb_elements )
            {
                coverage_.components.emplace(
                    elements.component.id(), nb_missing );
            }
        }

        template < typename Value >
        bool check_element_value(
            const Value& value, std::optional< double >& first_invalid )
        {
            if constexpr( std::is_same_v< Value, double > )
            {
                return check_value( value, first_invalid );
            }
            else
            {
                bool is_valid{ true };
                for( const auto item : value )
                {
                    is_valid = check_value( item, first_invalid ) && is_valid;
                }
                return is_valid;
            }
        }

        // Updates the value bounds with valid values
        bool check_value( double value, std::optional< double >& first_invalid )
        {
            if( !valid_range_.contains( value ) )
            {
                if( !first_invalid )
                {
                    first_invalid = value;
                }
                return false;
            }
            if( std::isnan( value ) )
            {
                return true;
            }
            if( !coverage_.value_bounds )
            {
                coverage_.value_bounds =
                    std::array< double, 2 >{ value, value };
                return true;
            }
            auto& bounds = coverage_.value_bounds.value();
            bounds[0] = std::min( bounds[0], value );
            bounds[1] = std::max( bounds[1], value );
            return true;
        }

        void add_issue( std::string_view message,
            std::vector< geode::SimulationIssueLocation >&& locations,
            std::string fix = {} )
        {
            if( locations.empty() )
            {
                return;
            }
            geode::SimulationIssue issue{ SIMULATION_ISSUE_SEVERITY::error,
                SIMULATION_CHECK_STAGE::properties, std::string{ name_ },
                std::string{ message } };
            issue.locations = std::move( locations );
            issue.fix = std::move( fix );
            report_.add_issue( std::move( issue ) );
        }

        // "<what> on 2 Blocks"
        [[nodiscard]] std::string on_components( std::string_view what,
            absl::Span< const geode::SimulationIssueLocation > locations,
            std::string_view consequence = {} ) const
        {
            return absl::StrCat( what, " on ",
                count_string( locations.size(), component_type_.get() ),
                consequence );
        }

        void report_issues()
        {
            auto message = on_components( "stored on vertices", on_vertices_,
                absl::StrCat( ", simulators read it on ", component_type_.get(),
                    " elements" ) );
            add_issue( message, std::move( on_vertices_ ),
                "store the attribute on the mesh elements" );
            std::vector< std::string_view > expected;
            for( const auto type : requirement_.value_types )
            {
                expected.push_back( value_type_string( type ) );
            }
            message = on_components(
                absl::StrCat( "must store ", absl::StrJoin( expected, " or " ),
                    " values" ),
                wrong_types_ );
            add_issue( message, std::move( wrong_types_ ) );
            message = on_components( "not transferable", not_transferable_,
                ": it cannot be merged into the simulation mesh" );
            add_issue( message, std::move( not_transferable_ ),
                "make the attribute transferable" );
            message = on_components( "unnamed attribute", unnamed_,
                ": the simulation mesh needs a name" );
            add_issue( message, std::move( unnamed_ ), "name the attribute" );
            message = on_components( "attribute value type differs",
                other_types_, ": the merged attribute needs one type" );
            add_issue( message, std::move( other_types_ ) );
            message = on_components( "missing", missing_ );
            add_issue( message, std::move( missing_ ),
                requirement_.required_by.empty()
                    ? std::string{}
                    : absl::StrCat( requirement_.required_by, " needs ", name_,
                        " on every ", component_type_.get() ) );
            message = on_components( valid_range_.invalid_values, invalid_ );
            add_issue( message, std::move( invalid_ ) );
        }

    private:
        const geode::BRep& model_;
        const geode::PhysicalPropertyRequirement& requirement_;
        geode::SimulationReport& report_;
        geode::PhysicalPropertyCoverage coverage_;
        std::string_view name_;
        geode::ComponentType component_type_;
        geode::internal::ValidRange valid_range_;
        geode::uuid attribute_id_;
        std::optional< std::string > merged_type_;
        std::vector< geode::SimulationIssueLocation > on_vertices_;
        std::vector< geode::SimulationIssueLocation > wrong_types_;
        std::vector< geode::SimulationIssueLocation > not_transferable_;
        std::vector< geode::SimulationIssueLocation > unnamed_;
        std::vector< geode::SimulationIssueLocation > other_types_;
        std::vector< geode::SimulationIssueLocation > missing_;
        std::vector< geode::SimulationIssueLocation > invalid_;
    };
} // namespace

namespace geode
{
    PhysicalPropertyCoverage SimulationChecker::check_physical_property(
        const PhysicalPropertyRequirement& requirement )
    {
        return PropertyAttributeChecker{ model(), requirement, report() }.run();
    }
} // namespace geode
