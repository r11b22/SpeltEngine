
#include "Defaults/Objects/Lighting/DirectionalLight.h"
#include "Lighting/LightData.h"


DirectionalLight::DirectionalLight(const std::string& name, glm::vec3 direction, glm::vec3 color)
    : Object(name),
    mLightData(direction, color, color)
{

}

LightData DirectionalLight::getLightData() const {
    return mLightData;
}
