/*
 * Copyright (c) 2019 - 2023 Geode-solutions. All rights reserved.
 */

#include <geode/basic/factory.h>

#define PYTHON_FACTORY_CLASS( type )                                           \
    pybind11::class_< type >( module, #type )                                  \
        .def( "list_creators", &type::list_creators )                          \
        .def( "has_creator", &type::has_creator )
