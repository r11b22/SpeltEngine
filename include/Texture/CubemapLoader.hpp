#pragma once

#include "Texture/CubemapTexture.hpp"
#include "Texture/TextureData.h"
#include "Asset/AssetLoader.hpp"
#include <filesystem>

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
        void readFile(CubeFace face, const std::filesystem::path& path, bool flipVertical = false);

        CubemapTexture createCubemap(std::string name);
    private:

};

template <>
struct AssetLoadInfo<CubemapTexture> {
    std::string name;
    std::filesystem::path pathRight;
    std::filesystem::path pathLeft;
    std::filesystem::path pathFront;
    std::filesystem::path pathBack;
    std::filesystem::path pathTop;
    std::filesystem::path pathBottom;
    bool flipped;

    void setPath(CubeFace face, std::filesystem::path path){
        switch (face) {
            case CubeFace::Right:
            pathRight = path;
            break;

            case CubeFace::Left:
            pathLeft = path;
            break;

            case CubeFace::Front:
            pathFront = path;
            break;

            case CubeFace::Back:
            pathBack = path;
            break;

            case CubeFace::Top:
            pathTop = path;
            break;

            case CubeFace::Bottom:
            pathBottom = path;
            break;
        }
    }
};

template <>
struct AssetLoader<CubemapTexture> {
    static CubemapTexture load(AssetLoadInfo<CubemapTexture> asset) {
        CubmapLoader loader{};

        loader.readFile(CubeFace::Right,  asset.pathRight, asset.flipped);
        loader.readFile(CubeFace::Left,   asset.pathLeft, asset.flipped);
        loader.readFile(CubeFace::Top,    asset.pathTop, asset.flipped);
        loader.readFile(CubeFace::Bottom, asset.pathBottom, asset.flipped);
        loader.readFile(CubeFace::Front,  asset.pathFront, asset.flipped);
        loader.readFile(CubeFace::Back,   asset.pathBack, asset.flipped);

        CubemapTexture texture = loader.createCubemap(asset.name);

        return std::move(texture);
    }
};
