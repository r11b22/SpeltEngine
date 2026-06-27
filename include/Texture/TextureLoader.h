#pragma once

#include <filesystem>
#include "Texture/Texture.h"
#include "Texture/TextureData.h"
#include "Asset/AssetLoader.hpp"

class TextureLoader {
    private:
        TextureData mData;
    public:
        ~TextureLoader();
        void readFile(const std::filesystem::path& path, bool flipVertical = false);

        Texture createTexture(std::string name);
    private:

};


template <>
struct AssetLoadInfo<Texture> {
    std::string name;
    std::filesystem::path path;
    bool flipped;
};

template <>
struct AssetLoader<Texture> {
    static Texture load(AssetLoadInfo<Texture> asset) {
        TextureLoader loader{};
        loader.readFile(asset.path, asset.flipped);

        Texture mesh = loader.createTexture(asset.name);

        return std::move(mesh);
    }
};
