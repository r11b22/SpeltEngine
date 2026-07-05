#pragma once


#include "Renderer/Instancing/InstanceLayout.hpp"
#include "Shader/ShaderUniform.h"
#include <vector>
class InstanceData {
    private:
        std::vector<ShaderUniform> mUniforms;
    public:

        void addUniform(ShaderUniform uniform);

        const std::vector<ShaderUniform>& getUniforms() const;

        bool isInstanceOf(const InstanceData& other) const;

        InstanceLayout getLayout() const;

        void writeToBuffer(std::byte* dst, const InstanceLayout& layout) const;
    private:
};
