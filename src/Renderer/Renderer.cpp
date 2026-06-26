//
// Created by joost on 4/12/26.
//

#include "../../include/Renderer/Renderer.h"

#include <format>
#include <iostream>
#include <type_traits>

#include "Asset/AssetManager.hpp"
#include "FrameBuffer/MultisampledFrameBuffer.h"
#include "Renderer/RenderCommand.h"
#include "Strings/ShaderSource.h"
#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/MultisampledTexture.h"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"
#include "glm/ext/vector_float4.hpp"

Renderer::Renderer(Window* target)
    :
    mShaderPrograms(),
    mTarget(target)
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        throw std::runtime_error("Could not initialize GLAD");
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
}

EffectHandle Renderer::addPostProcessingEffect(PostProcessingEffect effect) {
    return mPostProcessingPipeline->addEffect(std::move(effect));
}

EffectHandle Renderer::addPostProcessingEffect(PostProcessingGroup& effect) {
    return effect.apply(*mPostProcessingPipeline);
}

void Renderer::enableEffect(EffectHandle handle) {
    mPostProcessingPipeline->enableEffect(handle);
}

void Renderer::disableEffect(EffectHandle handle) {
    mPostProcessingPipeline->disableEffect(handle);
}


void Renderer::addShaderProgram(std::string name, std::unique_ptr<ShaderProgram> shaderPrograms) {
    mShaderPrograms.insert({std::move(name), std::move(shaderPrograms)});
}



void Renderer::prepare() {
    mPostProcessingPipeline->prepare();
}

void Renderer::drawPass(const RenderQueue& queue, const Camera& camera, const std::vector<LightData>& lights) {
    mInputFrameBuffer->bind();
    glClearColor(mScreenClearColor.x, mScreenClearColor.y, mScreenClearColor.z, mScreenClearColor.a);
    glClear(mClearBitField);

    if (mCurrentProgram != nullptr){
        mCurrentProgram->use();
    }

    for (const auto& command : queue.getRenderCommands()) {
        executeRenderCommand(command, camera, lights);
    }
    // reset state
    // THIS IS A HOTFIX AND SHOULD BE REPLACED WITH EVERY RENDER SETTING THE STATE INSTEAD OF ONLY THESE ONES
    mStateManager.applyState(RenderState{});

    mInputFrameBuffer->unbind();
    mPostProcessingPipeline->blitToInput(*mInputFrameBuffer);
}


void Renderer::renderToScreen() {

    Texture* output = nullptr;
    mPostProcessingPipeline->process();
    output = mPostProcessingPipeline->getOutput();

    glDisable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mScreenShader->use();
    mScreenShader->setUniformInt("screenTexture", 0);
    output->bind(0);
    mScreenQuad->draw(*mScreenShader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable(GL_DEPTH_TEST);

}

void Renderer::uploadStandardUniforms(ShaderProgram &program, const Camera& camera, const std::vector<LightData>& lights) {
    program.setUniformMat4x4("uProjectionMatrix", camera.getProjectionMatrix(*mTarget));
    program.setUniformMat4x4("uViewMatrix", camera.getViewMatrix());
    program.setUniformVec3("uCameraPos", camera.getPosition());

    mLightManager.applyLightData(program, lights);
}


void Renderer::draw(const RenderQueue& queue, const Camera& camera, const std::vector<LightData>& lights) {
    drawPass(queue, camera, lights);
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

void Renderer::executeRenderCommand(const RenderCommand& command, const Camera& camera, const std::vector<LightData>& lights) {
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
    const std::string& shaderName = command.shaderName;
    if(!command.mesh.isNoReference()){
        IRenderable* toRender = mAssetManager->getMesh(command.mesh);
        Material material = command.material;

        ShaderProgram* newProgram = mShaderPrograms.at(shaderName).get();
        if (newProgram != mCurrentProgram) {
            // Set the new shader as the currently active shader
            // RenderQueue optimizes the order as much as possible
            mCurrentProgram = newProgram;
            mCurrentProgram->use();
        }

        uploadStandardUniforms(*mCurrentProgram, camera, lights);

        for (const auto& uniform : command.uniforms) {
            mCurrentProgram->setUniform(uniform);
        }

         material.readyMaterial(*mCurrentProgram, *mAssetManager);

        int textureCount = 1;
        for (const auto& textureUniform : command.textureUniforms){
            std::visit([this, &textureCount, &textureUniform](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, TextureReference>) {
                    Texture* tex = mAssetManager->getTexture(arg);
                    tex->bind(textureCount);
                    mCurrentProgram->setUniformInt(textureUniform.name, textureCount);


                }else if constexpr (std::is_same_v<T, CubemapTextureReference>) {
                    CubemapTexture* tex = mAssetManager->getCubemap(arg);
                    tex->bind(textureCount);
                    mCurrentProgram->setUniformInt(textureUniform.name, textureCount);
                }

                textureCount++;
            }, textureUniform.data);
        }


        toRender->draw(*mCurrentProgram);
    }

}

void Renderer::executeStateChangeCommand(const StateChangeCommand& command) {
    mStateManager.applyState(command.state);
}

void Renderer::executeClearCommand(const ClearCommand& command) {
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
}
