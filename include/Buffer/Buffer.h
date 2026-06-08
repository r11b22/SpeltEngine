//
// Created by joost on 4/9/26.
//

#ifndef GCVCPROJECT_BUFFER_H
#define GCVCPROJECT_BUFFER_H
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"

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

    // float setters
    void setDataF(std::vector<float> data);
    void setDataF(float* data, size_t len);

    // int setters
    void setDataI(std::vector<int> data);
    void setDataI(int *data, size_t len);

    // uint setters
    void setDataUI(std::vector<unsigned int> data);
    void setDataUI(unsigned int *data, size_t len);

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
    void setDataVec3(std::vector<glm::vec3> data);
    /**
     * Uploades a list of vec3's to the buffer
     * Warning: vec3 arrays are padded to vec4 arrays, make sure to use vec4's in your shader
     * @param data the pointer to the vec3 array
     * @param len the length of the array
     */
    void setDataVec3(glm::vec3 *data, size_t len);

    /**
     * Creates a mapping for the cpu to read from the gpu buffer
     * @return
     */
    BufferMap<float> mapDataFloat(GLenum accessType);

    /**
     * Creates a mapping for the cpu to read from the gpu buffer
     * @return
     */
    BufferMap<glm::vec4> mapDataVec4(GLenum accessType);

    /**
     * Unmaps all memory maps made for this buffer
     * Warning: do not call this from non engine code
     */
    void unmap();
private:
};

#endif //GCVCPROJECT_BUFFER_H