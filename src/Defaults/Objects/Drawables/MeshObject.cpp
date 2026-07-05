//
// Created by joost on 6/2/26.
//

#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshReference.hpp"
#include "Renderer/Instancing/InstanceData.hpp"
#include "Renderer/RenderCommand.h"

MeshObject::MeshObject(const std::string &name, MeshReference mesh, Material material)
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
    InstanceData instance;

    instance.addUniform({"uModelMatrix", transformationMatrix});

    command.instances.push_back(std::move(instance));

    return {defaultState, command};
}

void MeshObject::setShader(std::string shader) {
    mShader = std::move(shader);
}

void MeshObject::setMesh(MeshReference mesh){
    mMesh = mesh;
}

Material MeshObject::getMaterial() {
    return mMaterial;
}

MeshReference MeshObject::getMesh() const {
    return mMesh;
}

std::string MeshObject::getShader() const{
    return mShader;
}
