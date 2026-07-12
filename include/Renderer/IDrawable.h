//
// Created by joost on 4/12/26.
//

#pragma once

#include "RenderCommand.h"
#include <cstddef>

namespace Spelt {
    struct RenderPassCommands {
        size_t pass;
        std::vector<RenderCommand> commands;
    };

    class IDrawable{
    public:
        virtual ~IDrawable() = default;
        virtual std::vector<RenderPassCommands> getRenderCommands() = 0;
    };
}
