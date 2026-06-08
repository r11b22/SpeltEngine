//
// Created by joost on 6/3/26.
//

#include "Lighting/PointLight.h"

PointLight::PointLight(const std::string& name, glm::vec3 color)
    :
    TransformableObject(name),
    mAmbient(color * 0.05f),
    mDiffuse(color),
    mSpecular(color)
{

}


glm::vec3 PointLight::getAmbient() const{
    return mAmbient;
}

glm::vec3 PointLight::getDiffuse() const{
    return mDiffuse;
}

glm::vec3 PointLight::getSpecular() const{
    return mSpecular;
}

float PointLight::getConstantFalloff() const{
    return mConstant;
}

float PointLight::getLinearFalloff() const{
    return mLinear;
}

float PointLight::getQuadraticFallof() const{
    return mQuadratic;
}
