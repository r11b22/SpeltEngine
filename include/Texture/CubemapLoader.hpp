#pragma once

#include "Error/Result.hpp"
#include "Texture/CubemapTexture.hpp"
#include "Texture/TextureData.h"
#include "Asset/AssetLoader.hpp"
#include <filesystem>


namespace Spelt {
    enum class CubemapLoaderError{
        FileNotFound
    };

    class CubmapLoader {
        private:
            TextureData mRight;
            TextureData mLeft;
            TextureData mTop;
            TextureData mBottom;
            TextureData mBack;
            TextureData mFront;
        public:
            ~CubmapLoader();
            Result<void, CubemapLoaderError> readFile(CubeFace face, const std::filesystem::path& path, bool flipVertical = false);

            CubemapTexture createCubemap(std::string name);
        private:

    };

    template <>
    struct AssetLoadInfo<CubemapTexture> {
        std::string name;
        std::filesystem::path pathRight; bool flippedRight = false;
        std::filesystem::path pathLeft; bool flippedLeft = false;
        std::filesystem::path pathFront; bool flippedFront = false;
        std::filesystem::path pathBack; bool flippedBack = false;
        std::filesystem::path pathTop; bool flippedTop = false;
        std::filesystem::path pathBottom; bool flippedBottom = false;

        void setPath(CubeFace face, std::filesystem::path path, bool flipped = false){
            switch (face) {
                case CubeFace::Right:
                pathRight = path;
                flippedRight = flipped;
                break;

                case CubeFace::Left:
                pathLeft = path;
                flippedLeft = flipped;
                break;

                case CubeFace::Front:
                pathFront = path;
                flippedFront = flipped;
                break;

                case CubeFace::Back:
                pathBack = path;
                flippedBack = flipped;
                break;

                case CubeFace::Top:
                pathTop = path;
                flippedTop = flipped;
                break;

                case CubeFace::Bottom:
                pathBottom = path;
                flippedBottom = flipped;
                break;
            }
        }
    };

    template <>
    struct AssetLoader<CubemapTexture> {
        static CubemapTexture load(AssetLoadInfo<CubemapTexture> asset) {
            CubmapLoader loader{};

            loader.readFile(CubeFace::Right,  asset.pathRight, asset.flippedRight).panicOnError(std::format("Could not read file at: {}", asset.pathRight.c_str()));
            loader.readFile(CubeFace::Left,   asset.pathLeft, asset.flippedLeft).panicOnError(std::format("Could not read file at: {}", asset.pathLeft.c_str()));
            loader.readFile(CubeFace::Top,    asset.pathTop, asset.flippedTop).panicOnError(std::format("Could not read file at: {}", asset.pathTop.c_str()));
            loader.readFile(CubeFace::Bottom, asset.pathBottom, asset.flippedBottom).panicOnError(std::format("Could not read file at: {}", asset.pathBottom.c_str()));
            loader.readFile(CubeFace::Front,  asset.pathFront, asset.flippedFront).panicOnError(std::format("Could not read file at: {}", asset.pathFront.c_str()));
            loader.readFile(CubeFace::Back,   asset.pathBack, asset.flippedBack).panicOnError(std::format("Could not read file at: {}", asset.pathBack.c_str()));

            CubemapTexture texture = loader.createCubemap(asset.name);

            return std::move(texture);
        }
    };
}
