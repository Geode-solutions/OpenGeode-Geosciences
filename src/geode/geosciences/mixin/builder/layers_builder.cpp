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

#include <geode/geosciences/mixin/builder/layers_builder.h>

#include <geode/geosciences/mixin/core/layer.h>
#include <geode/geosciences/mixin/core/layers.h>

namespace geode
{
    template < index_t dimension >
    const uuid& LayersBuilder< dimension >::create_layer()
    {
        return layers_.create_layer();
    }

    template < index_t dimension >
    void LayersBuilder< dimension >::delete_layer(
        const Layer< dimension >& layer )
    {
        layers_.delete_layer( layer );
    }

    template < index_t dimension >
    void LayersBuilder< dimension >::load_layers( absl::string_view directory )
    {
        return layers_.load_layers( directory );
    }

    template < index_t dimension >
    void LayersBuilder< dimension >::set_layer_name(
        const uuid& id, absl::string_view name )
    {
        layers_.modifiable_layer( id ).set_layer_name( name );
    }

    template class opengeode_geosciences_geosciences_api LayersBuilder< 2 >;
    template class opengeode_geosciences_geosciences_api LayersBuilder< 3 >;
} // namespace geode
