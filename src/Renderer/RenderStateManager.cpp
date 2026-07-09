//
// Created by joost on 6/13/26.
//
//
//

#include "Renderer/RenderStateManager.h"
#include "Renderer/RenderState.h"
#include <Tracy/tracy/Tracy.hpp>

// Helper to map custom StencilFunc to GL constants
static GLenum getGLStencilFunc(StencilFunc func) {
    switch (func) {
        case StencilFunc::Never:    return GL_NEVER;
        case StencilFunc::Less:     return GL_LESS;
        case StencilFunc::LEqual:   return GL_LEQUAL;
        case StencilFunc::Greater:  return GL_GREATER;
        case StencilFunc::GEqual:   return GL_GEQUAL;
        case StencilFunc::Equal:    return GL_EQUAL;
        case StencilFunc::NotEqual: return GL_NOTEQUAL;
        case StencilFunc::Always:   return GL_ALWAYS;
    }
    return GL_ALWAYS;
}

// Helper to map custom StencilOp to GL constants
static GLenum getGLStencilOp(StencilOp op) {
    switch (op) {
        case StencilOp::Keep:          return GL_KEEP;
        case StencilOp::Zero:          return GL_ZERO;
        case StencilOp::Replace:       return GL_REPLACE;
        case StencilOp::Increment:     return GL_INCR;
        case StencilOp::IncrementWrap: return GL_INCR_WRAP;
        case StencilOp::Decrement:     return GL_DECR;
        case StencilOp::DecrementWrap: return GL_DECR_WRAP;
        case StencilOp::Invert:        return GL_INVERT;
    }
    return GL_KEEP;
}

// --- Dumb OpenGL Setters ---
// These execute commands and sync mCurrentState unconditionally.

void RenderStateManager::setDepthTestEnabled(bool enabled) {
    enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    mCurrentState.depthTest = enabled;
}

void RenderStateManager::setDepthFunc(DepthFunc func) {
    GLenum glFunc = GL_LESS;
    switch(func) {
        case DepthFunc::Less:    glFunc = GL_LESS; break;
        case DepthFunc::LEqual:  glFunc = GL_LEQUAL; break;
        case DepthFunc::Always:  glFunc = GL_ALWAYS; break;
        case DepthFunc::Greater: glFunc = GL_GREATER; break;
        case DepthFunc::Equal:   glFunc = GL_EQUAL; break;
    }
    glDepthFunc(glFunc);
    mCurrentState.depthFunc = func;
}

void RenderStateManager::setDepthWriteEnabled(bool enabled) {
    glDepthMask(enabled ? GL_TRUE : GL_FALSE);
    mCurrentState.depthWrite = enabled;
}

void RenderStateManager::setCullFaceEnabled(bool enabled) {
    enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    mCurrentState.cullFace = enabled;
}

void RenderStateManager::setCullMode(CullMode mode) {
    GLenum glMode = GL_BACK;
    if (mode == CullMode::Front) glMode = GL_FRONT;
    glCullFace(glMode);
    mCurrentState.cullMode = mode;
}

void RenderStateManager::setBlendEnabled(bool enabled) {
    enabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    mCurrentState.blend = enabled;
}

void RenderStateManager::setBlendFunc(GLenum src, GLenum dst) {
    glBlendFunc(src, dst);
    mCurrentState.srcBlendFactor = src;
    mCurrentState.dstBlendFactor = dst;
}

void RenderStateManager::setStencilTestEnabled(bool enabled) {
    enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
    mCurrentState.stencilTest = enabled;
}

void RenderStateManager::setStencilWriteMask(GLuint mask) {
    glStencilMask(mask);
    mCurrentState.stencilWriteMask = mask;
}

void RenderStateManager::setStencilFunc(StencilFunc func, GLint ref, GLuint mask) {
    glStencilFunc(getGLStencilFunc(func), ref, mask);
    mCurrentState.stencilFunc = func;
    mCurrentState.stencilRef = ref;
    mCurrentState.stencilReadMask = mask;
}

void RenderStateManager::setStencilOp(StencilOp fail, StencilOp zfail, StencilOp zpass) {
    glStencilOp(getGLStencilOp(fail), getGLStencilOp(zfail), getGLStencilOp(zpass));
    mCurrentState.stencilFailOp = fail;
    mCurrentState.stencilZFailOp = zfail;
    mCurrentState.stencilZPassOp = zpass;
}

// --- Core API Implementation ---

void RenderStateManager::applyStartState(const RenderState& newState) {
    // 1. Depth Testing
    setDepthTestEnabled(newState.depthTest);
    setDepthFunc(newState.depthFunc);

    setDepthWriteEnabled(newState.depthWrite);

    // 2. Face Culling
    setCullFaceEnabled(newState.cullFace);
    setCullMode(newState.cullMode);


    // 3. Blending
    setBlendEnabled(newState.blend);
    setBlendFunc(newState.srcBlendFactor, newState.dstBlendFactor);

    // 4. Stencil Testing
    setStencilTestEnabled(newState.stencilTest);
    setStencilWriteMask(newState.stencilWriteMask);
    setStencilFunc(newState.stencilFunc, newState.stencilRef, newState.stencilReadMask);
    setStencilOp(newState.stencilFailOp, newState.stencilZFailOp, newState.stencilZPassOp);

}

void RenderStateManager::applyState(const RenderState &newState) {
    ZoneScoped;
    // 1. Depth Testing
    if (mCurrentState.depthTest != newState.depthTest) {
        setDepthTestEnabled(newState.depthTest);
    }

    if (newState.depthTest) {
        if (mCurrentState.depthFunc != newState.depthFunc) {
            setDepthFunc(newState.depthFunc);
        }
    }

    if (mCurrentState.depthWrite != newState.depthWrite) {
        setDepthWriteEnabled(newState.depthWrite);
    }

    // 2. Face Culling
    if (mCurrentState.cullFace != newState.cullFace) {
        setCullFaceEnabled(newState.cullFace);
    }

    if (newState.cullFace && mCurrentState.cullMode != newState.cullMode) {
        setCullMode(newState.cullMode);
    }

    // 3. Blending
    if (mCurrentState.blend != newState.blend) {
        setBlendEnabled(newState.blend);
    }

    if (newState.blend &&
       (mCurrentState.srcBlendFactor != newState.srcBlendFactor ||
        mCurrentState.dstBlendFactor != newState.dstBlendFactor)) {
        setBlendFunc(newState.srcBlendFactor, newState.dstBlendFactor);
    }

    // 4. Stencil Testing
    if (mCurrentState.stencilTest != newState.stencilTest) {
        setStencilTestEnabled(newState.stencilTest);
    }

    if (mCurrentState.stencilWriteMask != newState.stencilWriteMask) {
        setStencilWriteMask(newState.stencilWriteMask);
    }

    if (newState.stencilTest) {
        if (mCurrentState.stencilFunc     != newState.stencilFunc ||
            mCurrentState.stencilRef      != newState.stencilRef  ||
            mCurrentState.stencilReadMask != newState.stencilReadMask) {

            setStencilFunc(newState.stencilFunc, newState.stencilRef, newState.stencilReadMask);
        }

        if (mCurrentState.stencilFailOp  != newState.stencilFailOp  ||
            mCurrentState.stencilZFailOp != newState.stencilZFailOp ||
            mCurrentState.stencilZPassOp != newState.stencilZPassOp) {

            setStencilOp(newState.stencilFailOp, newState.stencilZFailOp, newState.stencilZPassOp);
        }
    }
}
