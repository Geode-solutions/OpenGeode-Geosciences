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
#include <geode/geosciences/mixin/core/layer.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( LayersBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class opengeode_geosciences_geosciences_api Layers
    {
        OPENGEODE_DISABLE_COPY_AND_MOVE( Layers );

    public:
        using Builder = LayersBuilder< dimension >;
        friend Builder;

        class opengeode_geosciences_geosciences_api LayerRangeBase
        {
        public:
            ~LayerRangeBase();

            bool operator!=( const LayerRangeBase& /*unused*/ ) const;

            void operator++();

        protected:
            LayerRangeBase( const Layers& layers );
            LayerRangeBase( LayerRangeBase&& other ) noexcept;
            LayerRangeBase( const LayerRangeBase& other );

        protected:
            IMPLEMENTATION_MEMBER( impl_ );
        };

        class opengeode_geosciences_geosciences_api LayerRange
            : public LayerRangeBase
        {
        public:
            LayerRange( const Layers& layers );

            const LayerRange& begin() const
            {
                return *this;
            }

            const LayerRange& end() const
            {
                return *this;
            }

            const Layer< dimension >& operator*() const;
        };

    public:
        ~Layers();

        index_t nb_layers() const;

        const Layer< dimension >& layer( const uuid& id ) const;

        LayerRange layers() const;

        void save_layers( absl::string_view directory ) const;

    protected:
        friend class LayersBuilder< dimension >;
        Layers();

    private:
        class ModifiableLayerRange : public LayerRangeBase
        {
        public:
            ModifiableLayerRange( const Layers& layers );

            const ModifiableLayerRange& begin() const
            {
                return *this;
            }

            const ModifiableLayerRange& end() const
            {
                return *this;
            }

            Layer< dimension >& operator*() const;
        };

    private:
        const uuid& create_layer();

        void delete_layer( const Layer< dimension >& layer );

        void load_layers( absl::string_view directory );

        ModifiableLayerRange modifiable_layers();

        Layer< dimension >& modifiable_layer( const uuid& id );

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( Layers );
} // namespace geode
