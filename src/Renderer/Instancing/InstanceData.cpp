

#include "Renderer/Instancing/InstanceData.hpp"
#include "Shader/ShaderUniform.h"
#include <unordered_set>
#include <vector>
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
