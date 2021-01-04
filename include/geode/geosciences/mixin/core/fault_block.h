/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/model/mixin/core/component.h>

#include <geode/basic/passkey.h>
#include <geode/geosciences/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( FaultBlocks );
    FORWARD_DECLARATION_DIMENSION_CLASS( FaultBlocksBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Geological component describing a Fault Block.
     */
    template < index_t dimension >
    class FaultBlock final : public Component< dimension >
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( FaultBlock );
        PASSKEY( FaultBlocks< dimension >, FaultBlocksKey );
        PASSKEY( FaultBlocksBuilder< dimension >, FaultBlocksBuilderKey );

        friend class bitsery::Access;

    public:
        ~FaultBlock();

        static ComponentType component_type_static()
        {
            return ComponentType{ "FaultBlock" };
        }

        ComponentType component_type() const final
        {
            return component_type_static();
        }

        ComponentID component_id() const
        {
            return { this->component_type_static(), this->id() };
        };

        FaultBlock( FaultBlocksKey ) : FaultBlock(){};

        void set_fault_block_name(
            absl::string_view name, FaultBlocksBuilderKey )
        {
            this->set_name( name );
        }

    private:
        FaultBlock();

        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
    ALIAS_2D_AND_3D( FaultBlock );
} // namespace geode
