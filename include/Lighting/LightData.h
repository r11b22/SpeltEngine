
#pragma once

#include "Lighting/AmbientLightData.h"
#include "Lighting/DirectionalLightData.h"
#include "Lighting/PointLightData.h"
#include <variant>

namespace Spelt {
    using LightData = std::variant<
    PointLightData,
    AmbientLightData,
    DirectionalLightData
    >;
}
