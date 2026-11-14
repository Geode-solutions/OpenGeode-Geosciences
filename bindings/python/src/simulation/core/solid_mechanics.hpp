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

#pragma once

#include <pybind11/native_enum.h>

#include <geode/geosciences/simulation/core/solid_mechanics.hpp>

namespace geode
{
    void define_solid_mechanics( pybind11::module& module )
    {
        pybind11::native_enum< SOLID_MECHANICS_LAW >( module,
            "SolidMechanicsLaw", "enum.Enum",
            "Constitutive law of "
            "the solid" )
            .value( "linear_elastic_isotropic",
                SOLID_MECHANICS_LAW::linear_elastic_isotropic )
            .finalize();
        pybind11::native_enum< STRESS_SIGN_CONVENTION >( module,
            "StressSignConvention", "enum.Enum",
            "Sign of the stresses and normal tractions stored in the model "
            "and of the computed stresses: compression_positive for "
            "geomechanics (pressures and overburden loads are positive), "
            "tension_positive for continuum mechanics, as in GEOS" )
            .value( "compression_positive",
                STRESS_SIGN_CONVENTION::compression_positive )
            .value(
                "tension_positive", STRESS_SIGN_CONVENTION::tension_positive )
            .finalize();
        pybind11::class_< SolidMechanics > mechanics_class( module,
            "SolidMechanics",
            "Set when a PhysicalProblem solves quasi-static solid mechanics. "
            "With a flow model, the problem is poromechanics. The elastic "
            "moduli and the density are the bulk_modulus, shear_modulus and "
            "density PhysicalProperties of the model" );
        mechanics_class
            .def( pybind11::init<>(),
                "Build the linear elastic isotropic law with the "
                "compression_positive convention" )
            .def( pybind11::init( []( SOLID_MECHANICS_LAW law,
                                      STRESS_SIGN_CONVENTION sign_convention ) {
                return SolidMechanics{ law, sign_convention };
            } ),
                pybind11::arg( "law" ),
                pybind11::arg_v( "sign_convention",
                    STRESS_SIGN_CONVENTION::compression_positive,
                    "StressSignConvention.compression_positive" ),
                "Build the solid mechanics from the SolidMechanicsLaw and the "
                "StressSignConvention" )
            .def(
                "__repr__",
                []( const SolidMechanics& mechanics ) {
                    return python_call_repr( "SolidMechanics",
                        { "law=" + python_repr( mechanics.law ),
                            "sign_convention="
                                + python_repr( mechanics.sign_convention ) } );
                },
                "Return an unambiguous description of the solid mechanics" );
        def_readwrite_named( mechanics_class, "law", &SolidMechanics::law,
            "SolidMechanicsLaw of the solid" );
        def_readwrite_named( mechanics_class, "sign_convention",
            &SolidMechanics::sign_convention,
            "StressSignConvention of the stresses" );
    }
} // namespace geode
