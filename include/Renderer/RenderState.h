#ifndef SPELTENGINE_RENDERSTATE_H
#define SPELTENGINE_RENDERSTATE_H

#include "glad/glad.h"
#include <cstdint>
#include <cstring>

enum class CullMode  : uint8_t { None, Front, Back, FrontAndBack };
enum class DepthFunc : uint8_t { Less, Equal, LEqual, Greater, Always };

// New Enums for Stencil State
enum class StencilFunc : uint8_t { Never, Less, LEqual, Greater, GEqual, Equal, NotEqual, Always };
enum class StencilOp   : uint8_t { Keep, Zero, Replace, Increment, IncrementWrap, Decrement, DecrementWrap, Invert };

struct RenderState {
    // 1. Toggles compressed into bitfields (1 byte total)
    uint8_t depthTest   : 1 = true;
    uint8_t depthWrite  : 1 = true;
    uint8_t blend       : 1 = false;
    uint8_t cullFace    : 1 = true;
    uint8_t stencilTest : 1 = false;
    uint8_t padding     : 3 = 0;

    // 2. Hardware state configurations (1 byte each)
    CullMode cullMode    = CullMode::Back;
    DepthFunc depthFunc  = DepthFunc::LEqual;

    // 3. Blending Factors
    GLenum srcBlendFactor = GL_SRC_ALPHA;
    GLenum dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;

    // 4. FULL STENCIL STATE ADDITIONS (Total: 6 bytes)
    StencilFunc stencilFunc = StencilFunc::Always; // e.g., GL_ALWAYS
    uint8_t stencilRef      = 0;                   // Reference value for the test
    uint8_t stencilReadMask = 0xFF;                // Mask for testing
    uint8_t stencilWriteMask= 0xFF;                // Mask for writing (glStencilMask)

    StencilOp stencilFailOp = StencilOp::Keep;     // sfail
    StencilOp stencilZFailOp= StencilOp::Keep;     // dpfail
    StencilOp stencilZPassOp= StencilOp::Keep;     // dppass

    // Overload the equality operator (Crucial for state caching!)
    // NOTE: If you add raw GLenum fields, make sure padding bytes don't cause false mismatches.
    bool operator==(const RenderState& other) const {
        return memcmp(this, &other, sizeof(RenderState)) == 0;
    }

    bool operator!=(const RenderState& other) const {
        return !(*this == other);
    }
};

#endif //SPELTENGINE_RENDERSTATE_H
