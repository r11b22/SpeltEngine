
#include "Texture/TextureAsset.hpp"

TextureAsset::TextureAsset(std::string name, std::filesystem::path path, bool flipped)
: mName(name), mPath(path), mFlipped(flipped)
{

}


const std::string& TextureAsset::getName() const {
    return mName;
}

const std::filesystem::path& TextureAsset::getPath() const{
    return mPath;
}

bool TextureAsset::getFlipped() const{
    return mFlipped;
}
