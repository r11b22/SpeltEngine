//
// Created by joost on 6/4/26.
//

#include "Lighting/AmbientLight.h"

AmbientLight::AmbientLight(const std::string& name, glm::vec3 ambient)
    : TransformableObject(name),
    mAmbient(ambient)
{

}


glm::vec3 AmbientLight::getAmbient() {
    return mAmbient;
}
