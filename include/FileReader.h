//
// Created by joost on 4/9/26.
//

#ifndef GCVCPROJECT_FILEREADER_H
#define GCVCPROJECT_FILEREADER_H

#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>

class FileReader {
public:
    static std::string readFile(const std::filesystem::path& path);
};

#endif //GCVCPROJECT_FILEREADER_H