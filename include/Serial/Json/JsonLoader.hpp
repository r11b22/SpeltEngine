#pragma once

#include "Asset/EmbeddedAsset.hpp"
#include "Serial/Json/Json.hpp"
#include <filesystem>
class JsonLoader {
    private:
        std::string mContents;
    public:

        void readFile(const std::filesystem::path& path);
        void readRaw(EmbeddedAsset asset);

        Json createJson();

        Json fromString(std::string json);
    private:

};
