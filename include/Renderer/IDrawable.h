//
// Created by joost on 4/12/26.
//

#ifndef GCVCPROJECT_DRAWABLE_H
#define GCVCPROJECT_DRAWABLE_H

#include <string>
#include "../Shaders/ShaderProgram.h"
#include "Material/Material.h"

class IDrawable{
public:
    virtual ~IDrawable() = default;
    virtual void draw(ShaderProgram& shaderProgram) = 0;
    virtual std::string getShaderProgramName() = 0;
    virtual Material& getMaterial() = 0;
};

#endif //GCVCPROJECT_DRAWABLE_H