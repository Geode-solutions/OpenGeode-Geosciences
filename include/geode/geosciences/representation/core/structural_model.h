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

#include <geode/model/representation/core/brep.h>

#include <geode/geosciences/mixin/core/faults.h>
#include <geode/geosciences/mixin/core/horizons.h>

namespace geode
{
    /*!
     * A Structural Model is a Boundary Representation composed of
     * Faults and Horizons.
     */
    class opengeode_geosciences_geosciences_api StructuralModel
        : public BRep,
          public AddComponents< 3, Faults, Horizons >
    {
    public:
        class opengeode_geosciences_geosciences_api HorizonItemRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< HorizonItemRange >
        {
        public:
            HorizonItemRange( const StructuralModel& structural_model,
                const Horizon3D& horizon );

            const Surface3D& operator*() const;

        private:
            const StructuralModel& structural_model_;
        };

        class opengeode_geosciences_geosciences_api FaultItemRange
            : public Relationships::ItemRangeIterator,
              public BeginEnd< FaultItemRange >
        {
        public:
            FaultItemRange(
                const StructuralModel& structural_model, const Fault3D& fault );

            const Surface3D& operator*() const;

        private:
            const StructuralModel& structural_model_;
        };

        static std::string native_extension_static()
        {
            return "og_strm";
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }

        HorizonItemRange items( const Horizon3D& horizon ) const;

        FaultItemRange items( const Fault3D& fault ) const;
    };
} // namespace geode
