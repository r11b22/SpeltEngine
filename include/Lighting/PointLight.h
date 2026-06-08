#pragma once

#include <vector>
#include <string>
#include "../Shaders/ShaderProgram.h"
#include <glm/glm.hpp>
#include "../Defaults/Objects/TransformableObject.h"


class PointLight : public TransformableObject{

private:
    glm::vec3 mAmbient;
    glm::vec3 mDiffuse;
    glm::vec3 mSpecular;
    float mConstant = 1.0f;
    float mLinear = 1.0f;
    float mQuadratic = 1.0f;

public:
    PointLight(const std::string& name, glm::vec3 color);

    glm::vec3 getAmbient() const;
    glm::vec3 getDiffuse() const;
    glm::vec3 getSpecular() const;

    float getConstantFalloff() const;
    float getLinearFalloff() const;
    float getQuadraticFallof() const;


};
