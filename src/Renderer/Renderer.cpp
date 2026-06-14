//
// Created by joost on 4/12/26.
//

#include "../../include/Renderer/Renderer.h"

#include <iostream>
#include <type_traits>

#include "Renderer/RenderCommand.h"
#include "Strings/ShaderSource.h"

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

    mScreenQuad = new Mesh();
    mScreenQuad->setVertices(vertices);
    mScreenQuad->setIndices(indices);

    mPostProcessingPipeline = new PostProcessingPipeline{mTarget};
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

void Renderer::drawPass(const RenderQueue& queue, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight) {
    mPostProcessingPipeline->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(mClearBitField);

    if (mCurrentProgram != nullptr){
        mCurrentProgram->use();
    }

    for (const auto& command : queue.getRenderCommands()) {
        executeRenderCommand(command, camera, pointLights, ambientLight);
    }
    // reset state
    // THIS IS A HOTFIX AND SHOULD BE REPLACED WITH EVERY RENDER SETTING THE STATE INSTEAD OF ONLY THESE ONES
    mStateManager.applyState(RenderState{});

    mPostProcessingPipeline->unbind();
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

void Renderer::uploadStandardUniforms(ShaderProgram &program, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight) {
    program.setUniformMat4x4("uProjectionMatrix", camera.getProjectionMatrix(*mTarget));
    program.setUniformMat4x4("uViewMatrix", camera.getViewMatrix());
    program.setUniformVec3("uCameraPos", camera.getPosition());

    uploadLightData(program, pointLights, ambientLight);
}


void Renderer::draw(const RenderQueue& queue, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight) {
    drawPass(queue, camera, pointLights, ambientLight);
    renderToScreen();
}

void Renderer::setClearBits(const GLbitfield bits) {
    mClearBitField = bits;
}

void Renderer::executeRenderCommand(const RenderCommand& command, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight) {
    std::visit([this, &camera, &pointLights, &ambientLight](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, DrawCommand>) {
            this->executeDrawCommand(arg, camera, pointLights, ambientLight);
        }else if constexpr (std::is_same_v<T, StateChangeCommand>) {
            this->executeStateChangeCommand(arg);
        }else if constexpr (std::is_same_v<T, ClearCommand>) {
            this->executeClearCommand(arg);
        }
    }, command);
}

void Renderer::executeDrawCommand(const DrawCommand& command, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight) {
    const std::string& shaderName = command.shaderName;
    const std::shared_ptr<IRenderable>& toRender = command.renderable;
    const std::shared_ptr<Material>& material = command.material;

    ShaderProgram* newProgram = mShaderPrograms.at(shaderName).get();
    if (newProgram != mCurrentProgram) {
        // Set the new shader as the currently active shader
        // RenderQueue optimizes the order as much as possible
        mCurrentProgram = newProgram;
        mCurrentProgram->use();
    }

    uploadStandardUniforms(*mCurrentProgram, camera, pointLights, ambientLight);

    for (const auto& uniform : command.uniforms) {
        mCurrentProgram->setUniform(uniform);
    }

    material->readyMaterial(*mCurrentProgram);
    toRender->draw(*mCurrentProgram);
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

void Renderer::uploadLightData(ShaderProgram& program, const std::vector<std::shared_ptr<PointLight>>& points, std::shared_ptr<AmbientLight> ambient) {

    if (ambient) {
        program.setUniformVec3("uAmbient", ambient->getAmbient());
    }

    program.setUniformInt("numPointLights", static_cast<int>(points.size()));

    for (size_t i = 0; i < points.size(); ++i) {
        std::string base = "uPointLights[" + std::to_string(i) + "].";
        program.setUniformVec3(base + "position", points[i]->getPosition());
        program.setUniformVec3(base + "diffuse", points[i]->getDiffuse());
        program.setUniformVec3(base + "specular", points[i]->getSpecular());

        program.setUniformFloat(base + "constant", points[i]->getConstantFalloff());
        program.setUniformFloat(base + "lineair", points[i]->getLinearFalloff());
        program.setUniformFloat(base + "quadratic", points[i]->getQuadraticFallof());
    }
}

Renderer::~Renderer() {
    delete mScreenShader;
    delete mScreenQuad;
    delete mPostProcessingPipeline;
}
