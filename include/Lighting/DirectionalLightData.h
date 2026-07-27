#pragma once

#include "glm/ext/vector_float3.hpp"

namespace Spelt {
    struct DirectionalLightData {
        glm::vec3 difuse;
        glm::vec3 specular;

        glm::vec3 direction;

        DirectionalLightData(glm::vec3 direction, glm::vec3 difuse, glm::vec3 specular) : direction(direction), difuse(difuse), specular(specular) {}

        /*
             * Memory Layout Format (std140 / std430 Aligned):
             *
             * In GPU buffer layouts like std140, a `vec3` requires a 16-byte alignment
             * (equivalent to `vec4`). Padding with a 4th dummy float after each `vec3`
             * ensures proper alignment when passing structured data to shaders.
             *
             * Offset (Floats) | Bytes     | Member           | Purpose
             * ----------------|-----------|------------------|---------------------
             * [0 .. 2]        |  0 .. 11  | difuse.x, y, z   | Diffuse color RGB
             * [3]             | 12 .. 15  | 0.0f             | Padding
             * [4 .. 6]        | 16 .. 27  | specular.x, y, z | Specular color RGB
             * [7]             | 28 .. 31  | 0.0f             | Padding
             * [8 .. 10]       | 32 .. 43  | direction.x, y, z| Light direction XYZ
             * [11]            | 44 .. 47  | 0.0f             | Padding
             *
             * Total Size: 12 floats (48 bytes)
             */
            std::vector<float> toPaddedVector() const {
                return {
                    difuse.x,    difuse.y,    difuse.z,    0.0f, // [0..3]   Diffuse + Pad
                    specular.x,  specular.y,  specular.z,  0.0f, // [4..7]   Specular + Pad
                    direction.x, direction.y, direction.z, 0.0f  // [8..11]  Direction + Pad
                };
            }
    };
}
