//
// Created by joost on 6/13/26.
//
//
//

#include "Renderer/RenderStateManager.h"
#include "Renderer/RenderState.h"

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

void RenderStateManager::applyState(const RenderState &newState) {
    // 1. Depth Testing
    if (mCurrentState.depthTest != newState.depthTest) {
        newState.depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        mCurrentState.depthTest = newState.depthTest;
    }

    if (newState.depthTest) {
        if (mCurrentState.depthFunc != newState.depthFunc) {
            GLenum glFunc = GL_LESS;
            switch(newState.depthFunc) {
                case DepthFunc::Less:    glFunc = GL_LESS; break;
                case DepthFunc::LEqual:  glFunc = GL_LEQUAL; break;
                case DepthFunc::Always:  glFunc = GL_ALWAYS; break;
                case DepthFunc::Greater: glFunc = GL_GREATER; break;
                case DepthFunc::Equal:   glFunc = GL_EQUAL; break;
            }
            glDepthFunc(glFunc);
            mCurrentState.depthFunc = newState.depthFunc;
        }
    }

    if (mCurrentState.depthWrite != newState.depthWrite) {
        glDepthMask(newState.depthWrite ? GL_TRUE : GL_FALSE);
        mCurrentState.depthWrite = newState.depthWrite;
    }

    // 2. Face Culling
    if (mCurrentState.cullFace != newState.cullFace) {
        newState.cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        mCurrentState.cullFace = newState.cullFace;
    }

    if (newState.cullFace && mCurrentState.cullMode != newState.cullMode) {
        GLenum glMode = GL_BACK;
        if (newState.cullMode == CullMode::Front) glMode = GL_FRONT;
        glCullFace(glMode);
        mCurrentState.cullMode = newState.cullMode;
    }

    // 3. Blending
    if (mCurrentState.blend != newState.blend) {
        newState.blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        mCurrentState.blend = newState.blend;
    }

    if (newState.blend &&
       (mCurrentState.srcBlendFactor != newState.srcBlendFactor ||
        mCurrentState.dstBlendFactor != newState.dstBlendFactor)) {
        glBlendFunc(newState.srcBlendFactor, newState.dstBlendFactor);
        mCurrentState.srcBlendFactor = newState.srcBlendFactor;
        mCurrentState.dstBlendFactor = newState.dstBlendFactor;
    }

    // 4. Stencil Testing
    if (mCurrentState.stencilTest != newState.stencilTest) {
        newState.stencilTest ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
        mCurrentState.stencilTest = newState.stencilTest;
    }

    // Even if stencil testing is disabled, glStencilMask controls clearing behavior,
    // so update the mask configuration independently of the toggle status.
    if (mCurrentState.stencilWriteMask != newState.stencilWriteMask) {
        glStencilMask(newState.stencilWriteMask);
        mCurrentState.stencilWriteMask = newState.stencilWriteMask;
    }

    if (newState.stencilTest) {
        // Update function, reference value, and read mask
        if (mCurrentState.stencilFunc     != newState.stencilFunc ||
            mCurrentState.stencilRef      != newState.stencilRef  ||
            mCurrentState.stencilReadMask != newState.stencilReadMask) {

            glStencilFunc(
                getGLStencilFunc(newState.stencilFunc),
                newState.stencilRef,
                newState.stencilReadMask
            );

            mCurrentState.stencilFunc     = newState.stencilFunc;
            mCurrentState.stencilRef      = newState.stencilRef;
            mCurrentState.stencilReadMask = newState.stencilReadMask;
        }

        // Update actions based on test results
        if (mCurrentState.stencilFailOp  != newState.stencilFailOp  ||
            mCurrentState.stencilZFailOp != newState.stencilZFailOp ||
            mCurrentState.stencilZPassOp != newState.stencilZPassOp) {

            glStencilOp(
                getGLStencilOp(newState.stencilFailOp),
                getGLStencilOp(newState.stencilZFailOp),
                getGLStencilOp(newState.stencilZPassOp)
            );

            mCurrentState.stencilFailOp  = newState.stencilFailOp;
            mCurrentState.stencilZFailOp = newState.stencilZFailOp;
            mCurrentState.stencilZPassOp = newState.stencilZPassOp;
        }
    }
}
