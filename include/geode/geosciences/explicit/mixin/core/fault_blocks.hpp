/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/basic/passkey.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/geosciences/explicit/common.hpp>
#include <geode/geosciences/explicit/mixin/core/fault_block.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( FaultBlocksBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_geosciences_explicit_api FaultBlocks
    {
        OPENGEODE_DISABLE_COPY( FaultBlocks );
        PASSKEY( FaultBlocksBuilder< dimension >, FaultBlocksBuilderKey );

    public:
        using Builder = FaultBlocksBuilder< dimension >;
        using Type = FaultBlock< dimension >;

        class opengeode_geosciences_explicit_api FaultBlockRangeBase
        {
        public:
            ~FaultBlockRangeBase();

            [[nodiscard]] bool operator!=(
                const FaultBlockRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            FaultBlockRangeBase( const FaultBlocks& fault_blocks );
            FaultBlockRangeBase( FaultBlockRangeBase&& other ) noexcept;
            FaultBlockRangeBase( const FaultBlockRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_geosciences_explicit_api FaultBlockRange
            : public FaultBlockRangeBase
        {
        public:
            FaultBlockRange( const FaultBlocks& fault_blocks );

            [[nodiscard]] const FaultBlockRange& begin() const
            {
                return *this;
            }

            [[nodiscard]] const FaultBlockRange& end() const
            {
                return *this;
            }

            [[nodiscard]] const FaultBlock< dimension >& operator*() const;
        };

    public:
        ~FaultBlocks();

        [[nodiscard]] index_t nb_fault_blocks() const;

        [[nodiscard]] bool has_fault_block( const uuid& id ) const;

        [[nodiscard]] const FaultBlock< dimension >& fault_block(
            const uuid& id ) const;

        [[nodiscard]] FaultBlockRange fault_blocks() const;

        [[nodiscard]] FaultBlockRange components() const
        {
            return fault_blocks();
        }

        void save_fault_blocks( std::string_view directory ) const;

    protected:
        friend class FaultBlocksBuilder< dimension >;
        FaultBlocks();
        FaultBlocks( FaultBlocks&& other ) noexcept;

        FaultBlocks& operator=( FaultBlocks&& other ) noexcept;

    private:
        class ModifiableFaultBlockRange : public FaultBlockRangeBase
        {
        public:
            ModifiableFaultBlockRange( const FaultBlocks& fault_blocks );

            [[nodiscard]] const ModifiableFaultBlockRange& begin() const
            {
                return *this;
            }

            [[nodiscard]] const ModifiableFaultBlockRange& end() const
            {
                return *this;
            }

            [[nodiscard]] FaultBlock< dimension >& operator*() const;
        };

    public:
        [[nodiscard]] const uuid& create_fault_block(
            FaultBlocksBuilderKey key );

        void create_fault_block(
            uuid fault_block_id, FaultBlocksBuilderKey key );

        void delete_fault_block( const FaultBlock< dimension >& fault_block,
            FaultBlocksBuilderKey key );

        void load_fault_blocks(
            std::string_view directory, FaultBlocksBuilderKey key );

        [[nodiscard]] ModifiableFaultBlockRange modifiable_fault_blocks(
            FaultBlocksBuilderKey key );

        [[nodiscard]] FaultBlock< dimension >& modifiable_fault_block(
            const uuid& id, FaultBlocksBuilderKey key );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( FaultBlocks );
} // namespace geode
