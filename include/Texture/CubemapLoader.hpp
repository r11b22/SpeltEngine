#pragma once

#include "Texture/CubemapTexture.hpp"
#include "Texture/TextureData.h"
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
