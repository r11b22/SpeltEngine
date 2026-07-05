#ifndef SPELTENGINE_RENDERSTATE_H
#define SPELTENGINE_RENDERSTATE_H

#include "glad/glad.h"
#include <cstdint>
#include <cstring>
#include <functional>

enum class CullMode  : uint8_t { None, Front, Back, FrontAndBack };
enum class DepthFunc : uint8_t { Less, Equal, LEqual, Greater, Always };

// New Enums for Stencil State
enum class StencilFunc : uint8_t { Never, Less, LEqual, Greater, GEqual, Equal, NotEqual, Always };
enum class StencilOp   : uint8_t { Keep, Zero, Replace, Increment, IncrementWrap, Decrement, DecrementWrap, Invert };

struct RenderState {
    // 1. Move 4-byte primitives to the top (No alignment padding needed)
    GLenum srcBlendFactor = GL_SRC_ALPHA;
    GLenum dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;

    // 2. Hardware configurations & Stencil State (1 byte each)
    CullMode cullMode        = CullMode::Back;
    DepthFunc depthFunc      = DepthFunc::LEqual;
    StencilFunc stencilFunc  = StencilFunc::Always;
    uint8_t stencilRef       = 0;
    uint8_t stencilReadMask  = 0xFF;
    uint8_t stencilWriteMask = 0xFF;

    StencilOp stencilFailOp  = StencilOp::Keep;
    StencilOp stencilZFailOp = StencilOp::Keep;
    StencilOp stencilZPassOp = StencilOp::Keep;

    // 3. Bitfield compressed toggles at the bottom (1 byte total)
    uint8_t depthTest   : 1 = true;
    uint8_t depthWrite  : 1 = true;
    uint8_t blend       : 1 = false;
    uint8_t cullFace    : 1 = true;
    uint8_t stencilTest : 1 = false;
    uint8_t bitfieldPadding : 3 = 0;

    // 4. Explicit padding to guarantee the struct total is a multiple of 4 (Total size: 20 bytes)
    uint8_t structPadding[3] = {0, 0, 0};

    // Safe to use now because there are absolutely zero hidden compiler gaps!
    bool operator==(const RenderState& other) const {
        return memcmp(this, &other, sizeof(RenderState)) == 0;
    }

    bool operator!=(const RenderState& other) const {
        return !(*this == other);
    }
};

namespace std {
    template<>
    struct hash<RenderState> {
        size_t operator()(const RenderState& state) const noexcept {
            // FNV-1a 64-bit fast raw hash
            const char* bytes = reinterpret_cast<const char*>(&state);
            size_t hash = 14695981039346656037ULL;
            for (size_t i = 0; i < sizeof(RenderState); ++i) {
                hash ^= static_cast<size_t>(bytes[i]);
                hash *= 1099511628211ULL;
            }
            return hash;
        }
    };
}

#endif //SPELTENGINE_RENDERSTATE_H
