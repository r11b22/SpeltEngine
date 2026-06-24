#pragma once

#include <filesystem>
#include "Texture/Texture.h"
#include "Texture/TextureData.h"

class TextureLoader {
    private:
        TextureData mData;
    public:
        ~TextureLoader();
        void readFile(const std::filesystem::path& path, bool flipVertical = false);

        Texture createTexture(std::string name);
    private:

};
