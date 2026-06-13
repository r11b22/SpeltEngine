//
// Created by joost on 6/12/26.
//

#ifndef SPELTENGINE_RENDERENGINE_H
#define SPELTENGINE_RENDERENGINE_H
#include <memory>

#include "IRenderable.h"
#include "Material/Material.h"

struct RenderCommand {
    std::string shaderName = "";
    std::shared_ptr<IRenderable> renderable = nullptr;
    std::shared_ptr<Material> material = nullptr;
    std::vector<ShaderUniform> uniforms = {};
};

#endif //SPELTENGINE_RENDERENGINE_H