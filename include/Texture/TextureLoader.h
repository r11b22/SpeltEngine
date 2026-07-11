#pragma once

#include <filesystem>
#include <variant>
#include "Texture/Texture.h"
#include "Texture/TextureData.h"
#include "Asset/AssetLoader.hpp"
#include "Asset/EmbeddedAsset.hpp"

class TextureLoader {
    private:
        TextureData mData;
    public:
        ~TextureLoader();
        void readFile(const std::filesystem::path& path, bool flipVertical = false);
        void readRaw(const unsigned char* binary, unsigned int size, bool flipVertical = false);

        Texture createTexture(std::string name);
    private:

};


struct PathLoadSource {
    std::filesystem::path path;
};

struct RawLoadSource {
    const unsigned char* binaryData;
    unsigned int binarySize;
};

template <>
struct AssetLoadInfo<Texture> {
    std::string name;
    bool flipped;

    std::variant<PathLoadSource, RawLoadSource> source;

    static AssetLoadInfo FromPath(std::string name, std::filesystem::path path, bool flipped = false) {
        return AssetLoadInfo(
            std::move(name),
            flipped,
            PathLoadSource{std::move(path)}
        );
    }

    static AssetLoadInfo FromEmbedded(std::string name, EmbeddedAsset embeddedAsset, bool flipped = false) {
        return AssetLoadInfo(
            std::move(name),
            flipped,
            RawLoadSource{embeddedAsset.data, embeddedAsset.size}
        );
    }

private:
    AssetLoadInfo(std::string n, bool f, std::variant<PathLoadSource, RawLoadSource> src)
        : name(std::move(n)), flipped(f), source(std::move(src)) {}
};

template <>
struct AssetLoader<Texture> {
    static Texture load(AssetLoadInfo<Texture> asset) {
        TextureLoader loader{};

        if (std::holds_alternative<PathLoadSource>(asset.source)) {
            const auto& pathSrc = std::get<PathLoadSource>(asset.source);
            loader.readFile(pathSrc.path, asset.flipped);
        }
        else if (std::holds_alternative<RawLoadSource>(asset.source)) {
            const auto& rawSrc = std::get<RawLoadSource>(asset.source);
            loader.readRaw(rawSrc.binaryData, rawSrc.binarySize, asset.flipped);
        }

        Texture texture = loader.createTexture(asset.name);

        return std::move(texture);
    }
};
