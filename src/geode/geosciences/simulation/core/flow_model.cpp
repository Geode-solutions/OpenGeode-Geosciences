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

#include <geode/geosciences/simulation/core/flow_model.hpp>

#include <bitsery/ext/std_smart_ptr.h>
#include <bitsery/traits/array.h>
#include <bitsery/traits/string.h>
#include <bitsery/traits/vector.h>

#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/growable.hpp>

#include <geode/geosciences/simulation/core/fluid.hpp>

namespace geode
{
    FlowModel::FlowModel() = default;
    FlowModel::FlowModel( const FlowModel& ) = default;
    FlowModel::FlowModel( FlowModel&& ) noexcept = default;
    FlowModel& FlowModel::operator=( const FlowModel& ) = default;
    FlowModel& FlowModel::operator=( FlowModel&& ) noexcept = default;
    FlowModel::~FlowModel() = default;

    template < typename Archive >
    void FlowModel::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, FlowModel >{ { []( Archive& local_archive,
                                                         FlowModel& flow ) {
                local_archive.ext( flow.fluid, bitsery::ext::StdSmartPtr{} );
            } } } );
    }

    template < typename Archive >
    void SinglePhaseFlow::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, SinglePhaseFlow >{
                       { []( Archive& local_archive, SinglePhaseFlow& flow ) {
                           local_archive.ext(
                               flow, bitsery::ext::BaseClass< FlowModel >{} );
                       } } } );
    }

    template < typename Archive >
    void MultiphaseFlow::serialize( Archive& archive )
    {
        archive.ext(
            *this, Growable< Archive, MultiphaseFlow >{
                       { []( Archive& local_archive, MultiphaseFlow& flow ) {
                           local_archive.ext(
                               flow, bitsery::ext::BaseClass< FlowModel >{} );
                           local_archive.value4b( flow.basis );
                       } } } );
    }

    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, FlowModel );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, SinglePhaseFlow );
    SERIALIZE_BITSERY_ARCHIVE(
        opengeode_geosciences_simulation_api, MultiphaseFlow );
} // namespace geode
