//
// Created by joost on 6/9/26.
//

#include "Renderer/BasicPerDrawableRenderPass.h"

void BasicPerDrawableRenderPass::drawObject(std::shared_ptr<IDrawable> drawable, ShaderProgram& currentProgram) {
    drawable->getMaterial().readyMaterial(currentProgram);
    drawable->draw(currentProgram);
}
