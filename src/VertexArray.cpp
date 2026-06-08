//
// Created by joost on 4/9/26.
//


#include "VertexArray.h"

#include <stdexcept>

VertexArray::VertexArray()
    : mId(0), mElementBuffer(GL_ELEMENT_ARRAY_BUFFER)
{
    glGenVertexArrays(1, &mId);

    if (mId == 0) {
        throw std::runtime_error("Could not create Vertex Array!");
    }

    // Bind the element buffer to this VAO
    bind();
    mElementBuffer.bind();
}

VertexArray::~VertexArray() {
    if (mId != 0)
        glDeleteVertexArrays(1, &mId);
}

VertexArray::VertexArray(VertexArray&& other) noexcept
    : mId(other.mId), mElementBuffer(std::move(other.mElementBuffer))
{
    other.mId = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (this != &other) {
        if (mId != 0)
            glDeleteVertexArrays(1, &mId);
        mId            = other.mId;
        mElementBuffer = std::move(other.mElementBuffer);
        other.mId      = 0;
    }
    return *this;
}

void VertexArray::addVertexBuffer(Buffer &buffer, const std::vector<VertexAttribute> &layout) {
    bind();
    buffer.bind();

    size_t stride = 0;
    for (const auto &attr : layout) {
        stride += attr.componentSize;
    }

    size_t offset = 0;
    for (const auto &attr : layout) {
        glVertexAttribPointer(
            attr.location,
            attr.componentCount,
            attr.type,
            GL_FALSE,
            stride,
            (const void*)offset
        );
        glEnableVertexAttribArray(attr.location);
        offset += attr.componentSize;
    }
}

Buffer &VertexArray::getElementBuffer() {
    return mElementBuffer;
}

void VertexArray::bind() {
    glBindVertexArray(mId);
}
