#pragma once


#include "Asset/AssetLoader.hpp"
#include "Mesh/Mesh.h"
#include "assimp/Importer.hpp"
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>



namespace Spelt {
    enum class MeshLoaderError {
        ReadFail,
        NoFile
    };

    class MeshLoader {
        private:
            Assimp::Importer* mImporter = nullptr;
            const aiScene* mScene = nullptr;
        public:
            MeshLoader();
            ~MeshLoader();
            Result<void, MeshLoaderError> readFile(const std::filesystem::path& path, bool collapse = true);

            Result<std::vector<float>, MeshLoaderError> getVertices(int mesh, glm::vec3 = glm::vec3{1.0f});
            Result<std::vector<unsigned int>, MeshLoaderError> getIndices(int mesh);

            Mesh createMesh(std::string name, int mesh, glm::vec3 scale = glm::vec3{1.0f});
        private:
    };

    template <>
    struct AssetLoadInfo<Mesh> {
        std::string name;
        std::filesystem::path path;
        int meshIdx = 0;
        glm::vec3 scale {1.0f};
    };

    template <>
    struct AssetLoader<Mesh> {
        static Mesh load(AssetLoadInfo<Mesh> asset) {
            MeshLoader loader{};
            loader.readFile(asset.path).throwOnError(std::format("Could not load requested asset with path: {}", asset.path.c_str()));

            Mesh mesh = loader.createMesh(asset.name, asset.meshIdx, asset.scale);

            return std::move(mesh);
        }
    };
}
