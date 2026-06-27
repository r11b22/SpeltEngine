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

AssetManager::~AssetManager(){
    clear();
}


void AssetManager::clear() {
    mRepos.clear();
}
