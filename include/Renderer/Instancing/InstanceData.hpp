#pragma once


#include "Shader/ShaderUniform.h"
#include <vector>
class InstanceData {
    private:
        std::vector<ShaderUniform> mUniforms;
    public:

        void addUniform(ShaderUniform uniform);

        const std::vector<ShaderUniform>& getUniforms() const;

        bool isInstanceOf(const InstanceData& other) const;
    private:
};
