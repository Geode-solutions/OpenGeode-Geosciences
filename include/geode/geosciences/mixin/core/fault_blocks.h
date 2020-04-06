/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/mesh/core/mesh_type.h>

#include <geode/geosciences/common.h>
#include <geode/geosciences/mixin/core/fault_block.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( FaultBlocksBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_geosciences_geosciences_api FaultBlocks
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( FaultBlocks );

    public:
        using Builder = FaultBlocksBuilder< dimension >;
        friend Builder;

        class opengeode_geosciences_geosciences_api FaultBlockRangeBase
        {
        public:
            ~FaultBlockRangeBase();

            bool operator!=( const FaultBlockRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            FaultBlockRangeBase( const FaultBlocks& fault_blocks );
            FaultBlockRangeBase( FaultBlockRangeBase&& other ) noexcept;
            FaultBlockRangeBase( const FaultBlockRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_geosciences_geosciences_api FaultBlockRange
            : public FaultBlockRangeBase
        {
        public:
            FaultBlockRange( const FaultBlocks& fault_blocks );

            const FaultBlockRange& begin() const
            {
                return *this;
            }

            const FaultBlockRange& end() const
            {
                return *this;
            }

            const FaultBlock< dimension >& operator*() const;
        };

    public:
        ~FaultBlocks();

        index_t nb_fault_blocks() const;

        const FaultBlock< dimension >& fault_block( const uuid& id ) const;

        FaultBlockRange fault_blocks() const;

        void save_fault_blocks( absl::string_view directory ) const;

    protected:
        friend class FaultBlocksBuilder< dimension >;
        FaultBlocks();

    private:
        class ModifiableFaultBlockRange : public FaultBlockRangeBase
        {
        public:
            ModifiableFaultBlockRange( const FaultBlocks& fault_blocks );

            const ModifiableFaultBlockRange& begin() const
            {
                return *this;
            }

            const ModifiableFaultBlockRange& end() const
            {
                return *this;
            }

            FaultBlock< dimension >& operator*() const;
        };

    private:
        const uuid& create_fault_block();

        void delete_fault_block( const FaultBlock< dimension >& fault_block );

        void load_fault_blocks( absl::string_view directory );

        ModifiableFaultBlockRange modifiable_fault_blocks();

        FaultBlock< dimension >& modifiable_fault_block( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( FaultBlocks );
} // namespace geode
