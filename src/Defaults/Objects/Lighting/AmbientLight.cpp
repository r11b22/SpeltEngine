//
// Created by joost on 6/4/26.
//

#include "Defaults/Objects/Lighting/AmbientLight.h"
#include "Lighting/LightData.h"
#include "Scene/Object.h"

AmbientLight::AmbientLight(const std::string& name, glm::vec3 ambient)
    : Object(name),
    mLightData(ambient)
{

}

LightData AmbientLight::getLightData() const{
    return mLightData;
}
