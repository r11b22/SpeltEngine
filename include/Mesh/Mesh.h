//
// Created by joost on 4/12/26.
//

#pragma once

#include "Asset/Asset.hpp"
#include "Buffer/Buffer.h"
#include "Renderer/IRenderable.h"
#include "Shader/ShaderProgram.h"
#include "VertexArray.h"

namespace Spelt {
    class Mesh : public IRenderable, public Asset {
    private:
        Buffer mVertexBuffer;
        VertexArray mVAO;

        int mVertexCount;


    public:
        Mesh(std::string name);
        ~Mesh() override = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept = default;
        Mesh& operator=(Mesh&& other) noexcept = default;


        void draw(ShaderProgram& shaderProgram) override;
        void drawInstanced(ShaderProgram& shaderProgram, int instanceCount) override;

        void setVertices(std::vector<float> vertices);
        void setIndices(std::vector<unsigned int> indices);

        unsigned int getVertexCount() const;


        /**
         * Binds the vertex buffer as a shader storage buffer
         */
        void bindBaseShaderStorageVertexBuffer(int location);
        /**
         * Binds the index buffer as a shader storage buffer
         */
        void bindBaseShaderStorageIndexBuffer(int location);


    private:
    };

}
