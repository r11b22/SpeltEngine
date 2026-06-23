//
// Created by joost on 6/2/26.
//

#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Mesh/MeshReference.hpp"
#include "Renderer/RenderCommand.h"

MeshObject::MeshObject(const std::string &name, MeshReference mesh, const std::shared_ptr<Material>& material)
    : TransformableObject(name), mMesh(mesh), mMaterial(material)
{
}

std::vector<RenderCommand> MeshObject::getRenderCommands() {
    StateChangeCommand defaultState;

    DrawCommand command;

    command.material = mMaterial;
    command.mesh = mMesh;
    command.shaderName = mShader;

    glm::mat4 transformationMatrix = getTransformationMatrix();
    command.uniforms.push_back({"uModelMatrix", transformationMatrix});

    return {defaultState, command};
}

void MeshObject::setShader(std::string shader) {
    mShader = std::move(shader);
}

std::shared_ptr<Material> MeshObject::getMaterial() {
    return mMaterial;
}

MeshReference MeshObject::getMesh() const {
    return mMesh;
}

std::string MeshObject::getShader() const{
    return mShader;
}
