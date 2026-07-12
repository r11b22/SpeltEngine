#include "Buffer/ShaderStorageBuffer.hpp"
#include "Buffer/Buffer.h"


namespace Spelt {
    ShaderStorageBuffer::ShaderStorageBuffer(GLenum usage)
        : Buffer(GL_SHADER_STORAGE_BUFFER, usage)
    {

    }
}
