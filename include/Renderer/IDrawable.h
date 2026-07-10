//
// Created by joost on 4/12/26.
//

#ifndef GCVCPROJECT_DRAWABLE_H
#define GCVCPROJECT_DRAWABLE_H

#include "RenderCommand.h"
#include <cstddef>

struct RenderPassCommands {
    size_t pass;
    std::vector<RenderCommand> commands;
};

class IDrawable{
public:
    virtual ~IDrawable() = default;
    virtual std::vector<RenderPassCommands> getRenderCommands() = 0;
};

#endif //GCVCPROJECT_DRAWABLE_H
