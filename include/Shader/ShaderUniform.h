//
// Created by joost on 6/13/26.
//

#ifndef SPELTENGINE_SHADERUNIFORM_H
#define SPELTENGINE_SHADERUNIFORM_H
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"
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

    bool operator==(const ShaderUniform& other) const {
        if (name != other.name) return false;
        if (data.index() != other.data.index()) return false;

        return std::visit([](const auto& lhs, const auto& rhs) -> bool {
            using TLhs = std::decay_t<decltype(lhs)>;
            using TRhs = std::decay_t<decltype(rhs)>;

            if constexpr (std::is_same_v<TLhs, TRhs>) {
                if constexpr (std::is_same_v<TLhs, glm::mat4>) {
                    return lhs[0] == rhs[0] && lhs[1] == rhs[1] &&
                            lhs[2] == rhs[2] && lhs[3] == rhs[3];
                } else {
                    return lhs == rhs;
                }
            }
            return false;
        }, data, other.data);
    }
};

#endif //SPELTENGINE_SHADERUNIFORM_H
