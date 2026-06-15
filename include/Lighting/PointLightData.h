#pragma once

#include "glm/ext/vector_float3.hpp"

struct PointLightData {
    glm::vec3 difuse;
    glm::vec3 specular;
    float constant = 1.0f;
    float linear = 1.0f;
    float quadratic = 1.0f;

    glm::vec3 position{0.0f};

    PointLightData(glm::vec3 difuse, glm::vec3 specular) : difuse(difuse), specular(specular) {}
};
