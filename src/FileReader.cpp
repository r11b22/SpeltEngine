//
// Created by joost on 4/9/26.
//
#include "FileReader.h"


std::string FileReader::readFile(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("File not found: " + path.string());
    }

    std::ifstream fileStream(path, std::ios::in | std::ios::binary);

    if (!fileStream.is_open()) {
        throw std::runtime_error("Could not open file: " + path.string());
    }

    std::stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}
