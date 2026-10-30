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

#include <geode/geosciences/simulation/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    enum struct SOLID_MECHANICS_LAW
    {
        linear_elastic_isotropic
    };

    /*!
     * Sign of the stresses and normal tractions stored in the model
     * (initial_stress, initial_shear_stress, boundary_normal_traction) and
     * of the computed stresses.
     */
    enum struct STRESS_SIGN_CONVENTION
    {
        // Geomechanics: pressures and overburden loads are positive
        compression_positive,
        // Continuum mechanics, as in GEOS
        tension_positive
    };

    /*!
     * Set when the problem solves quasi-static solid mechanics.
     * Elastic moduli and density are the bulk_modulus, shear_modulus and
     * density PhysicalProperties of the model.
     */
    struct opengeode_geosciences_simulation_api SolidMechanics
    {
        SOLID_MECHANICS_LAW law{
            SOLID_MECHANICS_LAW::linear_elastic_isotropic
        };
        STRESS_SIGN_CONVENTION sign_convention{
            STRESS_SIGN_CONVENTION::compression_positive
        };

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
} // namespace geode
