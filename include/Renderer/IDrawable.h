//
// Created by joost on 4/12/26.
//

#ifndef GCVCPROJECT_DRAWABLE_H
#define GCVCPROJECT_DRAWABLE_H

#include <string>

#include "RenderCommand.h"
#include "../Shaders/ShaderProgram.h"
#include "Material/Material.h"

class IDrawable{
public:
    virtual ~IDrawable() = default;
    virtual std::vector<RenderCommand> getRenderCommands() = 0;
};

#endif //GCVCPROJECT_DRAWABLE_H