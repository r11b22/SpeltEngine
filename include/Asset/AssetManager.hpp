#pragma once


#include "Asset/AssetRepository.hpp"
#include "Mesh/Mesh.h"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshReference.hpp"
#include "Texture/CubemapAsset.hpp"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureAsset.hpp"
#include "Texture/TextureReference.hpp"

class AssetManager {
    private:
        AssetRepository<Mesh> mMeshRepo;
        AssetRepository<Texture> mTextureRepo;
        AssetRepository<CubemapTexture> mCubemapTextureRepo;
    public:
        void clear();

        MeshReference loadMesh(MeshAsset asset);
        MeshReference addMesh(Mesh asset);
        MeshReference getMeshByName(const std::string& name) const;
        Mesh* getMesh(MeshReference ref);
        const Mesh* getMesh(MeshReference ref) const;

        TextureReference loadTexture(TextureAsset asset);
        TextureReference addTexture(Texture asset);
        TextureReference getTextureByName(const std::string& name) const;
        Texture* getTexture(TextureReference ref);
        const Texture* getTexture(TextureReference ref) const;


        CubemapTextureReference loadCubemap(CubemapAsset asset);
        CubemapTextureReference addCubemap(CubemapTexture asset);
        CubemapTextureReference getCubemapByName(const std::string& name) const;
        CubemapTexture* getCubemap(CubemapTextureReference ref);
        const CubemapTexture* getCubemap(CubemapTextureReference ref) const;

    private:
};
