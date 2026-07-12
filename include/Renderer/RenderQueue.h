//
// Created by joost on 6/4/26.
//

#pragma once

#include <vector>

#include "RenderCommand.h"

namespace Spelt {
    class RenderQueue {
    private:
        std::vector<RenderCommand> mRenderCommands;

        StateChangeCommand mLastState{};
        size_t mLastDrawCommandHash = 0;
        int mLastDrawCommandIdx = -1;
    public:

        const std::vector<RenderCommand>& getRenderCommands() const;

        void submitRenderCommands(std::vector<RenderCommand> commands);

        void clear();
    private:
        void mergeDrawCommand(DrawCommand* into, DrawCommand& toMerge) const;
    };

}
