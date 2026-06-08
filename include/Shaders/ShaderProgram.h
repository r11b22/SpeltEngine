//
// Created by joost on 4/9/26.
//

#ifndef GCVCPROJECT_SHADERPROGRAM_H
#define GCVCPROJECT_SHADERPROGRAM_H
#include <filesystem>
#include <vector>
#include <memory>

#include "Buffer/Buffer.h"
#include "glad/glad.h"
#include "glm/mat4x4.hpp"

class PointLight;

class ShaderProgram {
private:
    unsigned int mId;
    std::vector<unsigned int> attachedShaders;

    std::unordered_map<std::string, GLuint> mUsedBufferBindings = {};
public:
    ShaderProgram();
    ~ShaderProgram();

    // --- Move Semantics ---
    ShaderProgram(ShaderProgram&& other) noexcept;                // Move constructor
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;     // Move assignment

    // --- Disable Copy Semantics ---
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    void addShader(const char* source, GLenum type);
    void link();

    void use();

    /* Upload a matrix to the gpu
     * warning: this function does not call use on the shader program, make sure to call use() before calling this function
     */
    void setUniformMat4x4(const std::string& name, const glm::mat4& matrix);

    void setUniformVec3(const std::string& name, const glm::vec3& value);

    /* Upload an Integer to the gpu
     * warning: this function does not call use on the shader program, make sure to call use() before calling this function
     */
    void setUniformInt(const std::string& name, int value);

    /* Upload an Integer to the gpu
     * warning: this function does not call use on the shader program, make sure to call use() before calling this function
     */
    void setUniformUInt(const std::string& name, unsigned int value);

    /* Upload an Integer to the gpu
    * warning: this function does not call use on the shader program, make sure to call use() before calling this function
    */
    void setUniformBool(const std::string& name, bool value);

    void setUniformFloat(const std::string& name, float value);

    void bindBufferToUniform(const std::string& name, Buffer& buff);
private:
    GLint getUniformLocation(const std::string& name) const;
    GLint getResourceIndex(const std::string& name) const;
};

#endif //GCVCPROJECT_SHADERPROGRAM_H