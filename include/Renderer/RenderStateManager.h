//
// Created by joost on 6/13/26.
//

#pragma once

#include "RenderState.h"

namespace Spelt {
    class RenderStateManager {
    public:
        void applyStartState(const RenderState& newState);
        void applyState(const RenderState& newState);

    private:
        RenderState mCurrentState;

        // --- Dumb OpenGL Setters ---
        // Depth Testing Helpers
        void setDepthTestEnabled(bool enabled);
        void setDepthFunc(DepthFunc func);
        void setDepthWriteEnabled(bool enabled);

        // Face Culling Helpers
        void setCullFaceEnabled(bool enabled);
        void setCullMode(CullMode mode);

        // Blending Helpers
        void setBlendEnabled(bool enabled);
        void setBlendFunc(GLenum src, GLenum dst);

        // Stencil Testing Helpers
        void setStencilTestEnabled(bool enabled);
        void setStencilWriteMask(GLuint mask);
        void setStencilFunc(StencilFunc func, GLint ref, GLuint mask);
        void setStencilOp(StencilOp fail, StencilOp zfail, StencilOp zpass);
    };

}
