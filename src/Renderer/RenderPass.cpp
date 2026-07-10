#include "Renderer/RenderPass.hpp"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderQueue.h"
#include <utility>
#include <vector>


void RenderPass::addToRenderQueue(std::vector<RenderCommand> command){
    mQueue.submitRenderCommands(std::move(command));
}

const RenderQueue& RenderPass::getRenderQueue() const{
    return mQueue;
}


void RenderPass::setProjectionType(ProjectionType type){
    mProjectionType = std::move(type);
}

ProjectionType RenderPass::getProjectionType() const{
    return mProjectionType;
}


void RenderPass::setCameraType(CameraType type) {
    mCamerType = type;
}

CameraType RenderPass::getCameraType() const{
    return mCamerType;
}

void RenderPass::clearRenderQueue(){
    mQueue.clear();
}
