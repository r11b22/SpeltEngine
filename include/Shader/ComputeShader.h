//
// Created by joost on 6/5/26.
//
#pragma once

#include "ShaderProgram.h"

namespace Spelt {
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

        // --- Move Semantics ---
        ComputeShader(ComputeShader&&) noexcept = default;
        ComputeShader& operator=(ComputeShader&&) noexcept = default;

        // --- Disable Copy Semantics ---
        ComputeShader(const ComputeShader&) = delete;
        ComputeShader& operator=(const ComputeShader&) = delete;

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

        /*
        * Wait for the compute shader to finish all writing to the texture
        */
        void waitForFinish();
    };
}
