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

    for (const RenderCommand& command : commands){
        std::visit(overloaded{
            [&](const DrawCommand& drawCmd) {
                size_t testHash = hash(drawCmd);
                if(mLasDrawCommand != nullptr && mLastDrawCommandHash == testHash && drawCmd.isInstanceOf(*mLasDrawCommand)){
                    // merge draw command with the previous one
                    mergeDrawCommand(mLasDrawCommand, drawCmd);
                }else{
                    // Create new drawcommand
                    mRenderCommands.push_back(std::move(drawCmd));
                    mLasDrawCommand = &std::get<DrawCommand>(mRenderCommands.back());
                    mLastDrawCommandHash = testHash;
                }
            },
            [&](const StateChangeCommand& stateCmd) {
                // TODO cache hash
                if (hash(stateCmd) != hash(mLastState)){
                    mLastState = stateCmd;
                    // Get ready for a new draw command
                    mLasDrawCommand = nullptr;
                    mLastDrawCommandHash = 0;

                    mRenderCommands.push_back(stateCmd);
                }
            },
            [&](const ClearCommand& clearCmd) {
                mLastDrawCommandHash = 0;
                mLasDrawCommand = nullptr;
                mRenderCommands.push_back(clearCmd);
            }
        }, command);
    }
}



const std::vector<RenderCommand>& RenderQueue::getRenderCommands() const{
    return mRenderCommands;
}

void RenderQueue::clear() {
    mRenderCommands.clear();
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
