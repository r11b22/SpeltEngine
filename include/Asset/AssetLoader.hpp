#pragma once


#include <concepts>


template <typename T>
struct AssetLoader;

template <typename T>
struct AssetLoadInfo;

template<typename T>
concept LoadableAsset = requires(const AssetLoadInfo<T>& info) {
    { AssetLoader<T>::load(info) } -> std::same_as<T>;
};
