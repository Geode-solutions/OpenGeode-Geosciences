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

#include <geode/geosciences/simulation/helpers/simulation_model_checks.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <typeinfo>
#include <vector>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/range.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/geosciences/simulation/core/material.hpp>
#include <geode/geosciences/simulation/core/physical_problem.hpp>
#include <geode/geosciences/simulation/helpers/physical_problem_blocks.hpp>
#include <geode/geosciences/simulation/helpers/physical_problem_simulation.hpp>

namespace
{
    using geode::PHYSICAL_PROPERTY_NAME;
    using geode::PHYSICAL_PROPERTY_VALUE_TYPE;

    struct ElementCount
    {
        void add( geode::index_t element )
        {
            if( count == 0 )
            {
                first = element;
            }
            count++;
        }

        geode::index_t count{ 0 };
        geode::index_t first{ geode::NO_ID };
    };

    // Mesh elements of a component carrying a physical property
    struct ComponentElements
    {
        const geode::Component3D& component;
        const geode::AttributeManager& elements;
        const geode::AttributeManager& vertices;
        geode::index_t nb_elements;
        std::string_view element_name;
    };

    template < typename Action >
    void for_each_component( const geode::BRep& model,
        const geode::ComponentType& type,
        const Action& action )
    {
        if( type == geode::Block3D::component_type_static() )
        {
            for( const auto& block : model.blocks() )
            {
                const auto& mesh = block.mesh();
                action( ComponentElements{ block,
                    mesh.polyhedron_attribute_manager(),
                    mesh.vertex_attribute_manager(), mesh.nb_polyhedra(),
                    "polyhedra" } );
            }
            return;
        }
        for( const auto& surface : model.surfaces() )
        {
            const auto& mesh = surface.mesh();
            action(
                ComponentElements{ surface, mesh.polygon_attribute_manager(),
                    mesh.vertex_attribute_manager(), mesh.nb_polygons(),
                    "polygons" } );
        }
    }

    std::vector< bool > block_polyhedron_unique_vertices(
        const geode::BRep& model )
    {
        std::vector< bool > on_block( model.nb_unique_vertices(), false );
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            for( const auto polyhedron : geode::Range{ mesh.nb_polyhedra() } )
            {
                for( const auto vertex :
                    geode::LRange{ mesh.nb_polyhedron_vertices( polyhedron ) } )
                {
                    const auto unique_vertex =
                        model.unique_vertex( { block.component_id(),
                            mesh.polyhedron_vertex(
                                { polyhedron, vertex } ) } );
                    if( unique_vertex != geode::NO_ID )
                    {
                        on_block[unique_vertex] = true;
                    }
                }
            }
        }
        return on_block;
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

    bool is_valid_value( PHYSICAL_PROPERTY_NAME property, double value )
    {
        if( !std::isfinite( value ) )
        {
            return false;
        }
        switch( property )
        {
            case PHYSICAL_PROPERTY_NAME::porosity:
                return value > 0 && value <= 1;
            case PHYSICAL_PROPERTY_NAME::permeability:
                return value >= 0;
            case PHYSICAL_PROPERTY_NAME::boundary_temperature:
                return value > 0;
            case PHYSICAL_PROPERTY_NAME::boundary_pressure:
                return true;
            default:
                return value >= 0 && value <= 1;
        }
    }

    std::string_view valid_range_string( PHYSICAL_PROPERTY_NAME property )
    {
        switch( property )
        {
            case PHYSICAL_PROPERTY_NAME::porosity:
                return "(0, 1]";
            case PHYSICAL_PROPERTY_NAME::permeability:
                return "[0, +inf[";
            case PHYSICAL_PROPERTY_NAME::boundary_temperature:
                return "]0, +inf[";
            case PHYSICAL_PROPERTY_NAME::boundary_pressure:
                return "finite values";
            default:
                return "[0, 1]";
        }
    }

    class PhysicalPropertyChecker
    {
    public:
        PhysicalPropertyChecker( const geode::BRep& model,
            const geode::PhysicalPropertyRequirement& requirement,
            geode::SimulationReport& report )
            : model_( model ),
              requirement_( requirement ),
              report_( report ),
              coverage_( requirement.property ),
              name_( geode::physical_property_string( requirement.property ) )
        {
        }

        geode::PhysicalPropertyCoverage run()
        {
            if( !model_.has_physical_property( requirement_.property ) )
            {
                return std::move( coverage_ );
            }
            const auto& info =
                model_.physical_property_info( requirement_.property );
            const auto expected = geode::physical_property_component_type(
                requirement_.property );
            if( info.component_type != expected )
            {
                report_.add_error( "PhysicalProperty ", name_,
                    " is registered on ", info.component_type.get(),
                    " components, simulators read it on ", expected.get(),
                    " components" );
                return std::move( coverage_ );
            }
            coverage_.is_registered = true;
            attribute_id_ = info.attribute_id;
            for_each_component(
                model_, expected, [this]( const ComponentElements& elements ) {
                    check_component( elements );
                } );
            return std::move( coverage_ );
        }

    private:
        std::string attribute_description(
            const geode::AttributeManager& manager ) const
        {
            const auto attribute =
                manager.find_generic_attribute( attribute_id_ );
            if( attribute && attribute->name() && !attribute->name()->empty() )
            {
                return absl::StrCat( name_, " attribute \"",
                    attribute->name().value(), "\" (", attribute_id_.string(),
                    ")" );
            }
            return absl::StrCat(
                name_, " attribute (", attribute_id_.string(), ")" );
        }

        void check_component( const ComponentElements& elements )
        {
            const auto description =
                geode::simulation_component_description( elements.component );
            if( !elements.elements.attribute_exists( attribute_id_ ) )
            {
                if( elements.vertices.attribute_exists( attribute_id_ ) )
                {
                    report_.add_error( description, " stores ",
                        attribute_description( elements.vertices ),
                        " on vertices, simulators read it on ",
                        elements.element_name );
                }
                else if( requirement_.merged && elements.nb_elements != 0 )
                {
                    report_.add_error( description, " has no ", name_,
                        " attribute (", attribute_id_.string(),
                        "): the attribute must be on every ",
                        elements.component.component_type().get() );
                }
                return;
            }
            if( elements.nb_elements == 0 )
            {
                return;
            }
            const auto attribute = attribute_description( elements.elements );
            const auto type =
                value_type( elements.elements.attribute_type( attribute_id_ ) );
            if( !type
                || !absl::c_linear_search(
                    requirement_.value_types, type.value() ) )
            {
                std::vector< std::string_view > expected;
                for( const auto value_type : requirement_.value_types )
                {
                    expected.push_back( value_type_string( value_type ) );
                }
                report_.add_error( description, " ", attribute, " must store ",
                    absl::StrJoin( expected, " or " ), " values" );
                return;
            }
            if( requirement_.merged )
            {
                check_merged( elements, description, attribute );
            }
            if( type == PHYSICAL_PROPERTY_VALUE_TYPE::scalar )
            {
                check_values< double >( elements, description, attribute );
            }
            else
            {
                check_values< std::array< double, 3 > >(
                    elements, description, attribute );
            }
        }

        void check_merged( const ComponentElements& elements,
            std::string_view description,
            std::string_view attribute )
        {
            const auto generic =
                elements.elements.find_generic_attribute( attribute_id_ );
            if( !generic->properties().transferable )
            {
                report_.add_error( description, " ", attribute,
                    " is not transferable: it cannot be merged into the "
                    "simulation mesh" );
            }
            if( !generic->name() || generic->name()->empty() )
            {
                report_.add_error( description, " ", attribute,
                    " has no name: the simulation mesh needs one" );
            }
            const auto type = elements.elements.attribute_type( attribute_id_ );
            if( !merged_type_ )
            {
                merged_type_ = type;
                merged_type_component_ = std::string{ description };
            }
            else if( merged_type_.value() != type )
            {
                report_.add_error( description, " ", attribute,
                    " has another value type than on ",
                    merged_type_component_ );
            }
        }

        template < typename Value >
        void check_values( const ComponentElements& elements,
            std::string_view description,
            std::string_view attribute )
        {
            const auto values =
                elements.elements.find_read_only_attribute< Value >(
                    attribute_id_ );
            ElementCount missing;
            ElementCount invalid;
            double invalid_value{ 0 };
            for( const auto element : geode::Range{ elements.nb_elements } )
            {
                if( !values->has_value( element ) )
                {
                    missing.add( element );
                    continue;
                }
                const auto& value = values->value( element );
                if constexpr( std::is_same_v< Value, double > )
                {
                    check_value( value, element, invalid, invalid_value );
                }
                else
                {
                    for( const auto item : value )
                    {
                        check_value( item, element, invalid, invalid_value );
                    }
                }
            }
            if( missing.count != 0 && requirement_.value_on_every_element )
            {
                report_.add_error( description, " ", attribute,
                    " has no value on ", missing.count, " of ",
                    elements.nb_elements, " ", elements.element_name,
                    " (first: ", missing.first, ")" );
            }
            if( invalid.count != 0 )
            {
                report_.add_error( description, " ", attribute, " has ",
                    invalid.count, " values outside ",
                    valid_range_string( requirement_.property ),
                    " (first: ", elements.element_name, " ", invalid.first,
                    ", value ", invalid_value, ")" );
            }
            if( missing.count < elements.nb_elements )
            {
                coverage_.components.emplace(
                    elements.component.id(), missing.count );
            }
        }

        void check_value( double value,
            geode::index_t element,
            ElementCount& invalid,
            double& invalid_value )
        {
            if( !is_valid_value( requirement_.property, value ) )
            {
                if( invalid.count == 0 )
                {
                    invalid_value = value;
                }
                invalid.add( element );
                return;
            }
            if( !coverage_.value_bounds )
            {
                coverage_.value_bounds =
                    std::array< double, 2 >{ value, value };
                return;
            }
            auto& bounds = coverage_.value_bounds.value();
            bounds[0] = std::min( bounds[0], value );
            bounds[1] = std::max( bounds[1], value );
        }

    private:
        const geode::BRep& model_;
        const geode::PhysicalPropertyRequirement& requirement_;
        geode::SimulationReport& report_;
        geode::PhysicalPropertyCoverage coverage_;
        std::string_view name_;
        geode::uuid attribute_id_;
        std::optional< std::string > merged_type_;
        std::string merged_type_component_;
    };

    bool material_has_value(
        const geode::Material& material, PHYSICAL_PROPERTY_NAME property )
    {
        if( property == PHYSICAL_PROPERTY_NAME::porosity )
        {
            return material.porosity.has_value();
        }
        return material.permeability.has_value();
    }
} // namespace

namespace geode
{
    void check_simulation_blocks( const BRep& model, SimulationReport& report )
    {
        if( model.nb_blocks() == 0 )
        {
            report.add_error( "The model has no Block" );
            return;
        }
        for( const auto& block : model.blocks() )
        {
            const auto& mesh = block.mesh();
            if( mesh.nb_polyhedra() == 0 )
            {
                report.add_error( simulation_component_description( block ),
                    " has no polyhedron: mesh it or remove it from the model" );
                continue;
            }
            std::vector< bool > visited( mesh.nb_vertices(), false );
            ElementCount missing;
            for( const auto polyhedron : Range{ mesh.nb_polyhedra() } )
            {
                for( const auto polyhedron_vertex :
                    LRange{ mesh.nb_polyhedron_vertices( polyhedron ) } )
                {
                    const auto vertex = mesh.polyhedron_vertex(
                        { polyhedron, polyhedron_vertex } );
                    if( visited[vertex] )
                    {
                        continue;
                    }
                    visited[vertex] = true;
                    if( model.unique_vertex( { block.component_id(), vertex } )
                        == NO_ID )
                    {
                        missing.add( vertex );
                    }
                }
            }
            if( missing.count != 0 )
            {
                report.add_error( simulation_component_description( block ),
                    " has ", missing.count,
                    " polyhedron vertices without unique vertex (first: "
                    "vertex ",
                    missing.first, ")" );
            }
        }
    }

    void check_simulation_surfaces_on_blocks( const BRep& model,
        absl::Span< const uuid > surfaces,
        SimulationReport& report )
    {
        if( surfaces.empty() )
        {
            return;
        }
        const auto on_block = block_polyhedron_unique_vertices( model );
        for( const auto& surface_id : surfaces )
        {
            const auto& surface = model.surface( surface_id );
            const auto& mesh = surface.mesh();
            std::vector< bool > visited( mesh.nb_vertices(), false );
            ElementCount outside;
            for( const auto polygon : Range{ mesh.nb_polygons() } )
            {
                for( const auto polygon_vertex :
                    LRange{ mesh.nb_polygon_vertices( polygon ) } )
                {
                    const auto vertex =
                        mesh.polygon_vertex( { polygon, polygon_vertex } );
                    if( visited[vertex] )
                    {
                        continue;
                    }
                    visited[vertex] = true;
                    const auto unique_vertex = model.unique_vertex(
                        { surface.component_id(), vertex } );
                    if( unique_vertex == NO_ID || !on_block[unique_vertex] )
                    {
                        outside.add( vertex );
                    }
                }
            }
            if( outside.count != 0 )
            {
                report.add_error( simulation_component_description( surface ),
                    " has ", outside.count,
                    " polygon vertices which are not Block polyhedron "
                    "vertices (first: vertex ",
                    outside.first,
                    "): the Surface mesh must match the Block meshes" );
            }
        }
    }

    void check_simulation_unique_vertices_in_blocks(
        const BRep& model, SimulationReport& report )
    {
        ElementCount outside;
        for( const auto unique_vertex : Range{ model.nb_unique_vertices() } )
        {
            if( !model.has_component_mesh_vertices(
                    unique_vertex, Block3D::component_type_static() ) )
            {
                outside.add( unique_vertex );
            }
        }
        if( outside.count != 0 )
        {
            report.add_error( outside.count,
                " unique vertices are not Block vertices (first: unique "
                "vertex ",
                outside.first,
                "): every unique vertex, including those of Corners, Lines "
                "and Surfaces, must be a Block vertex" );
        }
    }

    std::string_view physical_property_string( PHYSICAL_PROPERTY_NAME property )
    {
        switch( property )
        {
            case PHYSICAL_PROPERTY_NAME::porosity:
                return "porosity";
            case PHYSICAL_PROPERTY_NAME::permeability:
                return "permeability";
            case PHYSICAL_PROPERTY_NAME::boundary_pressure:
                return "boundary_pressure";
            case PHYSICAL_PROPERTY_NAME::boundary_temperature:
                return "boundary_temperature";
            case PHYSICAL_PROPERTY_NAME::boundary_oil_fraction:
                return "boundary_oil_fraction";
            case PHYSICAL_PROPERTY_NAME::boundary_gas_fraction:
                return "boundary_gas_fraction";
            case PHYSICAL_PROPERTY_NAME::boundary_water_fraction:
                return "boundary_water_fraction";
        }
        return "unknown";
    }

    ComponentType physical_property_component_type(
        PHYSICAL_PROPERTY_NAME property )
    {
        if( property == PHYSICAL_PROPERTY_NAME::porosity
            || property == PHYSICAL_PROPERTY_NAME::permeability )
        {
            return Block3D::component_type_static();
        }
        return Surface3D::component_type_static();
    }

    PhysicalPropertyRequirement::PhysicalPropertyRequirement(
        PHYSICAL_PROPERTY_NAME property_in )
        : property( property_in ),
          value_types{ PHYSICAL_PROPERTY_VALUE_TYPE::scalar }
    {
        if( property == PHYSICAL_PROPERTY_NAME::permeability )
        {
            value_types.push_back( PHYSICAL_PROPERTY_VALUE_TYPE::vector3 );
        }
    }

    PhysicalPropertyCoverage::PhysicalPropertyCoverage(
        PHYSICAL_PROPERTY_NAME property_in )
        : property( property_in )
    {
    }

    bool PhysicalPropertyCoverage::carries( const uuid& component ) const
    {
        return components.contains( component );
    }

    bool PhysicalPropertyCoverage::covers( const uuid& component ) const
    {
        const auto carrier = components.find( component );
        return carrier != components.end() && carrier->second == 0;
    }

    bool PhysicalPropertyCoverage::is_carried() const
    {
        return !components.empty();
    }

    PhysicalPropertyCoverage check_physical_property( const BRep& model,
        const PhysicalPropertyRequirement& requirement,
        SimulationReport& report )
    {
        return PhysicalPropertyChecker{ model, requirement, report }.run();
    }

    void check_rock_property_availability( const PhysicalProblem& problem,
        const BRep& model,
        const PhysicalPropertyCoverage& coverage,
        SimulationReport& report )
    {
        const auto materials = problem.materials();
        const auto block_materials =
            physical_problem_block_materials( problem, model );
        std::vector< std::vector< const Block3D* > > uncovered_blocks(
            materials.size() );
        for( const auto& block : model.blocks() )
        {
            if( block.mesh().nb_polyhedra() == 0
                || coverage.covers( block.id() ) )
            {
                continue;
            }
            const auto material = block_materials.find( block.id() );
            if( material == block_materials.end()
                || material_has_value(
                    materials[material->second], coverage.property ) )
            {
                continue;
            }
            uncovered_blocks[material->second].push_back( &block );
        }
        const auto name = physical_property_string( coverage.property );
        for( const auto material : Indices{ materials } )
        {
            const auto& blocks = uncovered_blocks[material];
            if( blocks.empty() )
            {
                continue;
            }
            if( !coverage.is_registered )
            {
                report.add_error( "Material ", materials[material].name,
                    " needs a ", name, ": the model has no ", name,
                    " PhysicalProperty on Blocks" );
                continue;
            }
            report.add_error( "Material ", materials[material].name,
                " needs a ", name, ": the model ", name,
                " does not cover every polyhedron of ",
                simulation_component_description( *blocks.front() ),
                blocks.size() > 1 ? absl::StrCat( " and ", blocks.size() - 1,
                                        " other Blocks of this material" )
                                  : "" );
        }
    }

    void check_coinciding_physical_properties( const BRep& model,
        absl::Span< const PhysicalPropertyCoverage > coverages,
        SimulationReport& report )
    {
        if( coverages.size() < 2 )
        {
            return;
        }
        const auto type =
            physical_property_component_type( coverages.front().property );
        for_each_component( model, type,
            [&coverages, &report]( const ComponentElements& elements ) {
                std::vector< std::string_view > carried;
                std::vector< std::string_view > missing;
                for( const auto& coverage : coverages )
                {
                    const auto name =
                        physical_property_string( coverage.property );
                    if( coverage.carries( elements.component.id() ) )
                    {
                        carried.push_back( name );
                    }
                    else
                    {
                        missing.push_back( name );
                    }
                }
                if( carried.empty() || missing.empty() )
                {
                    return;
                }
                report.add_error(
                    simulation_component_description( elements.component ),
                    " carries ", absl::StrJoin( carried, ", " ), " but not ",
                    absl::StrJoin( missing, ", " ),
                    ": these properties must be given together" );
            } );
    }

    void check_physical_property_presence( const BRep& model,
        const PhysicalPropertyCoverage& coverage,
        std::string_view consequence,
        SimulationReport& report )
    {
        if( coverage.is_carried() )
        {
            return;
        }
        const auto name = physical_property_string( coverage.property );
        if( !model.has_physical_property( coverage.property ) )
        {
            report.add_warning(
                "The model has no ", name, " PhysicalProperty: ", consequence );
        }
        else if( coverage.is_registered )
        {
            report.add_warning( "No ",
                physical_property_component_type( coverage.property ).get(),
                " carries a ", name, " value: ", consequence );
        }
    }

    void warn_ignored_physical_properties( const BRep& model,
        absl::Span< const PHYSICAL_PROPERTY_NAME > properties,
        std::string_view simulator,
        SimulationReport& report )
    {
        for( const auto property : properties )
        {
            if( model.has_physical_property( property ) )
            {
                report.add_warning( "PhysicalProperty ",
                    physical_property_string( property ), " is ignored by ",
                    simulator );
            }
        }
    }
} // namespace geode
