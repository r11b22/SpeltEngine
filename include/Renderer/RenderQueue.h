//
// Created by joost on 6/4/26.
//

#ifndef CGVCPROJECT_RENDERQUEUE_H
#define CGVCPROJECT_RENDERQUEUE_H
#include <memory>
#include <vector>

#include "IDrawable.h"
#include "RenderCommand.h"

class RenderQueue {
private:
    std::vector<RenderCommand> mRenderCommands;
public:

    const std::vector<RenderCommand>& getRenderCommands() const;

    void submitRenderCommands(std::vector<RenderCommand> commands);

    void clear();
private:
};

#endif //CGVCPROJECT_RENDERQUEUE_H
