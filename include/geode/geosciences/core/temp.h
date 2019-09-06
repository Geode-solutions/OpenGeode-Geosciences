#pragma once

#include <geode/model/common.h>
#include <geode/model/mixin/core/blocks.h>
#include <geode/model/mixin/core/corners.h>
#include <geode/model/mixin/core/topology.h>
#include <geode/model/mixin/core/lines.h>
#include <geode/model/mixin/core/surfaces.h>
#include <geode/geosciences/core/faults.h>
#include <geode/geosciences/core/horizons.h>
namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Components );
    ALIAS_3D( Block );
    ALIAS_3D( Corner );
    ALIAS_3D( Line );
    ALIAS_3D( Surface );
} // namespace geode

namespace geode
{
    template < index_t dimension, template < index_t > class... Components >
    class GeoRepresentation : public Components< dimension >...
    {
        // methods
    };
    class BRep : public GeoRepresentation< 3, Corners, Lines, Surfaces, Blocks >
    {
        // methods
    };

    class StructuralModel : public BRep,
                            public Faults< 3 >,
                            public Horizons< 3 >
    {
        // methods
    };

    class CrossSection
        : public GeoRepresentation< 2, Corners, Lines, Surfaces >,
          public Faults< 2 >,
          public Horizons< 2 >
    {
        // methods
    };


} // namespace geode