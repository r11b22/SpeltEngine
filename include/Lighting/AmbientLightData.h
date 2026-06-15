#pragma once

#include "glm/ext/vector_float3.hpp"
#include "glm/vec3.hpp"

struct AmbientLightData {
    glm::vec3 ambient;

    AmbientLightData(glm::vec3 ambient) : ambient(ambient) {}
};
