//
// Created by joost on 6/12/26.
//

#ifndef SPELTENGINE_RENDERENGINE_H
#define SPELTENGINE_RENDERENGINE_H
#include <memory>
#include <variant>
#include <vector>

#include "IRenderable.h"
#include "Mesh/MeshReference.hpp"
#include "RenderState.h"
#include "Material/Material.h"
#include "Renderer/Instancing/InstanceData.hpp"
#include "Shader/ShaderUniform.h"
#include "Texture/TextureUniform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "Hashing/Hashing.hpp"



struct DrawCommand {
    std::string shaderName = "";
    MeshReference mesh;
    Material material{"noname", glm::vec3{1.0f}};
    std::vector<ShaderUniform> staticUniforms = {};
    std::vector<TextureUniform> textureUniforms = {};

    bool isInstanceOf(const DrawCommand& other) const;


    std::vector<InstanceData> instances = {};
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



namespace std {
    template <>
    struct hash<DrawCommand> {
        std::size_t operator()(const DrawCommand& k) const {
            std::size_t seed = 0;
            hash_combine(seed, k.shaderName);
            hash_combine(seed, k.mesh.getID());
            hash_combine(seed, k.material);

            // Hash the sizes of the uniforms as a quick safety check
            hash_combine(seed, k.staticUniforms.size());
            hash_combine(seed, k.textureUniforms.size());

            return seed;
        }
    };

    template <>
    struct hash<StateChangeCommand> {
        std::size_t operator()(const StateChangeCommand& k) const {
            std::size_t seed = 0;
            hash_combine(seed, k.state);
            return seed;
        }
    };
}

#endif //SPELTENGINE_RENDERENGINE_H
