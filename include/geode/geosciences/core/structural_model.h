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

#include <geode/geosciences/core/faults.h>
#include <geode/geosciences/core/horizons.h>

namespace geode
{
    /*!
     * A Structural Model is a Boundary Representation composed of
     * Faults and Horizons.
     */
    class opengeode_geosciences_geosciences_api StructuralModel
        : public BRep,
          public Faults< 3 >,
          public Horizons< 3 >
    {
    public:
        static std::string native_extension_static()
        {
            return "og_strm";
        }

        std::string native_extension() const
        {
            return native_extension_static();
        }

        double epsilon() const
        {
            std::call_once( is_epsilon_initialized_,
                &StructuralModel::compute_epsilon, this, std::ref( epsilon_ ) );
            return epsilon_;
        }

    private:
        virtual void compute_epsilon( double& epsilon ) const = 0;

    protected:
        static constexpr double epsilon_scaling = 1e-7;

    private:
        mutable std::once_flag is_epsilon_initialized_;
        mutable double epsilon_{ 0 };
    };
} // namespace geode
