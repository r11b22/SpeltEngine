#pragma once

#include "Buffer/ShaderStorageBuffer.hpp"
#include "Renderer/IRenderable.h"
#include "Renderer/Instancing/InstanceData.hpp"
#include "Shader/ShaderProgram.h"
#include <cstddef>
#include <vector>

namespace Spelt {
    class InstanceRenderer {
        private:
            std::vector<std::byte> mInstanceScratch;
            ShaderStorageBuffer mInstanceStreamBuffer;
            int mUniformLocation;
        public:
            InstanceRenderer(int uniformLocation);
            void draw(IRenderable* toRender, ShaderProgram& program, const std::vector<InstanceData>& instances);
        private:
    };
}
