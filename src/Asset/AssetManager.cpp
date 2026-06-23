#include "Asset/AssetManager.hpp"
#include "Mesh/MeshAsset.hpp"
#include "Mesh/MeshLoader.hpp"
#include "Mesh/MeshReference.hpp"

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
