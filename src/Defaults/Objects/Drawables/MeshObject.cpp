//
// Created by joost on 6/2/26.
//

#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Material/Material.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshReference.hpp"
#include "Renderer/IDrawable.h"
#include "Renderer/Instancing/InstanceData.hpp"
#include "Renderer/RenderCommand.h"

MeshObject::MeshObject(const std::string &name, MeshReference mesh, Material material)
    : TransformableObject(name), mMesh(mesh), mMaterial(material)
{
}

std::vector<RenderPassCommands> MeshObject::getRenderCommands() {
    StateChangeCommand defaultState;

    DrawCommand command;

    command.material = mMaterial;
    command.mesh = mMesh;
    command.shaderName = mShader;

    glm::mat4 transformationMatrix = getTransformationMatrix();
    InstanceData instance;

    instance.addUniform({"uModelMatrix", transformationMatrix});

    command.instances.push_back(std::move(instance));

    return {RenderPassCommands{mRenderPass, {defaultState, command}}};
}

void MeshObject::setShader(std::string shader) {
    mShader = std::move(shader);
}

void MeshObject::setMesh(MeshReference mesh){
    mMesh = mesh;
}

void MeshObject::setMaterial(Material material){
    mMaterial = material;
}

void MeshObject::setRenderPass(size_t pass){
    mRenderPass = pass;
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

size_t MeshObject::getRenderPass() const {
    return mRenderPass;
}
