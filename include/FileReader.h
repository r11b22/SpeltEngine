//
// Created by joost on 4/9/26.
//

#pragma once

#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

namespace Spelt {
    class FileReader {
    public:
        static std::string readFile(const std::filesystem::path& path);
    };
}
