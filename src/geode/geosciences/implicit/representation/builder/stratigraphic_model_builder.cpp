/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geosciences/implicit/representation/builder/stratigraphic_model_builder.hpp>

#include <geode/geometry/point.hpp>

#include <geode/model/mixin/core/block.hpp>

#include <geode/geosciences/implicit/geometry/stratigraphic_point.hpp>
#include <geode/geosciences/implicit/representation/core/stratigraphic_model.hpp>

namespace geode
{
    StratigraphicModelBuilder::StratigraphicModelBuilder(
        StratigraphicModel& stratigraphic_model )
        : ImplicitStructuralModelBuilder( stratigraphic_model ),
          stratigraphic_model_( stratigraphic_model )
    {
    }

    ModelCopyMapping StratigraphicModelBuilder::copy(
        const StratigraphicModel& implicit_model )
    {
        auto mappings = ImplicitStructuralModelBuilder::copy( implicit_model );
        stratigraphic_model_.initialize_stratigraphic_query_trees( {} );
        return mappings;
    }

    void StratigraphicModelBuilder::reinitialize_stratigraphic_query_trees()
    {
        this->reinitialize_implicit_query_trees();
        stratigraphic_model_.initialize_stratigraphic_query_trees( {} );
    }

    void StratigraphicModelBuilder::
        instantiate_stratigraphic_attribute_on_blocks()
    {
        this->instantiate_implicit_attribute_on_blocks();
        stratigraphic_model_.instantiate_stratigraphic_location_on_blocks( {} );
    }

    void StratigraphicModelBuilder::set_stratigraphic_location(
        const Block3D& block, index_t vertex_id, Point2D value )
    {
        stratigraphic_model_.set_stratigraphic_location(
            block, vertex_id, std::move( value ), {} );
    }

    void StratigraphicModelBuilder::set_stratigraphic_coordinates(
        const Block3D& block,
        index_t vertex_id,
        const StratigraphicPoint3D& value )
    {
        this->set_implicit_value( block, vertex_id, value.implicit_value() );
        stratigraphic_model_.set_stratigraphic_location(
            block, vertex_id, value.stratigraphic_location(), {} );
    }
} // namespace geode
