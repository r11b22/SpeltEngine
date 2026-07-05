#pragma once

#include "Buffer/Buffer.h"
#include "Buffer/ShaderStorageBuffer.hpp"
#include "Mesh/MeshReference.hpp"
#include <unordered_map>
class InstancceBufferManager {
    private:
        std::unordered_map<MeshReference, ShaderStorageBuffer> mInstanceBuffers;
    public:
        bool hasInstanceBuffer(MeshReference mesh);
        void createInstanceBuffer(MeshReference mesh);

    private:
};
