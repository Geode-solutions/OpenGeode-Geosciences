/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geosciences/mixin/core/bitsery_archive.h>

#include <geode/model/mixin/core/bitsery_archive.h>

#include <geode/geosciences/mixin/core/fault.h>
#include <geode/geosciences/mixin/core/fault_block.h>
#include <geode/geosciences/mixin/core/horizon.h>
#include <geode/geosciences/mixin/core/stratigraphic_unit.h>

namespace bitsery
{
    namespace ext
    {
        template < typename TBase >
        struct GeosciencesHierarchy
        {
            using Childs = PolymorphicClassesList<>;
        };

        template <>
        struct GeosciencesHierarchy< geode::Component2D >
            : PolymorphicDerivedClasses< geode::Fault2D,
                  geode::Horizon2D,
                  geode::FaultBlock2D,
                  geode::StratigraphicUnit2D >
        {
        };

        template <>
        struct GeosciencesHierarchy< geode::Component3D >
            : PolymorphicDerivedClasses< geode::Fault3D,
                  geode::Horizon3D,
                  geode::FaultBlock3D,
                  geode::StratigraphicUnit3D >
        {
        };

        BITSERY_CLASS_NAME( geode::Fault2D, "Fault2D" );
        BITSERY_CLASS_NAME( geode::Horizon2D, "Horizon2D" );
        BITSERY_CLASS_NAME( geode::FaultBlock2D, "FaultBlock2D" );
        BITSERY_CLASS_NAME( geode::StratigraphicUnit2D, "StratigraphicUnit2D" );
        BITSERY_CLASS_NAME( geode::Fault3D, "Fault3D" );
        BITSERY_CLASS_NAME( geode::Horizon3D, "Horizon3D" );
        BITSERY_CLASS_NAME( geode::FaultBlock3D, "FaultBlock3D" );
        BITSERY_CLASS_NAME( geode::StratigraphicUnit3D, "StratigraphicUnit3D" );
    } // namespace ext
} // namespace bitsery

namespace
{
    template < typename Serializer >
    void register_pcontext( geode::PContext& context )
    {
        context.registerBasesList< Serializer,
            bitsery::ext::GeosciencesHierarchy >(
            bitsery::ext::PolymorphicClassesList< geode::Component2D >{} );
        context.registerBasesList< Serializer,
            bitsery::ext::GeosciencesHierarchy >(
            bitsery::ext::PolymorphicClassesList< geode::Component3D >{} );
    }
} // namespace

namespace geode
{
    void register_geosciences_serialize_pcontext( PContext& context )
    {
        register_pcontext< Serializer >( context );
    }

    void register_geosciences_deserialize_pcontext( PContext& context )
    {
        register_pcontext< Deserializer >( context );
    }
} // namespace geode
