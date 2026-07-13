//
// Created by joost on 4/9/26.
//

#pragma once

#include "Error/Result.hpp"
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

namespace Spelt {
    enum class FileReaderError {
        FileNotFound,
        CouldNotOpen
    };

    class FileReader {
    public:
        static Result<std::string, FileReaderError> readFile(const std::filesystem::path& path);
    };
}
