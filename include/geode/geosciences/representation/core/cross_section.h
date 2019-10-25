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

#include <geode/model/representation/core/section.h>

#include <geode/geosciences/mixin/core/faults.h>
#include <geode/geosciences/mixin/core/horizons.h>

namespace geode
{
    /*!
     * A Cross Section is a Section composed of Faults and Horizons.
     */
    class opengeode_geosciences_geosciences_api CrossSection
        : public Section,
          public AddComponents< 2, Faults, Horizons >
    {
    public:
        class opengeode_model_api HorizonItemRange
            : public Relationships::ItemRange
        {
        public:
            HorizonItemRange(
                const CrossSection& cross_section, const Horizon2D& horizon );

            const HorizonItemRange& begin() const
            {
                return *this;
            }

            const HorizonItemRange& end() const
            {
                return *this;
            }

            const Line2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        class opengeode_model_api FaultItemRange
            : public Relationships::ItemRange
        {
        public:
            FaultItemRange(
                const CrossSection& cross_section, const Fault2D& fault );

            const FaultItemRange& begin() const
            {
                return *this;
            }

            const FaultItemRange& end() const
            {
                return *this;
            }

            const Line2D& operator*() const;

        private:
            const CrossSection& cross_section_;
        };

        static std::string native_extension_static()
        {
            return "og_xsctn";
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }

        HorizonItemRange items( const Horizon2D& horizon ) const;

        FaultItemRange items( const Fault2D& fault ) const;
    };
} // namespace geode
