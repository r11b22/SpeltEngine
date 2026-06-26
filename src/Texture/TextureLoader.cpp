#include "Texture/TextureLoader.h"
#include "Texture/Texture.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


TextureLoader::~TextureLoader(){
    stbi_image_free(mData.texData);
}

Texture TextureLoader::createTexture(std::string name) {
    return Texture{name, mData};
}

void TextureLoader::readFile(const std::filesystem::path &path, bool flipVertical) {
    stbi_set_flip_vertically_on_load(flipVertical);
    int texWidth, texHeight, channelCount;
    unsigned char* texData = stbi_load(path.string().c_str(), &texWidth, &texHeight, &channelCount, 0);

    if (!texData) {
        throw std::runtime_error("Could not load texture at: " + path.string());
    }

    mData = {texData, texWidth, texHeight, channelCount};
}
