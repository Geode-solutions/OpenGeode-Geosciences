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

#include <geode/geosciences/simulation/core/bitsery_archive.hpp>

#include <geode/geosciences/simulation/core/flow_model.hpp>
#include <geode/geosciences/simulation/core/fluid.hpp>
#include <geode/geosciences/simulation/core/initial_condition.hpp>
#include <geode/geosciences/simulation/core/saturation_functions.hpp>

namespace
{
    template < typename Serializer >
    void register_simulation_pcontext( geode::PContext& context )
    {
        geode::register_simulation_leaf< Serializer, geode::FluidModel,
            geode::SinglePhaseFluid >( context );
        geode::register_simulation_leaf< Serializer, geode::FluidModel,
            geode::DeadOilFluid >( context );
        geode::register_simulation_leaf< Serializer, geode::FluidModel,
            geode::Co2BrineFluid >( context );
        geode::register_simulation_leaf< Serializer, geode::FlowModel,
            geode::SinglePhaseFlow >( context );
        geode::register_simulation_leaf< Serializer, geode::FlowModel,
            geode::MultiphaseFlow >( context );
        geode::register_simulation_leaf< Serializer,
            geode::RelativePermeability,
            geode::BrooksCoreyRelativePermeability >( context );
        geode::register_simulation_leaf< Serializer,
            geode::RelativePermeability, geode::TableRelativePermeability >(
            context );
        geode::register_simulation_leaf< Serializer, geode::CapillaryPressure,
            geode::BrooksCoreyCapillaryPressure >( context );
        geode::register_simulation_leaf< Serializer, geode::CapillaryPressure,
            geode::TableCapillaryPressure >( context );
        geode::register_simulation_leaf< Serializer, geode::InitialCondition,
            geode::UniformInitialCondition >( context );
    }
} // namespace

namespace geode
{
    void register_simulation_serialize_pcontext( PContext& context )
    {
        register_simulation_pcontext< Serializer >( context );
    }

    void register_simulation_deserialize_pcontext( PContext& context )
    {
        register_simulation_pcontext< Deserializer >( context );
    }
} // namespace geode
