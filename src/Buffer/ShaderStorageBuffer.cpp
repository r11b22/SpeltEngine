#include "Buffer/ShaderStorageBuffer.hpp"
#include "Buffer/Buffer.h"


ShaderStorageBuffer::ShaderStorageBuffer(GLenum usage)
    : Buffer(GL_SHADER_STORAGE_BUFFER, usage)
{

}
