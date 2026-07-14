#include "Texture/TextureLoader.h"
#include "Error/Result.hpp"
#include "Texture/Texture.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

namespace Spelt {
    TextureLoader::~TextureLoader(){
        stbi_image_free(mData.texData);
    }

    Texture TextureLoader::createTexture(std::string name) {
        return Texture{name, mData};
    }

    Result<void, TextureLoaderError> TextureLoader::readFile(const std::filesystem::path &path, bool flipVertical) {
        stbi_set_flip_vertically_on_load(flipVertical);
        int texWidth, texHeight, channelCount;
        unsigned char* texData = stbi_load(path.string().c_str(), &texWidth, &texHeight, &channelCount, 0);

        if (!texData) {
            return Error(TextureLoaderError::FileNotFound);
        }

        mData = {texData, texWidth, texHeight, channelCount};

        return Success{};
    }

    Result<void, TextureLoaderError> TextureLoader::readRaw(const unsigned char* binary, unsigned int size, bool flipVertical) {
        stbi_set_flip_vertically_on_load(flipVertical);
        int texWidth, texHeight, channelCount;

        // stbi_load_from_memory takes the raw encoded file buffer (e.g., the PNG file data)
        unsigned char* texData = stbi_load_from_memory(binary, size, &texWidth, &texHeight, &channelCount, 0);

        if (!texData) {
            return Error(TextureLoaderError::LoadingFailed);
        }

        mData = {texData, texWidth, texHeight, channelCount};
        return Success{};
    }
}
