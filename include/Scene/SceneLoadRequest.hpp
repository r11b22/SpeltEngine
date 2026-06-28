#pragma once

#include "Asset/AssetLoader.hpp"
#include "Asset/AssetManager.hpp"
class ILoadRequest {
    public:
        virtual ~ILoadRequest() = default;
        virtual void load(AssetManager& assetManager) const = 0;
};

template <typename T>
requires LoadableAsset<T>
class LoadRequest : public ILoadRequest {
    private:
        AssetLoadInfo<T> mToLoad;
    public:
        LoadRequest(AssetLoadInfo<T> toLoad) : mToLoad(toLoad) {}

        void load(AssetManager& assetManager) const override {
            assetManager.loadAsset(mToLoad);
        }
    private:
};
