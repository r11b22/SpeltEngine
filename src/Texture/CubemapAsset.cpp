
#include "Texture/CubemapAsset.hpp"
#include "Texture/CubemapTexture.hpp"
#include <filesystem>

namespace Spelt {
    CubemapAsset::CubemapAsset(std::string name, bool flipped)
        : mName(name), mFlipped(flipped)
    {

    }
    void CubemapAsset::setPath(CubeFace face, std::filesystem::path path) {
        switch (face) {
            case CubeFace::Right:
            mPathRight = path;
            break;

            case CubeFace::Left:
            mPathLeft = path;
            break;

            case CubeFace::Front:
            mPathFront = path;
            break;

            case CubeFace::Back:
            mPathBack = path;
            break;

            case CubeFace::Top:
            mPathTop = path;
            break;

            case CubeFace::Bottom:
            mPathBottom = path;
            break;
        }
    }

    const std::filesystem::path& CubemapAsset::getPath(CubeFace face) const{
        switch (face) {
            case CubeFace::Right:
            return mPathRight ;

            case CubeFace::Left:
            return mPathLeft;

            case CubeFace::Front:
            return mPathFront;

            case CubeFace::Back:
            return mPathBack;

            case CubeFace::Top:
            return mPathTop;

            case CubeFace::Bottom:
            return mPathBottom;
        }

        // does not reach
        return mPathBack;
    }


    bool CubemapAsset::getFlipped() const{
        return mFlipped;
    }

    const std::string& CubemapAsset::getName() const {
        return mName;
    }
}
