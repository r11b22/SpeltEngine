#pragma once

#include <filesystem>
#include <variant>
#include "Texture/Texture.h"
#include "Texture/TextureData.h"
#include "Asset/AssetLoader.hpp"
#include "Asset/EmbeddedAsset.hpp"

namespace Spelt {
    enum class TextureLoaderError{
        FileNotFound,
        LoadingFailed
    };

    class TextureLoader {
        private:
            TextureData mData;
        public:
            ~TextureLoader();
            Result<void, TextureLoaderError> readFile(const std::filesystem::path& path, bool flipVertical = false);
            Result<void, TextureLoaderError> readRaw(const unsigned char* binary, unsigned int size, bool flipVertical = false);

            Texture createTexture(std::string name);
        private:

    };


    struct TexturePathLoadSource {
        std::filesystem::path path;
    };

    struct TextureRawLoadSource {
        const unsigned char* binaryData;
        unsigned int binarySize;
    };

    template <>
    struct AssetLoadInfo<Texture> {
        std::string name;
        bool flipped;

        std::variant<TexturePathLoadSource, TextureRawLoadSource> source;

        static AssetLoadInfo FromPath(std::string name, std::filesystem::path path, bool flipped = false) {
            return AssetLoadInfo(
                std::move(name),
                flipped,
                TexturePathLoadSource{std::move(path)}
            );
        }

        static AssetLoadInfo FromEmbedded(std::string name, EmbeddedAsset embeddedAsset, bool flipped = false) {
            return AssetLoadInfo(
                std::move(name),
                flipped,
                TextureRawLoadSource{embeddedAsset.data, embeddedAsset.size}
            );
        }

    private:
        AssetLoadInfo(std::string n, bool f, std::variant<TexturePathLoadSource, TextureRawLoadSource> src)
            : name(std::move(n)), flipped(f), source(std::move(src)) {}
    };

    template <>
    struct AssetLoader<Texture> {
        static Texture load(AssetLoadInfo<Texture> asset) {
            TextureLoader loader{};

            if (std::holds_alternative<TexturePathLoadSource>(asset.source)) {
                const auto& pathSrc = std::get<TexturePathLoadSource>(asset.source);
                loader.readFile(pathSrc.path, asset.flipped).panicOnError(std::format("Could not find file at: {}", pathSrc.path.c_str()));
            }
            else if (std::holds_alternative<TextureRawLoadSource>(asset.source)) {
                const auto& rawSrc = std::get<TextureRawLoadSource>(asset.source);
                loader.readRaw(rawSrc.binaryData, rawSrc.binarySize, asset.flipped).panicOnError("Could not load texture form embedded binary!");
            }

            Texture texture = loader.createTexture(asset.name);

            return std::move(texture);
        }
    };
}
