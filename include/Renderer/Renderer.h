//
// Created by joost on 4/12/26.
//

#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../Camera.h"
#include "Asset/AssetManager.hpp"
#include "FrameBuffer/MultisampledFrameBuffer.h"
#include "RenderStateManager.h"
#include "Mesh/Mesh.h"
#include "PostProcessing/PostProcessingPipeline.h"
#include "Renderer/Instancing/InstanceRenderer.hpp"
#include "Renderer/RenderPass.hpp"
#include "Scene/Scene.hpp"
#include "../Window.h"
#include "../PostProcessing/PostProcessingGroup.h"
#include "Renderer/RenderCommand.h"
#include "Shader/ShaderProgram.h"
#include "Lighting/LightManager.h"
#include "Texture/MultisampledTexture.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"

namespace Spelt {
    class Renderer {
    private:
        std::map<std::string, std::unique_ptr<ShaderProgram>> mShaderPrograms;

        ShaderProgram* mScreenShader;
        Mesh* mScreenQuad;

        Window* mTarget;

        AssetManager* mAssetManager;

        MultisampledTexture* mInputTexture; // This should someday be replaced with a frame buffer that does not draw to a texture
                                // For now this works
        MultisampledFrameBuffer* mInputFrameBuffer;
        PostProcessingPipeline* mPostProcessingPipeline;

        RenderStateManager mStateManager = {};
        LightManager mLightManager = {};
        ShaderProgram* mCurrentProgram = nullptr;

        InstanceRenderer* mInstanceRenderer;

        glm::vec4 mScreenClearColor{0.0f, 0.0f, 0.0f, 1.0f};

        GLbitfield mClearBitField = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;


        // Renderpass state
        glm::mat4 mViewMatrix = glm::mat4{1.0f};
        glm::vec3 mCameraPosition = glm::vec3{0.0f};
        glm::mat4 mProjectionMatrix = glm::mat4{1.0f};
    public:
        Renderer(Window* target);
        ~Renderer();


        void addShaderProgram(std::string name, std::unique_ptr<ShaderProgram> shaderPrograms);

        EffectHandle addPostProcessingEffect(PostProcessingEffect effect);
        EffectHandle addPostProcessingEffect(PostProcessingGroup& effect);
        void enableEffect(EffectHandle handle);
        void disableEffect(EffectHandle handle);

        void prepare();

        void draw(const std::vector<RenderPass>& passes, const Camera& camera, const std::vector<LightData>& lights);

        void setClearBits(GLbitfield bits);

        void setClearColor(glm::vec4 color);

        void setAssetManager(AssetManager* assetManager);

        void setSetting(GLenum setting, bool value);

    private:
        void executeRenderPass(const RenderPass& pass, const Camera& camera, const std::vector<LightData>& lights);
        void drawPass(const std::vector<RenderPass>& passes, const Camera& camera, const std::vector<LightData>& lights);
        void renderToScreen();

        void executeRenderCommand(const RenderCommand& command, const Camera& camera, const std::vector<LightData>& lights);
        void executeDrawCommand(const DrawCommand& command, const Camera& camera, const std::vector<LightData>& lights);
        void executeStateChangeCommand(const StateChangeCommand& command);
        void executeClearCommand(const ClearCommand& command);

        void uploadStandardUniforms(ShaderProgram& program, const Camera& camera, const std::vector<LightData>& lights);
    };

}
