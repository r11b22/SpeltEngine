//
// Created by joost on 4/12/26.
//

#include "Mesh.h"

#include <iostream>

#include "ModelLoader.h"

Mesh::Mesh()
    : mVertexBuffer(GL_ARRAY_BUFFER), mVAO()
{
    mVAO.addVertexBuffer(mVertexBuffer, {
        VertexAttribute{0, 3, 3 * sizeof(float), GL_FLOAT},
        VertexAttribute{1, 3, 3 * sizeof(float), GL_FLOAT},
        VertexAttribute{2, 2, 2 * sizeof(float), GL_FLOAT}
    });
}

void Mesh::draw(ShaderProgram &shaderProgram) {
    shaderProgram.setUniformFloat("uSpecularStrength", 0.5f);
    mVAO.bind();
    shaderProgram.setUniformInt("uTexture", 0);
    if (mTexture.has_value()) {
        mTexture.value().bind(0);
    }
    glDrawElements(GL_TRIANGLES, mVertexCount, GL_UNSIGNED_INT, 0);

}

std::string Mesh::getShaderProgramName() {
    return mShader;
}

void Mesh::setVertices(std::vector<float> vertices) {
    mVertexBuffer.setDataF(vertices);


}

void Mesh::setIndices(std::vector<unsigned int> indices) {
    mVAO.getElementBuffer().setDataUI(indices);
    mVertexCount = indices.size();
}




void Mesh::setShader(std::string shader) {
    mShader = shader;
}

void Mesh::setTexture(const TextureData& texData) {
    mTexture = Texture(texData.texData, texData.texWidth, texData.texHeight, texData.channelCount);
}
