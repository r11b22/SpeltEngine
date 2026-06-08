//
// Created by joost on 6/2/26.
//

#include "Defaults/Objects/Drawables/Clickables/ClickableObject.h"

ClickableObject::ClickableObject(const std::string &name, Mesh mesh)
    : TransformableObject(name), mClickableMesh(std::move(mesh))
{
}

void ClickableObject::draw(ShaderProgram &shaderProgram) {
    shaderProgram.setUniformMat4x4("uModelMatrix", getTransformationMatrix());
    mClickableMesh.draw(shaderProgram);
}

std::string ClickableObject::getShaderProgramName() {
    return mClickableMesh.getShaderProgramName();
}

void ClickableObject::setClickCallback(std::function<void()> callback) {
    mClickableMesh.setClickCallback(std::move(callback));
}

void ClickableObject::onClick() const {
    mClickableMesh.onClick();
}
