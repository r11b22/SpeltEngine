#include "Asset/AssetManager.hpp"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshLoader.hpp"
#include "Mesh/MeshReference.hpp"
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

TextureReference AssetManager::getTextureByName(const std::string& name) const{
    return mTextureRepo.getAssetByName(name);
}

Texture* AssetManager::getTexture(TextureReference ref){
    return mTextureRepo.getAsset(ref);
}

const Texture* AssetManager::getTexture(TextureReference ref) const{
    return mTextureRepo.getAsset(ref);
}
