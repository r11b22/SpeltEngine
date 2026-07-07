#pragma once

#include "Serial/Json/Json.hpp"
#include <filesystem>
class JsonLoader {
    private:
    public:

        Json readFile(const std::filesystem::path& path);
        Json getJson(std::string json);
    private:

};
