//
// Created by joost on 4/12/26.
//

#include "../../include/Renderer/Renderer.h"

#include <iostream>
#include <algorithm>


#include "FileReader.h"
#include "Strings/ShaderSource.h"

Renderer::Renderer(Window* target)
    :
    mShaderPrograms(),
    mPickBuffer(nullptr),
    mPickShader{nullptr},
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

    mPickShader = new ShaderProgram{};
    mPickShader->addShader(pickingVertex, GL_VERTEX_SHADER);
    mPickShader->addShader(pickingFragment, GL_FRAGMENT_SHADER);
    mPickShader->link();

    mPickBuffer = new PickBuffer(target);

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


void Renderer::addRenderPass(std::unique_ptr<IRenderPass> perObjectRenderPass) {
    mRenderPasses.push_back(std::move(perObjectRenderPass));
}

void Renderer::prepare() {
    mPostProcessingPipeline->prepare();
}

void Renderer::drawPass(const Scene& scene, const Camera& camera) {
    mPostProcessingPipeline->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& pass : mRenderPasses) {
        std::optional<std::string> customShader = pass->getCustomShaderName();

        ShaderProgram* currentProgram = nullptr;

        if (customShader.has_value()) {
            currentProgram = mShaderPrograms.at(customShader.value()).get();
        }

        for (const auto& [shader, drawables] : scene.getRenderQueue().getDrawables()) {
            if (!customShader.has_value()) {
                currentProgram = mShaderPrograms.at(shader).get();
            }

            currentProgram->use();
            currentProgram->setUniformMat4x4("uProjectionMatrix", camera.getProjectionMatrix(*mTarget));
            currentProgram->setUniformMat4x4("uViewMatrix", camera.getViewMatrix());


            currentProgram->setUniformVec3("uCameraPos", camera.getPosition());
            currentProgram->setUniformInt("numPointLights", scene.getPointLights().size());
            if (const auto& ambient = scene.getAmbientLight()) {
                currentProgram->setUniformVec3("uAmbient", ambient->getAmbient());
            }

            uploadLightDataToShader(*currentProgram, scene.getPointLights());

            for (const std::shared_ptr<IDrawable>& toDraw : drawables) {
                pass->drawObject(toDraw, *currentProgram);
            }
        }
    }


    mPostProcessingPipeline->unbind();
}

void Renderer::pickingPass(const Scene& scene, const Camera& camera) {
    glDisable(GL_BLEND);
    mPickBuffer->bind();


    uint32_t clearID = 0;
    uint32_t clearValues[4] = { clearID, 0, 0, 0 };
    glClearBufferuiv(GL_COLOR, 0, clearValues);
    glClear( GL_DEPTH_BUFFER_BIT);

    mPickShader->use();
    mPickShader->setUniformMat4x4("uProjectionMatrix", camera.getProjectionMatrix(*mTarget));
    mPickShader->setUniformMat4x4("uViewMatrix", camera.getViewMatrix());

    const auto& drawables = scene.getRenderQueue().getDrawablesOrdered();
    for (int i =0; i < drawables.size(); i++) {

            mPickShader->setUniformUInt("uObjectIndex", i+1);

            drawables[i]->draw(*mPickShader);

    }


    mPickBuffer->unbind();
    glEnable(GL_BLEND);
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

std::shared_ptr<IDrawable> Renderer::getObjectAtPixel(const Scene& scene, int x, int y) const{
    unsigned int idx = mPickBuffer->getAtPixel(x, y);
    if (idx > 0) {
        std::shared_ptr<IDrawable> object = scene.getRenderQueue().getDrawablesOrdered()[idx-1];
        return object;
    }
    return nullptr;
}

void Renderer::drawScene(const Scene& scene) {
    pickingPass(scene, *scene.getCamera());
    drawPass(scene, *scene.getCamera());
    renderToScreen();
}

void Renderer::uploadLightDataToShader(ShaderProgram& program, const std::vector<std::shared_ptr<PointLight>>& lights) {
    program.setUniformInt("numPointLights", static_cast<int>(lights.size()));

    for (size_t i = 0; i < lights.size(); ++i) {
        std::string base = "uPointLights[" + std::to_string(i) + "].";
        program.setUniformVec3(base + "position", lights[i]->getPosition());
        program.setUniformVec3(base + "diffuse", lights[i]->getDiffuse());
        program.setUniformVec3(base + "specular", lights[i]->getSpecular());

        program.setUniformFloat(base + "constant", lights[i]->getConstantFalloff());
        program.setUniformFloat(base + "lineair", lights[i]->getLinearFalloff());
        program.setUniformFloat(base + "quadratic", lights[i]->getQuadraticFallof());
    }
}

Renderer::~Renderer() {
    delete mPickBuffer;
    delete mPickShader;
    delete mScreenShader;
    delete mScreenQuad;
    delete mPostProcessingPipeline;
}
