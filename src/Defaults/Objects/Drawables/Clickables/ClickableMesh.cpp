//
// Created by joost on 5/2/26.
//


#include "Defaults/Objects/Drawables/Clickables/ClickableMesh.h"

#include <iostream>

ClickableMesh::ClickableMesh(Mesh mesh)
    : mMesh(std::move(mesh)), mClickCallback([]{std::cout << "No callback set" << std::endl;})
{

}

void ClickableMesh::draw(ShaderProgram &shaderProgram) {
    mMesh.draw(shaderProgram);
}


std::string ClickableMesh::getShaderProgramName() {
    return mMesh.getShaderProgramName();
}

void ClickableMesh::setClickCallback(std::function<void()> callback) {
    mClickCallback = std::move(callback);
}

void ClickableMesh::onClick() const{
    mClickCallback();
}


