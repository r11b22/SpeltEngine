//
// Created by joost on 4/12/26.
//

#ifndef GCVCPROJECT_MESH_H
#define GCVCPROJECT_MESH_H
#include <optional>

#include "Buffer/Buffer.h"
#include "Renderer/IDrawable.h"
#include "ModelLoader.h"
#include "Texture.h"
#include "Transform/Transform.h"
#include "VertexArray.h"

class Mesh : public IRenderable {
private:
    Buffer mVertexBuffer;
    VertexArray mVAO;

    int mVertexCount;


public:
    Mesh();
    ~Mesh() override = default;

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept = default;
    Mesh& operator=(Mesh&& other) noexcept = default;


    void draw(ShaderProgram& shaderProgram) override;

    void setVertices(std::vector<float> vertices);
    void setIndices(std::vector<unsigned int> indices);

private:
};

#endif //GCVCPROJECT_MESH_H