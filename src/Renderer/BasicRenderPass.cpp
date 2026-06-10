//
// Created by joost on 6/9/26.
//

#include "Renderer/BasicRenderPass.h"



void BasicRenderPass::drawObject(std::shared_ptr<IDrawable> drawable, ShaderProgram& currentProgram) {
    drawable->getMaterial().readyMaterial(currentProgram);
    drawable->draw(currentProgram);
}
