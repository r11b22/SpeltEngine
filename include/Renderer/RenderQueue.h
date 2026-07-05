//
// Created by joost on 6/4/26.
//

#ifndef CGVCPROJECT_RENDERQUEUE_H
#define CGVCPROJECT_RENDERQUEUE_H
#include <vector>

#include "RenderCommand.h"

class RenderQueue {
private:
    std::vector<RenderCommand> mRenderCommands;
public:

    void flattenCommands();
    const std::vector<RenderCommand>& getRenderCommands() const;

    void submitRenderCommands(std::vector<RenderCommand> commands);

    void clear();
private:
    void mergeDrawCommand(DrawCommand* into, const DrawCommand& toMerge) const;
};

#endif //CGVCPROJECT_RENDERQUEUE_H
