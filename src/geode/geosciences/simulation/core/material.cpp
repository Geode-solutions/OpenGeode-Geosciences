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

#include <geode/geosciences/simulation/core/material.hpp>

#include <bitsery/ext/std_optional.h>
#include <bitsery/ext/std_smart_ptr.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

#include <geode/geosciences/simulation/core/saturation_functions.hpp>

namespace geode
{
    template < typename Archive >
    void RockThermalProperties::serialize( Archive& archive )
    {
        archive.ext( *this,
            Growable< Archive, RockThermalProperties >{
                { []( Archive& local_archive, RockThermalProperties& thermal ) {
                    local_archive.value8b( thermal.volumetric_heat_capacity );
                    local_archive.container8b( thermal.thermal_conductivity );
                } } } );
    }

    template < typename Archive >
    void Material::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, Material >{ { []( Archive& local_archive,
                                                        Material& material ) {
                local_archive.text1b( material.name, material.name.max_size() );
                local_archive.container( material.regions,
                    material.regions.max_size(),
                    []( Archive& region_archive, uuid& region ) {
                        region_archive.object( region );
                    } );
                local_archive.value8b( material.pore_compressibility );
                local_archive.value8b( material.reference_pressure );
                local_archive.ext( material.porosity,
                    bitsery::ext::StdOptional{},
                    []( Archive& value_archive, double& porosity_value ) {
                        value_archive.value8b( porosity_value );
                    } );
                local_archive.ext( material.permeability,
                    bitsery::ext::StdOptional{},
                    []( Archive& value_archive,
                        std::array< double, 3 >& permeability_values ) {
                        value_archive.container8b( permeability_values );
                    } );
                local_archive.ext( material.relative_permeability,
                    bitsery::ext::StdSmartPtr{} );
                local_archive.ext(
                    material.capillary_pressure, bitsery::ext::StdSmartPtr{} );
                local_archive.ext( material.thermal,
                    bitsery::ext::StdOptional{},
                    []( Archive& thermal_archive,
                        RockThermalProperties& thermal_properties ) {
                        thermal_archive.object( thermal_properties );
                    } );
            } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE( opengeode_geosciences_simulation_api, Material );
} // namespace geode
