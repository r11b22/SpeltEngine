//
// Created by joost on 6/4/26.
//

#include "Renderer/RenderQueue.h"
#include <algorithm>

void RenderQueue::addDrawable(std::shared_ptr<IDrawable> drawable) {
    std::string shaderName = drawable->getShaderProgramName();

    mDrawables[shaderName].push_back(drawable);
    mDrawablesOrdered.push_back(std::move(drawable));
}

const std::unordered_map<std::string, std::vector<std::shared_ptr<IDrawable> > > &RenderQueue::getDrawables() const{
    return mDrawables;
}

const std::vector<std::shared_ptr<IDrawable> > &RenderQueue::getDrawablesOrdered() const{
    return mDrawablesOrdered;
}

void RenderQueue::removeDrawable(const std::shared_ptr<IDrawable>& drawable) {
    std::string shaderName = drawable->getShaderProgramName();

    auto& vec = mDrawables[shaderName];
    vec.erase(std::remove(vec.begin(), vec.end(), drawable), vec.end());

    mDrawablesOrdered.erase(std::remove(mDrawablesOrdered.begin(), mDrawablesOrdered.end(), drawable), mDrawablesOrdered.end());
}
