//
// Created by joost on 4/9/26.
//

#pragma once
#include <vector>

#include "Error/Result.hpp"
#include "OpenGL/BindTracker.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"

namespace Spelt {
    enum class BufferError{
        AlreadyMapped,
        NotMapped
    };

    template <typename T>
    class BufferMap;

    class Buffer {
    private:
        unsigned int mId;
        GLenum mType;

        GLenum mUsageType;

        bool mMapped = false;
    public:
        Buffer(GLenum type, GLenum usage = GL_STATIC_DRAW);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept;
        Buffer& operator=(Buffer&& other) noexcept;

        void bind();
        void bindBase(int location);
        void bindBaseShaderStorage(int location);

        // float setters
        void setDataF(const std::vector<float>& data);
        void setDataF(const float* data, size_t len);

        // int setters
        void setDataI(const std::vector<int>& data);
        void setDataI(const int *data, size_t len);

        // uint setters
        void setDataUI(const std::vector<unsigned int>& data);
        void setDataUI(const unsigned int *data, size_t len);

        // bool setters
        /**
        * Put a list of booleans into the buffer
        * @warning Due to the padding of
        * @param data
        */
        void setDataB(const std::vector<bool>& data);
        void setDataB(const bool *data, size_t len);

        /**
        * Uploades a list of vec3's to the buffer
        * Warning: vec3 arrays are padded to vec4 arrays, make sure to use vec4's in your shader
        * @param data the vector of vec3's
        */
        void setDataVec3(const std::vector<glm::vec3>& data);

        /**
        * Uploades a list of vec3's to the buffer
        * Warning: vec3 arrays are padded to vec4 arrays, make sure to use vec4's in your shader
        * @param data the pointer to the vec3 array
        * @param len the length of the array
        */
        void setDataVec3(const glm::vec3 *data, size_t len);

        /**
        * Uploades data to the buffer. Size is given in bytes
        * Orphans the buffer before uploading
        */
        void setDataAndOrphan(const void* data, size_t bytes);

        /**
        * Creates a mapping for the cpu to read from the gpu buffer
        * @return
        */
        Result<BufferMap<float>, BufferError> mapDataFloat(GLenum accessType);

        /**
        * Creates a mapping for the cpu to read from the gpu buffer
        * @return
        */
        Result<BufferMap<glm::vec4>, BufferError> mapDataVec4(GLenum accessType);

        /**
        * Unmaps all memory maps made for this buffer
        * Warning: do not call this from non engine code
        */
        Result<void, BufferError> unmap();
    private:
        void setData(const void* data, size_t bytes);
    };

}
