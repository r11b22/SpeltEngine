#pragma once

#include "Asset/EmbeddedAsset.hpp"
#include "Serial/Json/Json.hpp"
#include <filesystem>

namespace Spelt {
    enum class JsonLoaderError{
        MissingOpeningQoute,
        UnterminatedString,
        InvalidLiteralVariant,
        UnexpectedCharacter,
        UnterminatedArray,
        MissingKey,
        MissingSeperator,
        UnclosedBraces,
    };

    class JsonLoader {
        private:
            std::string mContents;
        public:

            void readFile(const std::filesystem::path& path);
            void readRaw(EmbeddedAsset asset);

            Result<Json, JsonLoaderError> createJson();

            Result<Json, JsonLoaderError> fromString(std::string json);
        private:

    };
}
