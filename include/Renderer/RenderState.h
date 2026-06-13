//
// Created by joost on 6/13/26.
//

#ifndef SPELTENGINE_RENDERSTATE_H
#define SPELTENGINE_RENDERSTATE_H
#include <cstdint>

enum class CullMode : uint8_t { None, Front, Back, FrontAndBack };
enum class DepthFunc : uint8_t { Less, Equal, LEqual, Greater, Always };

struct RenderState {
    // 1. Toggles compressed into bitfields (takes up only 1 byte total)
    uint8_t depthTest   : 1 = true;
    uint8_t depthWrite  : 1 = true;
    uint8_t blend       : 1 = false;
    uint8_t cullFace    : 1 = true;
    uint8_t stencilTest : 1 = false;
    uint8_t padding     : 3 = 0; // Align to 8-bit boundary

    // 2. State Specifics (1 byte each)
    CullMode cullMode   = CullMode::Back;
    DepthFunc depthFunc = DepthFunc::LEqual;

    // 3. Blending Factors (1 byte each if using enums, or GLenum standard)
    GLenum srcBlendFactor = GL_SRC_ALPHA;
    GLenum dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;

    // Overload the equality operator (Crucial for state caching!)
    bool operator==(const RenderState& other) const {
        return memcmp(this, &other, sizeof(RenderState)) == 0;
    }

    bool operator!=(const RenderState& other) const {
        return !(*this == other);
    }
};

#endif //SPELTENGINE_RENDERSTATE_H