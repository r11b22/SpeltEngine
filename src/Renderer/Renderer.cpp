//
// Created by joost on 4/12/26.
//

#include "../../include/Renderer/Renderer.h"

#include <Tracy/tracy/Tracy.hpp>
#include <format>
#include <iostream>
#include <type_traits>

#include "Asset/AssetManager.hpp"
#include "Buffer/Buffer.h"
#include "Error/Panic.hpp"
#include "FrameBuffer/MultisampledFrameBuffer.h"
#include "PostProcessing/PostProcessingError.h"
#include "Renderer/Instancing/InstanceData.hpp"
#include "Renderer/Instancing/InstanceRenderer.hpp"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderPass.hpp"
#include "Renderer/RenderQueue.h"
#include "Renderer/RenderState.h"
#include "Strings/ShaderSource.h"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/MultisampledTexture.h"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"
#include "Window.h"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"


namespace Spelt {
    Renderer::Renderer(Window* target)
        :
        mShaderPrograms(),
        mTarget(target)
    {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            fatalPanic("Could not initialize GLAD");
        }

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        // enable alpha channel
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        mScreenShader = new ShaderProgram{};
        mScreenShader->addShader(straightToScreenVertexShader, GL_VERTEX_SHADER);
        mScreenShader->addShader(straightToScreenFragmentShader, GL_FRAGMENT_SHADER);
        mScreenShader->link();

        const std::vector<float> vertices = {
            // Positions        // Normals         // UVs
            -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f
        };
        const std::vector<unsigned int> indices = {
            0, 1, 2,
            0, 2, 3
        };

        mScreenQuad = new Mesh("nonameneeded");
        mScreenQuad->setVertices(vertices);
        mScreenQuad->setIndices(indices);

        mPostProcessingPipeline = new PostProcessingPipeline{mTarget};

        mInputTexture = new MultisampledTexture{mTarget->getWidth(), mTarget->getHeight(), GL_RGB16F, 4};
        mInputFrameBuffer = new MultisampledFrameBuffer{mTarget, 4};

        mInputFrameBuffer->attachTexture(mInputTexture, GL_COLOR_ATTACHMENT0);
        mInputFrameBuffer->setAttachments({GL_COLOR_ATTACHMENT0});

        mInstanceRenderer = new InstanceRenderer(3);
    }

    Result<EffectHandle, PostProcessingError> Renderer::addPostProcessingEffect(PostProcessingEffect effect) {
        return mPostProcessingPipeline->addEffect(std::move(effect));
    }

    Result<EffectHandle, PostProcessingError> Renderer::addPostProcessingEffect(PostProcessingGroup& effect) {
        return effect.apply(*mPostProcessingPipeline);
    }

    void Renderer::enableEffect(EffectHandle handle) {
        mPostProcessingPipeline->enableEffect(handle);
    }

    void Renderer::disableEffect(EffectHandle handle) {
        mPostProcessingPipeline->disableEffect(handle).panicOnError("It is not allowed to disable the requested effect!");
    }


    void Renderer::addShaderProgram(std::string name, std::unique_ptr<ShaderProgram> shaderPrograms) {
        mShaderPrograms.insert({std::move(name), std::move(shaderPrograms)});
    }



    void Renderer::prepare() {
        mPostProcessingPipeline->prepare();
        mStateManager.applyStartState(RenderState());
    }

    void Renderer::executeRenderPass(const RenderPass& pass, const Camera& camera, const std::vector<LightData>& lights){
        ZoneScoped;

        const RenderQueue& queue = pass.getRenderQueue();

        ProjectionType projType = pass.getProjectionType();

        switch (projType) {
            case ProjectionType::Perspective:
            mProjectionMatrix = camera.getPerspectiveProjectionMatrix(*mTarget);
            break;
            case ProjectionType::Orthographic:
            mProjectionMatrix = camera.getOrthographicProjectionMatrix(*mTarget);
            break;
        }

        CameraType camType = pass.getCameraType();
        switch (camType) {
            case CameraType::UI:
            mViewMatrix = glm::mat4{1.0f};
            mCameraPosition = glm::vec3{0.0f};
            break;
            case CameraType::ThreeD:
            mViewMatrix = camera.getViewMatrix();
            mCameraPosition = camera.getPosition();
            break;
        }

        if (mCurrentProgram != nullptr){
            mCurrentProgram->use();
        }



        for (const auto& command : queue.getRenderCommands()) {
            executeRenderCommand(command, camera, lights);
        }
        // reset state
        // THIS IS A HOTFIX AND SHOULD BE REPLACED WITH EVERY RENDER SETTING THE STATE INSTEAD OF ONLY THESE ONES
        mStateManager.applyState(RenderState{});


    }

    void Renderer::drawPass(const std::vector<RenderPass>& passes, const Camera& camera, const std::vector<LightData>& lights) {
        mInputFrameBuffer->bind();
        glClearColor(mScreenClearColor.x, mScreenClearColor.y, mScreenClearColor.z, mScreenClearColor.a);
        glClear(mClearBitField);

        for (const RenderPass& pass : passes){
            // TODO make this  configurable
            glClear(GL_DEPTH_BUFFER_BIT);
            executeRenderPass(pass, camera, lights);
        }

        mInputFrameBuffer->unbind();
        mPostProcessingPipeline->blitToInput(*mInputFrameBuffer);
    }


    void Renderer::renderToScreen() {
        ZoneScoped;
        Texture* output = nullptr;
        mPostProcessingPipeline->process().match(
                [](){},
                [](PostProcessingError error){
                    switch (error) {
                        case PostProcessingError::TextureCountMismatch:
                            fatalPanic("Post-processing pipeline texture count mismatch: Expected and provided texture counts do not align.");
                            break;
                        case PostProcessingError::WrongInitialEntry:
                            fatalPanic("Post-processing failure: The pipeline was initialized with an invalid or unexpected initial entry texture.");
                            break;
                        case PostProcessingError::MultipleApplies:
                            fatalPanic("Post-processing pipeline error: Detected multiple apply attempts on the same frame state.");
                            break;
                        case PostProcessingError::NoEffect:
                            fatalPanic("Post-processing execution failed: The active pipeline has no functional effects bound.");
                            break;
                        case PostProcessingError::InvalidPassIndex:
                            fatalPanic("Post-processing bounds error: Attempted to execute an out-of-bounds or invalid pass index.");
                            break;
                        case PostProcessingError::NoPasses:
                            fatalPanic("Post-processing structural error: The pipeline cannot execute because zero rendering passes are defined.");
                            break;
                        case PostProcessingError::Unprepared:
                            fatalPanic("Post-processing state error: The pipeline attempted to process data before being properly allocated or prepared.");
                            break;
                    }
                }
            );
        output = mPostProcessingPipeline->getOutput();

        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        mScreenShader->use();
        mScreenShader->setUniformInt("screenTexture", 0);
        output->bind(0);
        mScreenQuad->draw(*mScreenShader);

        glEnable(GL_DEPTH_TEST);

    }

    void Renderer::uploadStandardUniforms(ShaderProgram &program, const Camera& camera, const std::vector<LightData>& lights) {
        program.setUniformMat4x4("uProjectionMatrix", mProjectionMatrix);
        program.setUniformMat4x4("uViewMatrix", mViewMatrix);
        program.setUniformVec3("uCameraPos", mCameraPosition);

        mLightManager.applyLightData(program, lights);
    }


    void Renderer::draw(const std::vector<RenderPass>& passes, const Camera& camera, const std::vector<LightData>& lights) {
        ZoneScoped;

        drawPass(passes, camera, lights);
        renderToScreen();
    }

    void Renderer::setClearBits(const GLbitfield bits) {
        mClearBitField = bits;
    }

    void Renderer::setClearColor(glm::vec4 color){
        mScreenClearColor = color;
    }

    void Renderer::setAssetManager(AssetManager* assetManager){
        mAssetManager = assetManager;
    }

    void Renderer::setSetting(GLenum setting, bool value){
        if(value){
            glEnable(setting);
        }else{
            glDisable(setting);
        }
    }

    void Renderer::executeRenderCommand(const RenderCommand& command, const Camera& camera, const std::vector<LightData>& lights) {
        ZoneScoped;
        std::visit([this, &camera, &lights](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, DrawCommand>) {
                this->executeDrawCommand(arg, camera, lights);
            }else if constexpr (std::is_same_v<T, StateChangeCommand>) {
                this->executeStateChangeCommand(arg);
            }else if constexpr (std::is_same_v<T, ClearCommand>) {
                this->executeClearCommand(arg);
            }
        }, command);
    }

    void Renderer::executeDrawCommand(const DrawCommand& command, const Camera& camera, const std::vector<LightData>& lights) {
        ZoneScoped;
        const std::string& shaderName = command.shaderName;
        if(!command.mesh.isNoReference()){
            IRenderable* toRender = mAssetManager->getAsset<Mesh>(command.mesh);
            Material material = command.material;

            ShaderProgram* newProgram = mShaderPrograms.at(shaderName).get();
            if (newProgram != mCurrentProgram) {
                // Set the new shader as the currently active shader
                // RenderQueue optimizes the order as much as possible
                mCurrentProgram = newProgram;
                mCurrentProgram->use();
            }

            uploadStandardUniforms(*mCurrentProgram, camera, lights);

            for (const auto& uniform : command.staticUniforms) {
                mCurrentProgram->setUniform(uniform);
            }

            material.readyMaterial(*mCurrentProgram, *mAssetManager);

            int textureCount = 1;
            for (const auto& textureUniform : command.textureUniforms){
                std::visit([this, &textureCount, &textureUniform](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;

                    if constexpr (std::is_same_v<T, TextureReference>) {
                        Texture* tex = mAssetManager->getAsset<Texture>(arg);
                        tex->bind(textureCount);
                        mCurrentProgram->setUniformInt(textureUniform.name, textureCount);


                    }else if constexpr (std::is_same_v<T, CubemapTextureReference>) {
                        CubemapTexture* tex = mAssetManager->getAsset<CubemapTexture>(arg);
                        tex->bind(textureCount);
                        mCurrentProgram->setUniformInt(textureUniform.name, textureCount);
                    }

                    textureCount++;
                }, textureUniform.data);
            }

            mInstanceRenderer->draw(toRender, *mCurrentProgram, command.instances);
            /*for (const InstanceData& instance : command.instances){
                for(const auto& uniform : instance.getUniforms()){
                    mCurrentProgram->setUniform(uniform);
                }
                toRender->draw(*mCurrentProgram);
            }*/

        }

    }

    void Renderer::executeStateChangeCommand(const StateChangeCommand& command) {
        mStateManager.applyState(command.state);
    }

    void Renderer::executeClearCommand(const ClearCommand& command) {
        ZoneScoped;
        GLbitfield mask = 0;

        if (command.clearColor) {
            glClearColor(command.color[0], command.color[1], command.color[2], command.color[3]);
            mask |= GL_COLOR_BUFFER_BIT;
        }

        if (command.clearDepth) {
            glClearDepth(command.depth);
            mask |= GL_DEPTH_BUFFER_BIT;
        }

        if (command.clearStencil) {
            glClearStencil(command.stencil);
            mask |= GL_STENCIL_BUFFER_BIT;
        }

        // Execute the clear if any flags were set
        if (mask != 0) {
            glClear(mask);
        }
    }

    Renderer::~Renderer() {
        delete mScreenShader;
        delete mScreenQuad;
        delete mPostProcessingPipeline;
        delete mInputFrameBuffer;
        delete mInputTexture;
        delete mInstanceRenderer;
    }
}
