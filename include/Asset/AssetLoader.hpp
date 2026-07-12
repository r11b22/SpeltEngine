#pragma once


#include <concepts>

namespace Spelt {
    class AssetManager;

    template <typename T>
    struct AssetLoader;

    template <typename T>
    struct AssetLoadInfo;

    template<typename T>
    concept LoadableAssetBasic = requires(const AssetLoadInfo<T>& info) {
        { AssetLoader<T>::load(info) } -> std::same_as<T>;
    };

    template<typename T>
    concept LoadableAssetWithManager = requires(const AssetLoadInfo<T>& info, AssetManager& manager) {
        { AssetLoader<T>::load(info, manager) } -> std::same_as<T>;
    };

    template<typename T>
    concept LoadableAsset = LoadableAssetBasic<T> || LoadableAssetWithManager<T>;
}
