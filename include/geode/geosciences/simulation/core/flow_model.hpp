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

#include <memory>
#include <string_view>

#include <geode/geosciences/simulation/common.hpp>

namespace bitsery
{
    class Access;
} // namespace bitsery

namespace geode
{
    class FluidModel;
} // namespace geode

namespace geode
{
    enum struct COMPOSITION_BASIS
    {
        mole,
        mass
    };

    /*!
     * Flow process of the problem. Leaf classes must never be renamed: their
     * type name is written in saved files.
     */
    class opengeode_geosciences_simulation_api FlowModel
    {
    public:
        virtual ~FlowModel();

        [[nodiscard]] virtual std::string_view type_name() const = 0;

        [[nodiscard]] bool has_fluid() const;

        [[nodiscard]] const FluidModel& fluid() const;

        /// The fluid is shared: do not modify it after this call
        void set_fluid( std::shared_ptr< FluidModel > fluid );

    protected:
        FlowModel();
        FlowModel( const FlowModel& );
        FlowModel( FlowModel&& ) noexcept;
        FlowModel& operator=( const FlowModel& );
        FlowModel& operator=( FlowModel&& ) noexcept;

        template < typename Archive >
        void serialize( Archive& archive );

    private:
        friend class bitsery::Access;
        std::shared_ptr< FluidModel > fluid_;
    };

    class opengeode_geosciences_simulation_api SinglePhaseFlow final
        : public FlowModel
    {
    public:
        [[nodiscard]] static std::string_view type_name_static()
        {
            return "SinglePhaseFlow";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };

    class opengeode_geosciences_simulation_api MultiphaseFlow final
        : public FlowModel
    {
    public:
        [[nodiscard]] static std::string_view type_name_static()
        {
            return "MultiphaseFlow";
        }

        [[nodiscard]] std::string_view type_name() const final
        {
            return type_name_static();
        }

        MultiphaseFlow();
        explicit MultiphaseFlow( COMPOSITION_BASIS basis );

        [[nodiscard]] COMPOSITION_BASIS basis() const
        {
            return basis_;
        }

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

    private:
        COMPOSITION_BASIS basis_{ COMPOSITION_BASIS::mole };
    };
} // namespace geode
