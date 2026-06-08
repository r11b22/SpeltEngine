//
// Created by joost on 6/5/26.
//


#include "Shaders/ComputeShader.h"

#include "FileReader.h"

ComputeShader::ComputeShader(const std::string &source, int groupXSize, int groupYSize, int groupZSize)
    : mGroupXSize(groupXSize), mGroupYSize(groupYSize), mGroupZSize(groupZSize)
{
    addShader(source.c_str(), GL_COMPUTE_SHADER);
    link();
}


ComputeShader ComputeShader::createFromSource(const std::string &source, int groupXSize, int groupYSize, int groupZSize) {
    return ComputeShader{source, groupXSize, groupYSize, groupZSize};
}

ComputeShader ComputeShader::createFromPath(const std::filesystem::path &path, int groupXSize, int groupYSize, int groupZSize) {
    const std::string source = FileReader::readFile(path);
    return ComputeShader{source, groupXSize, groupYSize, groupZSize};
}

void ComputeShader::dispatch(int xElements, int yElements, int zElements) {
    use();

    int numGroupsX = (xElements + mGroupXSize - 1) / mGroupXSize;
    int numGroupsY = (yElements + mGroupYSize - 1) / mGroupYSize;
    int numGroupsZ = (zElements + mGroupZSize - 1) / mGroupZSize;

    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}

void ComputeShader::rawDispatch(int xElements, int yElements, int zElements) const {
    int numGroupsX = (xElements + mGroupXSize - 1) / mGroupXSize;
    int numGroupsY = (yElements + mGroupYSize - 1) / mGroupYSize;
    int numGroupsZ = (zElements + mGroupZSize - 1) / mGroupZSize;

    glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ);
}
