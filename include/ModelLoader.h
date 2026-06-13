//
// Created by joost on 4/12/26.
//

#ifndef GCVCPROJECT_MODELLOADER_H
#define GCVCPROJECT_MODELLOADER_H
#include <filesystem>
#include <vector>

#include "glm/vec3.hpp"
#include "Material/Material.h"


struct aiScene;

namespace Assimp {
    class Importer;
}

class Mesh;


struct TextureData {
    unsigned char* texData = nullptr;
    int texWidth = 0, texHeight = 0, channelCount = 0;
};

class ModelLoader {
private:
    Assimp::Importer* mImporter = nullptr;
    const aiScene* mScene = nullptr;

    TextureData mTexData = {};
public:
    ModelLoader();
    ~ModelLoader();
    void readFile(const std::filesystem::path& path, bool collapse = true);
    void loadTexture(const std::filesystem::path& path, bool flipVertical = false);

    static TextureData loadTextureStatic(const std::filesystem::path& path, bool flipVertical = false);
    static void freeTextureData(TextureData& data);

    std::vector<float> getVertices(int mesh, glm::vec3 = glm::vec3{1.0f});
    std::vector<unsigned int> getIndices(int mesh);

    std::shared_ptr<Mesh> createMeshScaled(int mesh = 0, glm::vec3 scale = glm::vec3{1.0f});
    std::shared_ptr<Mesh> createMeshScaledUniform(int mesh = 0, float scale = 1.0f);
    std::shared_ptr<Mesh> createMesh(int mesh = 0);

    std::shared_ptr<Material> createMaterial();
private:

};

#endif //GCVCPROJECT_MODELLOADER_H
