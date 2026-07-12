//
// Created by joost on 4/9/26.
//


#include "Buffer/Buffer.h"
#include "Buffer/BufferMap.h"
#include "Error/Result.hpp"
#include "Window.h"

#include <stdexcept>

namespace Spelt {


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
        : mId(other.mId), mType(other.mType), mUsageType(other.mUsageType), mMapped(other.mMapped)
    {
        other.mId = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept {
        if (this != &other) {
            if (mId != 0)
                glDeleteBuffers(1, &mId);
            mId   = other.mId;
            mType = other.mType;
            mMapped = other.mMapped;
            mUsageType = other.mUsageType;
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

    void Buffer::setData(const void* data, size_t bytes) {
        bind();
        glBufferData(mType, bytes, data, mUsageType);
    }

    void Buffer::setDataF(const std::vector<float>& data) {
        setDataF(data.data(), data.size()); // vector black magic
    }

    void Buffer::setDataF(const float* data, size_t len) {
        setData(data, len * sizeof(float));
    }

    void Buffer::setDataI(const std::vector<int>& data) {
        setDataI(data.data(), data.size());
    }

    void Buffer::setDataI(const int *data, size_t len) {
        setData(data, len * sizeof(int));
    }

    void Buffer::setDataUI(const std::vector<unsigned int>& data) {
        setDataUI(data.data(), data.size());
    }

    void Buffer::setDataUI(const unsigned int *data, size_t len) {
        setData(data, len * sizeof(unsigned int));
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

    void Buffer::setDataVec3(const std::vector<glm::vec3>& data) {
        // Pass the pointer to the first element and the count of vectors
        setDataVec3(data.data(), data.size());
    }

    void Buffer::setDataVec3(const glm::vec3 *data, size_t len) {
        // 1. Create a temporary staging vector of vec4s to handle std430 16-byte alignment
        std::vector<glm::vec4> alignedData(len);

        for (size_t i = 0; i < len; ++i) {
            // Copy x, y, z from your vec3. The w component serves as 4-byte padding (set to 0.0 or 1.0)
            alignedData[i] = glm::vec4(data[i], 0.0f);
        }

        // 2. Bind the current buffer instance
        // 3. Upload the aligned vec4 data instead of the raw vec3 data
        // Total bytes = number of elements * 16 bytes (sizeof(glm::vec4))
        setData(alignedData.data(), len * sizeof(glm::vec4));
    }

    void Buffer::setDataAndOrphan(const void* data, size_t bytes){
        bind();

        glBufferData(mType, bytes, nullptr, mUsageType);
        glBufferSubData(mType, 0, bytes, data);
    }

    Result<BufferMap<float>, BufferError> Buffer::mapDataFloat(GLenum accessType) {
        if (mMapped) {
            return Result<BufferMap<float>, BufferError>::createError(BufferError::AlreadyMapped);
        }

        bind();
        float* ptr = (float*)glMapBuffer(mType, accessType);
        mMapped = true;

        return Result<BufferMap<float>, BufferError>::createValue(BufferMap<float>(ptr, this));
    }

    Result<BufferMap<glm::vec4>, BufferError> Buffer::mapDataVec4(GLenum accessType) {
        if (mMapped) {
            return Result<BufferMap<glm::vec4>, BufferError>::createError(BufferError::AlreadyMapped);
        }

        bind();
        glm::vec4* ptr = (glm::vec4*)glMapBuffer(mType, accessType);
        mMapped = true;

        return Result<BufferMap<glm::vec4>, BufferError>::createValue(BufferMap<glm::vec4>(ptr, this));
    }

    Result<void, BufferError> Buffer::unmap() {
        if (!mMapped) {
            return Result<void, BufferError>::createError(BufferError::NotMapped);
        }

        bind();
        mMapped = false;
        glUnmapBuffer(mType);

        return Result<void, BufferError>::createValue();
    }
}
