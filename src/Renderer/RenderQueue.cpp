//
// Created by joost on 6/4/26.
//

#include "Renderer/RenderQueue.h"
#include "Hashing/Hashing.hpp"
#include "Renderer/RenderCommand.h"
#include <cstddef>
#include <iterator>
#include <vector>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void RenderQueue::submitRenderCommands(std::vector<RenderCommand> commands) {
    mRenderCommands.reserve(mRenderCommands.size() + commands.size());

    for (RenderCommand& command : commands) { // Removed 'const' to allow std::move
        std::visit(overloaded{
            [&](DrawCommand& drawCmd) {
                size_t testHash = hash(drawCmd);

                // Check if we have a valid index to merge into
                if (mLastDrawCommandIdx != -1 &&
                    mLastDrawCommandIdx < mRenderCommands.size() &&
                    mLastDrawCommandHash == testHash)
                {
                    auto& targetDrawCmd = std::get<DrawCommand>(mRenderCommands[mLastDrawCommandIdx]);
                    if (drawCmd.isInstanceOf(targetDrawCmd)) {
                        mergeDrawCommand(&targetDrawCmd, drawCmd);
                        return; // Successfully merged, do not push to vector
                    }
                }

                // Create new draw command by moving it
                mRenderCommands.push_back(std::move(command));
                mLastDrawCommandIdx = mRenderCommands.size() - 1; // Store index, completely safe!
                mLastDrawCommandHash = testHash;
            },
            [&](StateChangeCommand& stateCmd) {
                if (hash(stateCmd) != hash(mLastState)) {
                    mLastState = stateCmd;
                    mLastDrawCommandIdx = -1; // Invalidate index
                    mLastDrawCommandHash = 0;

                    mRenderCommands.push_back(std::move(command));
                }
            },
            [&](ClearCommand& clearCmd) {
                mLastDrawCommandHash = 0;
                mLastDrawCommandIdx = -1; // Invalidate index
                mRenderCommands.push_back(std::move(command));
            }
        }, command);
    }
}



const std::vector<RenderCommand>& RenderQueue::getRenderCommands() const{
    return mRenderCommands;
}

void RenderQueue::clear() {
    mRenderCommands.clear();
    mLastDrawCommandIdx = -1;
    mLastDrawCommandHash = 0;
    mLastState = {};
}

void RenderQueue::mergeDrawCommand(DrawCommand* into, const DrawCommand& toMerge) const {
    if (!into) return; // Safety check

    // Append all elements from toMerge.instanceUniforms to the end of into->instanceUniforms
    // TODO maybe optimize to use move instead
    into->instances.insert(
        into->instances.end(),
        std::make_move_iterator(toMerge.instances.begin()),
        std::make_move_iterator(toMerge.instances.end())
    );
}
