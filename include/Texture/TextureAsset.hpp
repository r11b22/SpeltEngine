#pragma once

#include <filesystem>

namespace Spelt {

    class TextureAsset {
        private:
            std::string mName;
            std::filesystem::path mPath;
            bool mFlipped;
        public:
            TextureAsset(std::string name, std::filesystem::path path, bool flipped = false);

            const std::string& getName() const;
            const std::filesystem::path& getPath() const;
            bool getFlipped() const;
        private:

    };
}
