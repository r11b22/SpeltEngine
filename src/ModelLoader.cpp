//
// Created by joost on 4/12/26.
//

#include "ModelLoader.h"

#include <iostream>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Mesh.h>
#include "stb_image.h"
#include "assimp/Importer.hpp"


ModelLoader::ModelLoader() {
    mImporter = new Assimp::Importer();
}

ModelLoader::~ModelLoader() {
    delete mImporter;
    freeTextureData(mTexData);
}

void ModelLoader::readFile(const std::filesystem::path &path, bool collapse) {

    if (collapse) {
        mScene = mImporter->ReadFile(path.string().c_str(),
    aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs |
        aiProcess_PreTransformVertices);
    }else{
        mScene = mImporter->ReadFile(path.string().c_str(),
            aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices |
                aiProcess_FlipUVs );
    }


    mTexData.texData = nullptr;

    if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode) {
        throw std::runtime_error(mImporter->GetErrorString());
    }

}

void ModelLoader::loadTexture(const std::filesystem::path &path, bool flipVertical) {
    if (mTexData.texData != nullptr) {
        freeTextureData(mTexData);
    }

    mTexData = loadTextureStatic(path, flipVertical);

    if (!mTexData.texData) {
        throw std::runtime_error("Could not load texture at: " + path.string());
    }
}



std::vector<float> ModelLoader::getVertices(int idx, glm::vec3 scale) {

    if (!mScene) {
        throw std::runtime_error("Could not get vertices: No file was read!");
    }

    aiMesh* mesh = mScene->mMeshes[idx];

    std::vector<float> vertices;


    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Position — scaled
        vertices.push_back(mesh->mVertices[i].x * scale.x);
        vertices.push_back(mesh->mVertices[i].y * scale.y);
        vertices.push_back(mesh->mVertices[i].z * scale.z);

        // Normals — not scaled; re-normalize to handle non-uniform scale
        if (mesh->HasNormals()) {
            float nx = mesh->mNormals[i].x / scale.x;
            float ny = mesh->mNormals[i].y / scale.y;
            float nz = mesh->mNormals[i].z / scale.z;
            float len = std::sqrt(nx * nx + ny * ny + nz * nz);
            if (len > 0.0f) { nx /= len; ny /= len; nz /= len; }
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        } else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }

        // UVs — unchanged
        if (mesh->HasTextureCoords(0)) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        } else {
            vertices.push_back(0.0f);
            vertices.push_back(0.0f);
        }
    }

    return std::move(vertices);
}

std::vector<unsigned int> ModelLoader::getIndices(int idx) {
    if (!mScene) {
        throw std::runtime_error("Could not get vertices: No file was read!");
    }


    aiMesh* mesh = mScene->mMeshes[idx];

    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    return std::move(indices);
}

std::shared_ptr<Mesh> ModelLoader::createMeshScaled(int idx, glm::vec3 scale) {
    std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>();

    newMesh->setVertices(getVertices(idx, scale));
    newMesh->setIndices(getIndices(idx));

    if (mTexData.texData) {
        newMesh->setTexture(mTexData);
    }

    return newMesh;
}

std::shared_ptr<Mesh> ModelLoader::createMeshScaledUniform(int idx, float scale) {
    return createMeshScaled(idx, glm::vec3{scale});
}

std::shared_ptr<Mesh> ModelLoader::createMesh(int idx) {
    return createMeshScaled(idx, glm::vec3{1.0f});
}

TextureData ModelLoader::loadTextureStatic(const std::filesystem::path &path, bool flipVertical) {
    stbi_set_flip_vertically_on_load(flipVertical);
    int texWidth, texHeight, channelCount;
    unsigned char* texData = stbi_load(path.string().c_str(), &texWidth, &texHeight, &channelCount, 0);

    if (!texData) {
        throw std::runtime_error("Could not load texture at: " + path.string());
    }

    return {texData, texWidth, texHeight, channelCount};
}

void ModelLoader::freeTextureData(TextureData &data) {
    stbi_image_free(data.texData);
    data.texData = nullptr;
}
