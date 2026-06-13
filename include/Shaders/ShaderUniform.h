//
// Created by joost on 6/13/26.
//

#ifndef SPELTENGINE_SHADERUNIFORM_H
#define SPELTENGINE_SHADERUNIFORM_H
#include <string>
#include <variant>
#include <glm/glm.hpp>

using UniformData = std::variant<
    int,
    float,
    unsigned int,
    bool,
    glm::mat4,
    glm::vec3
>;

struct ShaderUniform {
    std::string name;
    UniformData data;
};

#endif //SPELTENGINE_SHADERUNIFORM_H
