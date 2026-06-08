//
// Created by joost on 4/9/26.
//


#include "Buffer/Buffer.h"
#include "Buffer/BufferMap.h"

#include <stdexcept>

Buffer::Buffer(GLenum type, GLenum usage)
    : mType(type), mId(0), mUsageType(usage)
{
    glGenBuffers(1, &mId);

    if (mId == 0) {
        throw std::runtime_error("Failed to create new buffer!");
    }
}

Buffer::~Buffer() {
    if (mId != 0)
        glDeleteBuffers(1, &mId);
}

Buffer::Buffer(Buffer&& other) noexcept
    : mId(other.mId), mType(other.mType)
{
    other.mId = 0;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        if (mId != 0)
            glDeleteBuffers(1, &mId);
        mId   = other.mId;
        mType = other.mType;
        other.mId = 0;
    }
    return *this;
}

void Buffer::bind(){
    glBindBuffer(mType, mId);
}

void Buffer::bindBase(int location) {
    glBindBufferBase(mType, location, mId);
}

void Buffer::setDataF(std::vector<float> data) {
    setDataF(&data[0], data.size()); // vector black magic
}

void Buffer::setDataF(float* data, size_t len) {
    bind();
    glBufferData(mType, len * sizeof(float), data, mUsageType);
}

void Buffer::setDataI(std::vector<int> data) {
    setDataI(&data[0], data.size());
}

void Buffer::setDataI(int *data, size_t len) {
    bind();
    glBufferData(mType, len * sizeof(int), data, mUsageType);
}

void Buffer::setDataUI(std::vector<unsigned int> data) {
    setDataUI(&data[0], data.size());
}

void Buffer::setDataUI(unsigned int *data, size_t len) {
    bind();
    glBufferData(mType, len * sizeof(unsigned int), data, mUsageType);
}

void Buffer::setDataB(const std::vector<bool>& data) {
    std::vector<int> intData;
    intData.resize(data.size());

    std::copy(data.begin(), data.end(), intData.begin());

    setDataI(intData.data(), intData.size());
}

void Buffer::setDataB(const bool *data, size_t len) {
    std::vector<int> intData;
    intData.reserve(len);

    for (size_t i = 0; i < len; ++i) {
        intData.push_back(data[i] ? 1 : 0);
    }

    setDataI(intData.data(), len);
}

void Buffer::setDataVec3(std::vector<glm::vec3> data) {
    // Pass the pointer to the first element and the count of vectors
    setDataVec3(data.data(), data.size());
}

void Buffer::setDataVec3(glm::vec3 *data, size_t len) {
    // 1. Create a temporary staging vector of vec4s to handle std430 16-byte alignment
    std::vector<glm::vec4> alignedData(len);

    for (size_t i = 0; i < len; ++i) {
        // Copy x, y, z from your vec3. The w component serves as 4-byte padding (set to 0.0 or 1.0)
        alignedData[i] = glm::vec4(data[i], 0.0f);
    }

    // 2. Bind the current buffer instance
    bind();

    // 3. Upload the aligned vec4 data instead of the raw vec3 data
    // Total bytes = number of elements * 16 bytes (sizeof(glm::vec4))
    glBufferData(mType, len * sizeof(glm::vec4), alignedData.data(), mUsageType);
}

BufferMap<float> Buffer::mapDataFloat(GLenum accessType) {
    if (mMapped) {
        throw std::runtime_error("Can not map an already mapped Buffer!");
    }

    bind();
    float* ptr = (float*)glMapBuffer(mType, accessType);
    mMapped = true;

    return BufferMap<float>(ptr, this);
}

BufferMap<glm::vec4> Buffer::mapDataVec4(GLenum accessType) {
    if (mMapped) {
        throw std::runtime_error("Can not map an already mapped Buffer!");
    }

    bind();
    glm::vec4* ptr = (glm::vec4*)glMapBuffer(mType, accessType);
    mMapped = true;

    return BufferMap<glm::vec4>(ptr, this);
}

void Buffer::unmap() {
    if (!mMapped) {
        throw std::runtime_error("Can not unmap a non mapped Buffer!");
    }

    bind();
    mMapped = false;
    glUnmapBuffer(mType);
}
