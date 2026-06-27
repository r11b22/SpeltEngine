#include "Texture/CubemapLoader.hpp"
#include "Texture/CubemapTexture.hpp"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <utility>

CubmapLoader::~CubmapLoader(){
    stbi_image_free(mRight.texData);
}

CubemapTexture CubmapLoader::createCubemap(std::string name) {
    CubemapTexture texture{name, mRight.texWidth, mLeft.texHeight, mLeft.channelCount};

    texture.setSidePixelData(CubeFace::Right, mRight.texData);
    texture.setSidePixelData(CubeFace::Left, mLeft.texData);
    texture.setSidePixelData(CubeFace::Top, mTop.texData);
    texture.setSidePixelData(CubeFace::Bottom, mBottom.texData);
    texture.setSidePixelData(CubeFace::Back, mBack.texData);
    texture.setSidePixelData(CubeFace::Front, mFront.texData);

    return std::move(texture);
}

void CubmapLoader::readFile(CubeFace face, const std::filesystem::path& path, bool flipVertical) {
    stbi_set_flip_vertically_on_load(flipVertical);
    int texWidth, texHeight, channelCount;
    unsigned char* texData = stbi_load(path.string().c_str(), &texWidth, &texHeight, &channelCount, 0);

    if (!texData) {
        throw std::runtime_error("Could not load cubemap at: " + path.string());
    }

    switch (face) {
        case CubeFace::Right:
            if(mRight.texData){
                stbi_image_free(mRight.texData);
            }
            mRight = {texData, texWidth, texHeight, channelCount};
            break;
        case CubeFace::Left:
            if(mLeft.texData){
                stbi_image_free(mLeft.texData);
            }
            mLeft = {texData, texWidth, texHeight, channelCount};
            break;
        case CubeFace::Top:
            if(mTop.texData){
                stbi_image_free(mTop.texData);
            }
            mTop = {texData, texWidth, texHeight, channelCount};
            break;
        case CubeFace::Bottom:
            if(mBottom.texData){
                stbi_image_free(mBottom.texData);
            }
            mBottom = {texData, texWidth, texHeight, channelCount};
            break;
        case CubeFace::Back:
            if(mBack.texData){
                stbi_image_free(mBack.texData);
            }
            mBack = {texData, texWidth, texHeight, channelCount};
            break;
        case CubeFace::Front:
            if(mFront.texData){
                stbi_image_free(mFront.texData);
            }
            mFront = {texData, texWidth, texHeight, channelCount};
            break;
    }

}
