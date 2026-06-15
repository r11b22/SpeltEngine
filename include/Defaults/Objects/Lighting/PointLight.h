#pragma once

#include <string>
#include <glm/glm.hpp>
#include "Defaults/Objects/TransformableObject.h"
#include "Lighting/ILight.h"
#include "Lighting/LightData.h"
#include "Lighting/PointLightData.h"


class PointLight : public TransformableObject, public ILight{

private:
    PointLightData mLightData;

public:
    PointLight(const std::string& name, glm::vec3 color);

    LightData getLightData() const override;
};
