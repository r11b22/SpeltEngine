
#pragma once

#include "Lighting/AmbientLightData.h"
#include "Lighting/PointLightData.h"
#include <variant>
using LightData = std::variant<
PointLightData,
AmbientLightData
>;
