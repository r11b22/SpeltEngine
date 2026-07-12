//
// Created by joost on 6/3/26.
//

#include "Defaults/Objects/Lighting/PointLight.h"
#include "Lighting/LightData.h"
#include "Lighting/PointLightData.h"


namespace Spelt::Defaults {
    PointLight::PointLight(const std::string& name, glm::vec3 color)
        :
        TransformableObject(name),
        mLightData(color, color)
    {

    }

    LightData PointLight::getLightData() const {
        PointLightData result = mLightData;

        result.position = getPosition();
        return std::move(result);
    }
}
