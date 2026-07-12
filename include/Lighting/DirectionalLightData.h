#pragma once

#include "glm/ext/vector_float3.hpp"

namespace Spelt {
    struct DirectionalLightData {
        glm::vec3 difuse;
        glm::vec3 specular;

        glm::vec3 direction;

        DirectionalLightData(glm::vec3 direction, glm::vec3 difuse, glm::vec3 specular) : direction(direction), difuse(difuse), specular(specular) {}
    };
}
