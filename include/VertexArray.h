//
// Created by joost on 4/9/26.
//

#ifndef GCVCPROJECT_VERTEXARRAY_H
#define GCVCPROJECT_VERTEXARRAY_H
#include <memory>

#include "Buffer/Buffer.h"
struct VertexAttribute {
    int location;
    int componentCount;
    size_t componentSize;
    unsigned int type;
};

class VertexArray {
private:
    unsigned int mId;
    Buffer mElementBuffer; // A VAO can only have one element buffer so we just make this object own its EBO
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    void addVertexBuffer(Buffer &buffer, const std::vector<VertexAttribute> &layout);
    Buffer& getElementBuffer();
    void bind();
private:
};
#endif //GCVCPROJECT_VERTEXARRAY_H