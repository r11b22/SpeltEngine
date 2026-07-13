#include "Mesh/MeshLoader.hpp"
#include "Error/Result.hpp"
#include "Mesh/MeshAsset.hpp"
#include "glm/ext/vector_float3.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

namespace Spelt {
    MeshLoader::MeshLoader() {
        mImporter = new Assimp::Importer();
    }

    MeshLoader::~MeshLoader() {
        delete mImporter;
    }

    Result<void, MeshLoaderError> MeshLoader::readFile(const std::filesystem::path &path, bool collapse) {

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

        if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode) {
            return Result<void, MeshLoaderError>::createError(MeshLoaderError::ReadFail);
        }

        return Result<void, MeshLoaderError>::createValue();
    }


    Result<std::vector<float>, MeshLoaderError> MeshLoader::getVertices(int idx, glm::vec3 scale) {

        if (!mScene) {
            return Result<std::vector<float>, MeshLoaderError>::createError(MeshLoaderError::NoFile);
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

        return Result<std::vector<float>, MeshLoaderError>::createValue(std::move(vertices));
    }

    Result<std::vector<unsigned int>, MeshLoaderError> MeshLoader::getIndices(int idx) {
        if (!mScene) {
            return Result<std::vector<unsigned int>, MeshLoaderError>::createError(MeshLoaderError::NoFile);
        }


        aiMesh* mesh = mScene->mMeshes[idx];

        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return Result<std::vector<unsigned int>, MeshLoaderError>::createValue(std::move(indices));
    }

    Mesh MeshLoader::createMesh(std::string name, int idx, glm::vec3 scale){
        Mesh newMesh{name};

        newMesh.setVertices(getVertices(idx, scale).value());
        newMesh.setIndices(getIndices(idx).value());

        return std::move(newMesh);
    }
}
