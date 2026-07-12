//
// Created by joost on 6/12/26.
//

#pragma once
#include "Shader/ShaderProgram.h"

namespace Spelt {
    class IRenderable{
    public:
        virtual ~IRenderable() = default;
        virtual void draw(ShaderProgram& shaderProgram) = 0;

        // TODO make this optional
        virtual void drawInstanced(ShaderProgram& shaderProgram, int instanceCount) = 0;
    };
}
