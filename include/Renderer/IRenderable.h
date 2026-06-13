//
// Created by joost on 6/12/26.
//

#ifndef SPELTENGINE_IRENDERABLE_H
#define SPELTENGINE_IRENDERABLE_H
#include "Shaders/ShaderProgram.h"

class IRenderable{
public:
    virtual ~IRenderable() = default;
    virtual void draw(ShaderProgram& shaderProgram) = 0;
};

#endif //SPELTENGINE_IRENDERABLE_H