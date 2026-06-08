//
// Created by joost on 6/5/26.
//

#ifndef CGVCPROJECT_COMPUTESHADER_H
#define CGVCPROJECT_COMPUTESHADER_H
#include "ShaderProgram.h"

class ComputeShader : public ShaderProgram {
private:
    ComputeShader(const std::string& source, int groupXSize, int groupYSize, int groupZSize);
    int mGroupXSize;
    int mGroupYSize;
    int mGroupZSize;

public:
    ~ComputeShader() = default;
    static ComputeShader createFromSource(const std::string& source, int groupXSize, int groupYSize, int groupZSize);
    static ComputeShader createFromPath(const std::filesystem::path& path, int groupXSize, int groupYSize, int groupZSize);

    /**
     * Dispatches the compute shader == executes it shader
     * @param xElements The x dimension of your elements e.g. length of an array
     * @param yElements The y dimension of your elements e.g. the column count in your 2d array. (1 for 1D array)
     * @param zElements The z dimension of your elements
     */
    void dispatch(int xElements = 1, int yElements = 1, int zElements = 1);

    /**
     * Dispatches the compute shader == executes it shader
     * This does not call use on the shader, make sure the shader is in use when calling this
     * @param xElements The x dimension of your elements e.g. length of an array
     * @param yElements The y dimension of your elements e.g. the column count in your 2d array. (1 for 1D array)
     * @param zElements The z dimension of your elements
     */
    void rawDispatch(int xElements = 1, int yElements = 1, int zElements = 1) const;
};

#endif //CGVCPROJECT_COMPUTESHADER_H