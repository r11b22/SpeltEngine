#pragma once


#include "Mesh/Mesh.h"
#include "assimp/Importer.hpp"
#include <filesystem>
#include <vector>
#include <glm/glm.hpp>

class MeshLoader {
    private:
        Assimp::Importer* mImporter = nullptr;
        const aiScene* mScene = nullptr;
    public:
        MeshLoader();
        ~MeshLoader();
        void readFile(const std::filesystem::path& path, bool collapse = true);

        std::vector<float> getVertices(int mesh, glm::vec3 = glm::vec3{1.0f});
        std::vector<unsigned int> getIndices(int mesh);

        Mesh createMesh(std::string name, int mesh, glm::vec3 scale = glm::vec3{1.0f});
    private:
};
