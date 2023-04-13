/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "geometry/stratigraphic_point.h"
#include "mixin/builder/stratigraphic_relationships_builder.h"
#include "mixin/core/stratigraphic_relationships.h"
#include "representation/builder/implicit_cross_section_builder.h"
#include "representation/builder/implicit_structural_model_builder.h"
#include "representation/builder/stratigraphic_model_builder.h"
#include "representation/builder/stratigraphic_section_builder.h"
#include "representation/builder/stratigraphic_units_stack_builder.h"
#include "representation/core/helpers.h"
#include "representation/core/implicit_cross_section.h"
#include "representation/core/implicit_structural_model.h"
#include "representation/core/stratigraphic_model.h"
#include "representation/core/stratigraphic_section.h"
#include "representation/core/stratigraphic_units_stack.h"
#include "representation/io/implicit_cross_section.h"
#include "representation/io/implicit_structural_model.h"
#include "representation/io/stratigraphic_model.h"
#include "representation/io/stratigraphic_section.h"
#include "representation/io/stratigraphic_units_stack.h"

namespace pybind11
{
    namespace detail
    {
        template <>
        struct type_caster< absl::string_view >
            : string_caster< absl::string_view, true >
        {
        };

        template < typename Type, size_t dimension >
        struct type_caster< absl::InlinedVector< Type, dimension > >
            : list_caster< absl::InlinedVector< Type, dimension >, Type >
        {
        };

        template < typename T >
        struct type_caster< absl::optional< T > >
            : public optional_caster< absl::optional< T > >
        {
        };
    } // namespace detail
} // namespace pybind11

PYBIND11_MODULE( opengeode_geosciences_py_implicit, module )
{
    module.doc() = "OpenGeode-Geosciences Python binding for implicit";
    pybind11::module::import( "opengeode" );
    pybind11::class_< geode::GeosciencesImplicitLibrary >(
        module, "GeosciencesImplicitLibrary" )
        .def( "initialize", &geode::GeosciencesImplicitLibrary::initialize );
    geode::define_stratigraphic_point( module );

    geode::define_stratigraphic_relationships( module );
    geode::define_stratigraphic_relationships_builder( module );

    geode::detail::define_implicit_model_helpers( module );
    geode::define_implicit_cross_section( module );
    geode::define_implicit_structural_model( module );
    geode::define_stratigraphic_model( module );
    geode::define_stratigraphic_section( module );
    geode::define_stratigraphic_units_stack( module );
    geode::define_implicit_cross_section_builder( module );
    geode::define_implicit_structural_model_builder( module );
    geode::define_stratigraphic_model_builder( module );
    geode::define_stratigraphic_section_builder( module );
    geode::define_stratigraphic_units_stack_builder( module );

    geode::define_implicit_cross_section_io( module );
    geode::define_implicit_structural_model_io( module );
    geode::define_stratigraphic_model_io( module );
    geode::define_stratigraphic_section_io( module );
    geode::define_stratigraphic_units_stack_io( module );
}