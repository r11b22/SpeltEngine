//
// Created by joost on 6/13/26.
//

#include "glad/glad.h"
#include "Renderer/RenderStateManager.h"

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
                // Add others...
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

}
