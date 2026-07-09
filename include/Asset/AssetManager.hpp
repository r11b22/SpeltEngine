#pragma once


#include "Asset/AssetLoader.hpp"
#include "Asset/AssetReference.hpp"
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
#include <concepts>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <any>

class AssetManager {
    private:
        // Makes sure to keep track of destructors
        std::unordered_map<std::type_index, std::unique_ptr<void, void(*)(void*)>> mRepos;
    public:
        ~AssetManager();


        void clear();

        template<typename T>
            requires LoadableAsset<T> || LoadableAssetWithManager<T>
        AssetReference<T> loadAsset(AssetLoadInfo<T> source) {
            if constexpr (LoadableAssetWithManager<T>) {
                return addAsset(std::move(AssetLoader<T>::load(source, *this)));
            } else {
                return addAsset(std::move(AssetLoader<T>::load(source)));
            }
        }

        template<typename T>
            requires std::derived_from<T, Asset>
        AssetReference<T> addAsset(T asset){
            AssetRepository<T>& repo = getRepo<T>();

            return repo.pushAsset(std::move(asset));
        }

        template<typename T>
            requires std::derived_from<T, Asset>
        AssetReference<T> getAssetByName(const std::string& name) const{
            const AssetRepository<T>& repo = getRepo<T>();

            return repo.getAssetByName(name);
        }

        template<typename T>
            requires std::derived_from<T, Asset>
        bool hasAssetOfName(const std::string& name) const{
            if (!hasRepo<T>()){
                return false;
            }

            const AssetRepository<T>& repo = getRepo<T>();

            return repo.hasAssetOfName(name);
        }

        template<typename T>
            requires std::derived_from<T, Asset>
        T* getAsset(AssetReference<T> ref){
            AssetRepository<T>& repo = getRepo<T>();

            return repo.getAsset(std::move(ref));
        }


        template<typename T>
            requires std::derived_from<T, Asset>
        const T* getAsset(AssetReference<T> ref) const{
            const AssetRepository<T>& repo = getRepo<T>();

            return repo.getAsset(std::move(ref));
        }

    private:

        template<typename T>
        AssetRepository<T>& getRepo() {
            auto it = mRepos.find(typeid(T));
            if (it == mRepos.end()) {
                // Create the repository on the heap
                auto* repo = new AssetRepository<T>();

                // Insert it with a custom lambda deleter that remembers how to delete AssetRepository<T>
                auto deleter = [](void* ptr) { delete static_cast<AssetRepository<T>*>(ptr); };

                it = mRepos.emplace(typeid(T), std::unique_ptr<void, void(*)(void*)>(repo, deleter)).first;
            }
            return *static_cast<AssetRepository<T>*>(it->second.get());
        }

        template<typename T>
        bool hasRepo() const {
            auto it = mRepos.find(typeid(T));
            return it != mRepos.end();
        }

        template<typename T>
        const AssetRepository<T>& getRepo() const {
            auto it = mRepos.find(typeid(T));
            if (it == mRepos.end()) {
                throw std::runtime_error("The requested asset could not be found: No assets of that type exist yet!");
            }
            return *static_cast<const AssetRepository<T>*>(it->second.get());
        }
};
