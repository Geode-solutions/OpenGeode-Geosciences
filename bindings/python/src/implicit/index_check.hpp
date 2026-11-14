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

#pragma once

#include "../common.hpp"

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>

namespace geode
{
    /*!
     * Raise an IndexError if the vertex is not a vertex of the mesh of the
     * component (a Block3D or a Surface2D)
     */
    template < typename Component >
    void check_component_vertex(
        std::string_view name, const Component& component, index_t vertex_id )
    {
        check_index(
            name, "vertex", vertex_id, component.mesh().nb_vertices() );
    }

    /*!
     * Raise an IndexError if the polyhedron is not one of the Block
     */
    inline void check_block_polyhedron(
        std::string_view name, const Block3D& block, index_t polyhedron_id )
    {
        check_index(
            name, "polyhedron", polyhedron_id, block.mesh().nb_polyhedra() );
    }

    /*!
     * Raise an IndexError if the polygon is not one of the Surface
     */
    inline void check_surface_polygon(
        std::string_view name, const Surface2D& surface, index_t polygon_id )
    {
        check_index(
            name, "polygon", polygon_id, surface.mesh().nb_polygons() );
    }
} // namespace geode
