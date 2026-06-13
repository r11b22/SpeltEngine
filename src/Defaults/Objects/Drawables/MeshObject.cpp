//
// Created by joost on 6/2/26.
//

#include "Defaults/Objects/Drawables/MeshObject.h"

MeshObject::MeshObject(const std::string &name, const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material)
    : TransformableObject(name), mMesh(mesh), mMaterial(material)
{
}

std::vector<RenderCommand> MeshObject::getRenderCommands() {
    RenderCommand command;

    command.material = mMaterial;
    command.renderable = mMesh;
    command.shaderName = mShader;

    glm::mat4 transformationMatrix = getTransformationMatrix();
    command.uniforms.push_back({"uModelMatrix", transformationMatrix});

    return {command};
}


void MeshObject::setIndices(std::vector<unsigned int> indices) {
    mMesh->setIndices(std::move(indices));
}

void MeshObject::setVertices(std::vector<float> vertices) {
    mMesh->setVertices(std::move(vertices));
}

void MeshObject::setShader(std::string shader) {
    mShader = std::move(shader);
}

std::shared_ptr<Mesh> MeshObject::getMesh() {
    return mMesh;
}

std::shared_ptr<Material> MeshObject::getMaterial() {
    return mMaterial;
}

std::string MeshObject::getShader() {
    return mShader;
}
