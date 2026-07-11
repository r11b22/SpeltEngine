//
// Created by joost on 4/12/26.
//

#include "Mesh/Mesh.h"
#include "Buffer/Buffer.h"


Mesh::Mesh(std::string name)
    : Asset(name), mVertexBuffer(GL_ARRAY_BUFFER), mVAO()
{
    mVAO.addVertexBuffer(mVertexBuffer, {
        VertexAttribute{0, 3, 3 * sizeof(float), GL_FLOAT},
        VertexAttribute{1, 3, 3 * sizeof(float), GL_FLOAT},
        VertexAttribute{2, 2, 2 * sizeof(float), GL_FLOAT}
    });
}

void Mesh::draw(ShaderProgram &shaderProgram) {
    mVAO.bind();
    glDrawElements(GL_TRIANGLES, mVertexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::drawInstanced(ShaderProgram &shaderProgram, int instanceCount) {
    mVAO.bind();
    glDrawElementsInstanced(
        GL_TRIANGLES,
        mVertexCount,
        GL_UNSIGNED_INT,
        nullptr,
        instanceCount
        );
}

void Mesh::setVertices(std::vector<float> vertices) {
    // Bind the VAO to make sure the changes get pushed to the VAO
    mVAO.bind();
    mVertexBuffer.setDataF(vertices);
}

void Mesh::setIndices(std::vector<unsigned int> indices) {
    // Bind the VAO to make sure the changes get pushed to the VAO
    mVAO.bind();
    mVAO.getElementBuffer().setDataUI(indices);
    mVertexCount = indices.size();
}
