#pragma once

#include "Texture/CubemapTexture.hpp"
#include <filesystem>
#include <string>


namespace Spelt {
    class CubemapAsset {
        private:
            std::string mName;
            std::filesystem::path mPathRight;
            std::filesystem::path mPathLeft;
            std::filesystem::path mPathFront;
            std::filesystem::path mPathBack;
            std::filesystem::path mPathTop;
            std::filesystem::path mPathBottom;
            bool mFlipped;
        public:
            explicit CubemapAsset(std::string name, bool flipped = false);

            void setPath(CubeFace face, std::filesystem::path path);
            const std::filesystem::path& getPath(CubeFace face) const;

            const std::string& getName() const;

            bool getFlipped() const;
        private:
    };
}
