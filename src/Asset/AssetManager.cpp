#include "Asset/AssetManager.hpp"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshLoader.hpp"
#include "Mesh/MeshReference.hpp"
#include "Texture/CubemapLoader.hpp"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureLoader.h"
#include "Texture/TextureReference.hpp"

void AssetManager::clear() {
    mMeshRepo.clear();
    mTextureRepo.clear();
}

MeshReference AssetManager::loadMesh(MeshAsset asset){
    MeshLoader loader{};

    loader.readFile(asset.getPath());


    Mesh mesh = loader.createMesh(asset.getName(), asset.getMeshID(), asset.getScale());
    MeshReference ref = mMeshRepo.pushAsset(std::move(mesh));

    return ref;
}

MeshReference AssetManager::addMesh(Mesh asset){
    return mMeshRepo.pushAsset(std::move(asset));
}

MeshReference AssetManager::getMeshByName(const std::string& name) const{
    return mMeshRepo.getAssetByName(name);
}

Mesh* AssetManager::getMesh(MeshReference ref) {
    return mMeshRepo.getAsset(ref);
}

const Mesh* AssetManager::getMesh(MeshReference ref) const {
    return mMeshRepo.getAsset(ref);
}

TextureReference AssetManager::loadTexture(TextureAsset asset){
    TextureLoader loader{};

    loader.readFile(asset.getPath(), asset.getFlipped());


    Texture texture = loader.createTexture(asset.getName());
    TextureReference ref = mTextureRepo.pushAsset(std::move(texture));

    return ref;
}

TextureReference AssetManager::addTexture(Texture texture){
    return mTextureRepo.pushAsset(std::move(texture));
}

TextureReference AssetManager::getTextureByName(const std::string& name) const{
    return mTextureRepo.getAssetByName(name);
}

Texture* AssetManager::getTexture(TextureReference ref){
    return mTextureRepo.getAsset(ref);
}

const Texture* AssetManager::getTexture(TextureReference ref) const{
    return mTextureRepo.getAsset(ref);
}



CubemapTextureReference AssetManager::loadCubemap(CubemapAsset asset){
    CubmapLoader loader{};

    loader.readFile(CubeFace::Right, asset.getPath(CubeFace::Right), asset.getFlipped());
    loader.readFile(CubeFace::Left, asset.getPath(CubeFace::Left), asset.getFlipped());
    loader.readFile(CubeFace::Top, asset.getPath(CubeFace::Top), asset.getFlipped());
    loader.readFile(CubeFace::Bottom, asset.getPath(CubeFace::Bottom), asset.getFlipped());
    loader.readFile(CubeFace::Back, asset.getPath(CubeFace::Back), asset.getFlipped());
    loader.readFile(CubeFace::Front, asset.getPath(CubeFace::Front), asset.getFlipped());


    CubemapTexture texture = loader.createCubemap(asset.getName());
    CubemapTextureReference ref = mCubemapTextureRepo.pushAsset(std::move(texture));

    return ref;
}

CubemapTextureReference AssetManager::addCubemap(CubemapTexture texture){
    return mCubemapTextureRepo.pushAsset(std::move(texture));
}

CubemapTextureReference AssetManager::getCubemapByName(const std::string& name) const{
    return mCubemapTextureRepo.getAssetByName(name);
}

CubemapTexture* AssetManager::getCubemap(CubemapTextureReference ref){
    return mCubemapTextureRepo.getAsset(ref);
}

const CubemapTexture* AssetManager::getCubemap(CubemapTextureReference ref) const{
    return mCubemapTextureRepo.getAsset(ref);
}
