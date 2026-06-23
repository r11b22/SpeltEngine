#pragma once

#include "glm/ext/vector_float3.hpp"
#include <filesystem>

class MeshAsset {
    private:
        int mMeshID;
        glm::vec3 mScale;
        std::filesystem::path mMeshPath;
        std::string mName;
    public:
        MeshAsset(std::string name, std::filesystem::path path, int meshId = 0, glm::vec3 scale = glm::vec3{1.0f})
            : mMeshID(meshId), mMeshPath(path), mName(name), mScale(scale) {}

        std::filesystem::path getPath() const{
            return mMeshPath;
        }

        const std::string& getName() const{
            return mName;
        }

        int getMeshID() const{
            return mMeshID;
        }

        glm::vec3 getScale() const{
            return mScale;
        }
    private:
};
