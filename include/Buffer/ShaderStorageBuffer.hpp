#pragma once


#include "Buffer/Buffer.h"

namespace Spelt {
    class ShaderStorageBuffer : public Buffer {
        private:
        public:
            ShaderStorageBuffer(GLenum usage);
        private:
    };
}
