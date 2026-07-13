//
// Created by joost on 4/9/26.
//
#include "FileReader.h"
#include "Error/Result.hpp"

namespace Spelt {
    Result<std::string, FileReaderError> FileReader::readFile(const std::filesystem::path& path) {
        if (!std::filesystem::exists(path)) {
            return Result<std::string, FileReaderError>::createError(FileReaderError::FileNotFound);
        }

        std::ifstream fileStream(path, std::ios::in | std::ios::binary);

        if (!fileStream.is_open()) {
            return Result<std::string, FileReaderError>::createError(FileReaderError::CouldNotOpen);
        }

        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        return Result<std::string, FileReaderError>::createValue(buffer.str());
    }
}
