//
// Created by joost on 6/4/26.
//

#include "Renderer/RenderQueue.h"
#include "Hashing/Hashing.hpp"
#include "Renderer/RenderCommand.h"
#include <cstddef>
#include <iterator>
#include <vector>

void RenderQueue::submitRenderCommands(std::vector<RenderCommand> commands) {
    mRenderCommands.reserve(mRenderCommands.size() + commands.size());

    mRenderCommands.insert(mRenderCommands.end(), std::make_move_iterator(commands.begin()), std::make_move_iterator(commands.end()));
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

void RenderQueue::flattenCommands() {
    std::vector<RenderCommand> flatCommands = {};
    flatCommands.reserve(mRenderCommands.size());

    StateChangeCommand currentState{};
    size_t currentDrawCommandHash = 0;
    DrawCommand* currentDrawCommand = nullptr;
    for (const RenderCommand& command : mRenderCommands){
        std::visit(overloaded{
            [&](const DrawCommand& drawCmd) {
                size_t testHash = hash(drawCmd);
                if(currentDrawCommand != nullptr && currentDrawCommandHash == testHash && drawCmd.isInstanceOf(*currentDrawCommand)){
                    // merge draw command with the previous one
                    mergeDrawCommand(currentDrawCommand, drawCmd);
                }else{
                    // Create new drawcommand
                    flatCommands.push_back(std::move(drawCmd));
                    currentDrawCommand = &std::get<DrawCommand>(flatCommands.back());
                    currentDrawCommandHash = testHash;
                }
            },
            [&](const StateChangeCommand& stateCmd) {
                // TODO cache hash
                if (hash(stateCmd) != hash(currentState)){
                    currentState = stateCmd;
                    // Get ready for a new draw command
                    currentDrawCommand = nullptr;
                    currentDrawCommandHash = 0;

                    flatCommands.push_back(stateCmd);
                }
            },
            [&](const ClearCommand& clearCmd) {
                currentDrawCommandHash = 0;
                currentDrawCommand = nullptr;
                flatCommands.push_back(clearCmd);
            }
        }, command);
    }

    mRenderCommands = std::move(flatCommands);
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
