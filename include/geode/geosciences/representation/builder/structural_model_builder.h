/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/model/mixin/builder/add_components_builders.h>
#include <geode/model/representation/builder/brep_builder.h>

#include <geode/geosciences/mixin/builder/faults_builder.h>
#include <geode/geosciences/mixin/builder/horizons_builder.h>
#include <geode/geosciences/mixin/core/fault.h>
#include <geode/geosciences/mixin/core/faults.h>
#include <geode/geosciences/mixin/core/horizon.h>
#include <geode/geosciences/mixin/core/horizons.h>

namespace geode
{
    class StructuralModel;
} // namespace geode

namespace geode
{
    /*!
     * Class managing modifications of a StructuralModel
     */
    class opengeode_geosciences_geosciences_api StructuralModelBuilder
        : public BRepBuilder,
          public AddComponentsBuilders< 3, Faults, Horizons >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( StructuralModelBuilder );

    public:
        StructuralModelBuilder( StructuralModel& structural_model );

        const uuid& add_fault();

        const uuid& add_fault( typename Fault3D::FAULT_TYPE type );

        void add_surface_in_fault(
            const Surface3D& surface, const Fault3D& fault );

        void remove_fault( const Fault3D& fault );

        const uuid& add_horizon();

        const uuid& add_horizon( typename Horizon3D::HORIZON_TYPE type );

        void add_surface_in_horizon(
            const Surface3D& surface, const Horizon3D& horizon );

        void remove_horizon( const Horizon3D& horizon );
    };
} // namespace geode
