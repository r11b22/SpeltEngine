//
// Created by joost on 6/12/26.
//

#ifndef SPELTENGINE_RENDERENGINE_H
#define SPELTENGINE_RENDERENGINE_H
#include <memory>
#include <variant>

#include "IRenderable.h"
#include "RenderState.h"
#include "Material/Material.h"


struct DrawCommand {
    std::string shaderName = "";
    std::shared_ptr<IRenderable> renderable = nullptr;
    std::shared_ptr<Material> material = nullptr;
    std::vector<ShaderUniform> uniforms = {};
};

struct StateChangeCommand {
    RenderState state;
};

struct ClearCommand {
    bool clearColor   = true;
    bool clearDepth   = true;
    bool clearStencil = false;

    float color[4]    = { 0.0f, 0.0f, 0.0f, 1.0f };
    float depth       = 1.0f;
    int stencil       = 0;
};

using RenderCommand = std::variant<
    DrawCommand,
    StateChangeCommand,
    ClearCommand
>;




#endif //SPELTENGINE_RENDERENGINE_H
