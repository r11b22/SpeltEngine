//
// Created by joost on 6/12/26.
//

#pragma once

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


namespace Spelt {
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





}


namespace std {
    template <>
    struct hash<Spelt::DrawCommand> {
        std::size_t operator()(const Spelt::DrawCommand& k) const {
            std::size_t seed = 0;
            Spelt::hash_combine(seed, k.shaderName);
            Spelt::hash_combine(seed, k.mesh.getID());
            Spelt::hash_combine(seed, k.material);

            // Hash the sizes of the uniforms as a quick safety check
            Spelt::hash_combine(seed, k.staticUniforms.size());
            Spelt::hash_combine(seed, k.textureUniforms.size());

            return seed;
        }
    };

    template <>
    struct hash<Spelt::StateChangeCommand> {
        std::size_t operator()(const Spelt::StateChangeCommand& k) const {
            std::size_t seed = 0;
            Spelt::hash_combine(seed, k.state);
            return seed;
        }
    };
}
