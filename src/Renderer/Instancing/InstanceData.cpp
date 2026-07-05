

#include "Renderer/Instancing/InstanceData.hpp"
#include "Renderer/Instancing/InstanceLayout.hpp"
#include "Shader/ShaderUniform.h"
#include <iostream>
#include <type_traits>
#include <unordered_set>
#include <variant>
#include <vector>
#include <cstring>

void InstanceData::addUniform(ShaderUniform uniform){
    mUniforms.push_back(std::move(uniform));
}

const std::vector<ShaderUniform>& InstanceData::getUniforms() const{
    return mUniforms;
}


bool InstanceData::isInstanceOf(const InstanceData& other) const{
    // If the counts don't match, they can't have the exact same set of uniforms
    if (mUniforms.size() != other.mUniforms.size()) {
        return false;
    }

    // Collect all uniform names from the 'other' instance
    std::unordered_set<std::string> otherNames;
    for (const auto& uniform : other.mUniforms) {
        // Adjust 'uniform.name' to 'uniform.getName()' if it's a private member with a getter
        otherNames.insert(uniform.name);
    }

    // Check if every uniform in 'this' exists in the 'other' set
    for (const auto& uniform : mUniforms) {
        if (otherNames.find(uniform.name) == otherNames.end()) {
            return false; // Found a uniform name that doesn't exist in 'other'
        }
    }

    return true;
}

InstanceLayout InstanceData::getLayout() const {
    InstanceLayout layout;
    size_t currentOffset = 0;

    for (const auto& uniform : getUniforms()){
        InstanceAttribDesc desc;
        desc.name = uniform.name;

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, float>) {
                desc.type = InstanceAttribType::Float;
                desc.byteSize = 4;
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                // WARNING: std430 forces vec3 to align to 16 bytes in arrays!
                // It's safest to use vec4 in your shaders instead of vec3 for SSBOs.
                desc.type = InstanceAttribType::Vec3;
                desc.byteSize = 12;
            }else if constexpr (std::is_same_v<T, bool>) {
                desc.type = InstanceAttribType::Int;
                desc.byteSize = 4;
            } else if constexpr (std::is_same_v<T, glm::mat4>) {
                desc.type = InstanceAttribType::Mat4;
                desc.byteSize = 64;
            } else if constexpr (std::is_same_v<T, int>) {
                desc.type = InstanceAttribType::Int;
                desc.byteSize = 4;
            }
        }, uniform.data);

        desc.offset = currentOffset;
        currentOffset += desc.byteSize;
        layout.attribs.push_back(std::move(desc));
    }

    layout.stride = currentOffset;
    return layout;
}

void InstanceData::writeToBuffer(std::byte* dst, const InstanceLayout& layout) const {
    const auto& uniforms = getUniforms();
    for (size_t i = 0; i < uniforms.size() && i < layout.attribs.size(); ++i) {
        const auto& attrib = layout.attribs[i];
        const auto& uniform = uniforms[i];

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_trivially_copyable_v<T>) {
                std::memcpy(dst + attrib.offset, &arg, sizeof(T));
            }
        }, uniform.data);
    }
}
