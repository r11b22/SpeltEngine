//
// Created by joost on 4/9/26.
//


#include "Buffer/Buffer.h"
#include "Buffer/BufferMap.h"
#include "Error/Panic.hpp"
#include "Error/Result.hpp"
#include "OpenGL/BindTracker.hpp"
#include "Window.h"

#include <cstddef>
#include <stdexcept>

namespace Spelt {


    Buffer::Buffer(GLenum type, GLenum usage)
        : mType(type), mId(0), mUsageType(usage)
    {
        glGenBuffers(1, &mId);

        if (mId == 0) {
            fatalPanic("Failed to create new buffer!");
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

    size_t Buffer::getSize() const {
        return mSize;
    }

    size_t Buffer::getReserved() const {
        return mReserved;
    }

    void Buffer::bind(){
        if (!BindTracker::getInstance().isBound(BindType::Buffer, mId)){
            glBindBuffer(mType, mId);
            BindTracker::getInstance().bind(BindType::Buffer, mId);
        }
    }

    void Buffer::bindCopyRead() {
        glBindBuffer(GL_COPY_READ_BUFFER, mId);
    }

    void Buffer::bindCopyWrite() {
        glBindBuffer(GL_COPY_WRITE_BUFFER, mId);
    }

    void Buffer::bindBase(int location) {
        if (!BindTracker::getInstance().isBound(BindType::Buffer, mId)){
            BindTracker::getInstance().bind(BindType::Buffer, mId);
        }
        glBindBufferBase(mType, location, mId);
    }

    void Buffer::bindBaseShaderStorage(int location) {
        if (!BindTracker::getInstance().isBound(BindType::Buffer, mId)){
            BindTracker::getInstance().bind(BindType::Buffer, mId);
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, mId);
    }

    Result<void, BufferError> Buffer::copy(Buffer& toCopy, size_t writeOffset, size_t readOffset){
        if (this == &toCopy) return Error{BufferError::InvalidArgument};

        if(writeOffset > getReserved()) return Error{BufferError::OffsetOutOfBounds};
        if(readOffset > toCopy.getSize()) return Error{BufferError::OffsetOutOfBounds};

        if(getReserved() - writeOffset < toCopy.getSize() - readOffset) return Error{BufferError::NotEnoughSpace};

        bindCopyWrite();
        toCopy.bindCopyRead();

        glCopyBufferSubData(
            GL_COPY_READ_BUFFER,
            GL_COPY_WRITE_BUFFER,
            readOffset, // readOffset
            writeOffset, // writeOffset
            toCopy.getSize() - readOffset // size in bytes
        );

        return Success{};
    }

    Result<void, BufferError> Buffer::append(Buffer& toCopy){
        return copy(toCopy, getSize(), 0);
    }

    Result<void, BufferError> Buffer::reserve(size_t newCapacity) {
        if (newCapacity <= mReserved) {
            return Success{};
        }

        if (mMapped) {
            return Error{BufferError::AlreadyMapped};
        }

        Buffer tempBuffer(mType, mUsageType);

        tempBuffer.bind();
        glBufferData(tempBuffer.mType, newCapacity, nullptr, tempBuffer.mUsageType);
        tempBuffer.mReserved = newCapacity;

        if (mId != 0 && mSize > 0) {
            auto copyResult = tempBuffer.copy(*this, 0, 0);
            if (!copyResult.isValue()) {
                return copyResult;
            }
        }

        size_t currentSize = mSize;

        *this = std::move(tempBuffer);

        mSize = currentSize;

        return Success{};
    }

    void Buffer::setData(const void* data, size_t bytes) {
        bind();
        mReserved = bytes;
        glBufferData(mType, bytes, data, mUsageType);
        mSize = bytes;
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
        mSize = bytes;
        glBufferSubData(mType, 0, bytes, data);
        mReserved = bytes;
    }


    // TODO track size
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
