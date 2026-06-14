//
// Created by joost on 6/4/26.
//

#include "Renderer/RenderQueue.h"
#include "Renderer/RenderCommand.h"

void RenderQueue::submitDrawable(std::shared_ptr<IDrawable> drawable) {
    std::vector<RenderCommand> commands = drawable->getRenderCommands();

    mRenderCommands.reserve(mRenderCommands.size() + commands.size());

    mRenderCommands.insert(mRenderCommands.end(), std::make_move_iterator(commands.begin()), std::make_move_iterator(commands.end()));
}

const std::vector<RenderCommand>& RenderQueue::getRenderCommands() const{
    return mRenderCommands;
}

void RenderQueue::clear() {
    mRenderCommands.clear();
}
