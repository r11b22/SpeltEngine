//
// Created by joost on 4/9/26.
//

#include "../../include/Shaders/ShaderProgram.h"
#include <stdexcept>

ShaderProgram::ShaderProgram()
{
    mId = glCreateProgram();

    if (mId == 0) {
        throw std::runtime_error("Could create new shaders program!");
    }
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(mId);
}

// Move Constructor
ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : mId(other.mId), attachedShaders(std::move(other.attachedShaders))
{
    // Steal the ID, then reset the donor so its destructor won't free the OpenGL asset
    other.mId = 0;
}

// Move Assignment Operator
ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    // Self-assignment guard (e.g., sp = std::move(sp);)
    if (this != &other) {
        // 1. Clean up our current resource if we hold one
        if (mId != 0) {
            glDeleteProgram(mId);
        }

        // 2. Steal resources from the other object
        mId = other.mId;
        attachedShaders = std::move(other.attachedShaders);

        // 3. Reset the donor object
        other.mId = 0;
    }
    return *this;
}

void ShaderProgram::addShader(const char *source, GLenum type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // Check for compilation success
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        // Get the length of the error log
        int infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        // Allocate a buffer and retrieve the log message
        std::string infoLog(infoLogLength, '\0');
        glGetShaderInfoLog(shader, infoLogLength, NULL, &infoLog[0]);

        // Clean up the failed shader before throwing to avoid memory leaks
        glDeleteShader(shader);

        // Translate GLenum to a human-readable string for the exception
        std::string shaderTypeStr = (type == GL_VERTEX_SHADER)   ? "VERTEX" :
                                    (type == GL_FRAGMENT_SHADER) ? "FRAGMENT" :
                                    (type == GL_COMPUTE_SHADER)  ? "COMPUTE" : "UNKNOWN";

        throw std::runtime_error("ERROR::SHADER::" + shaderTypeStr + "::COMPILATION_FAILED\n" + infoLog);
    }

    // Only attach and track if compilation succeeded
    glAttachShader(mId, shader);
    attachedShaders.push_back(shader);
}

void ShaderProgram::link() {
    glLinkProgram(mId);
    for (unsigned int shader : attachedShaders) {
        glDeleteShader(shader);
    }
}

void ShaderProgram::use() {
    glUseProgram(mId);
}

void ShaderProgram::setUniformMat4x4(const std::string &name, const glm::mat4& matrix) {
    const GLint location = getUniformLocation(name);
    if (location == -1) return;
    glUniformMatrix4fv(
        location,
        1,
        GL_FALSE,
        &matrix[0][0]
    );
}

void ShaderProgram::setUniformInt(const std::string &name, int value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) return;
    glUniform1i(location, value);
}

void ShaderProgram::setUniformUInt(const std::string &name, unsigned int value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) return;
    glUniform1ui(location, value);
}

void ShaderProgram::setUniformFloat(const std::string &name, float value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) return;
    glUniform1f(location, value);
}

void ShaderProgram::setUniformBool(const std::string &name, bool value) {
    const GLint location = getUniformLocation(name);
    if (location == -1) return;
    glUniform1i(location, value);
}

void ShaderProgram::setUniform(const ShaderUniform& uniform) {
    // std::visit inspects the variant and passes its active value to the lambda
    std::visit([this, &uniform](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, int>) {
            this->setUniformInt(uniform.name, arg);
        }
        else if constexpr (std::is_same_v<T, float>) {
            this->setUniformFloat(uniform.name, arg);
        }
        else if constexpr (std::is_same_v<T, unsigned int>) {
            this->setUniformUInt(uniform.name, arg);
        }
        else if constexpr (std::is_same_v<T, bool>) {
            this->setUniformBool(uniform.name, arg);
        }
        else if constexpr (std::is_same_v<T, glm::mat4>) {
            this->setUniformMat4x4(uniform.name, arg);
        }
        else if constexpr (std::is_same_v<T, glm::vec3>) {
            this->setUniformVec3(uniform.name, arg);
        }
    }, uniform.data);
}

GLint ShaderProgram::getUniformLocation(const std::string &name) const{
    const GLint location = glGetUniformLocation(mId, name.c_str());

    if (location == -1) {
        //throw std::runtime_error("Could not find uniform: " + name);
    }
    return location;
}

GLint ShaderProgram::getResourceIndex(const std::string &name) const {
    GLuint loc = glGetProgramResourceIndex(mId, GL_SHADER_STORAGE_BLOCK, name.c_str());

    if (loc == GL_INVALID_INDEX) {
        return -1;
    }

    return loc;
}

void ShaderProgram::setUniformVec3(const std::string &name, const glm::vec3& value) {
    GLint loc = glGetUniformLocation(mId, name.c_str());
    if (loc == -1) return;
    glUniform3fv(loc, 1, &value[0]);
}

void ShaderProgram::bindBufferToUniform(const std::string &name, Buffer &buff) {
    const GLint loc = getResourceIndex(name);
    if (loc == -1) {
        return;
    }

    GLuint bindingPoint;
    auto it = mUsedBufferBindings.find(name);

    if (it != mUsedBufferBindings.end()) {
        bindingPoint = it->second;
    } else {
        bindingPoint = mUsedBufferBindings.size();
        mUsedBufferBindings[name] = bindingPoint;
    }

    glShaderStorageBlockBinding(mId, loc, bindingPoint);

    buff.bindBase(bindingPoint);
}
