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
    mVAO.bind();
    glDrawElements(GL_TRIANGLES, mVertexCount, GL_UNSIGNED_INT, 0);
}

void Mesh::setVertices(std::vector<float> vertices) {
    mVertexBuffer.setDataF(vertices);
}

void Mesh::setIndices(std::vector<unsigned int> indices) {
    mVAO.getElementBuffer().setDataUI(indices);
    mVertexCount = indices.size();
}




