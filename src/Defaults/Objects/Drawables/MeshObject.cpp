//
// Created by joost on 6/2/26.
//

#include "Defaults/Objects/Drawables/MeshObject.h"

MeshObject::MeshObject(const std::string &name, const std::shared_ptr<Mesh>& mesh)
    : TransformableObject(name), mMesh(mesh)
{
}

void MeshObject::draw(ShaderProgram &shaderProgram) {
    shaderProgram.setUniformMat4x4("uModelMatrix", getTransformationMatrix());
    mMesh->draw(shaderProgram);
}

std::string MeshObject::getShaderProgramName() {
    return mMesh->getShaderProgramName();
}

Material & MeshObject::getMaterial() {
    return mMesh->getMaterial();
}

void MeshObject::setIndices(std::vector<unsigned int> indices) {
    mMesh->setIndices(std::move(indices));
}

void MeshObject::setVertices(std::vector<float> vertices) {
    mMesh->setVertices(std::move(vertices));
}

void MeshObject::setShader(std::string shader) {
    mMesh->setShader(shader);
}

void MeshObject::setTexture(const TextureData &texData) {
    mMesh->setTexture(texData);
}
