//
// Created by joost on 4/12/26.
//

#ifndef GCVCPROJECT_RENDERER_H
#define GCVCPROJECT_RENDERER_H
#include <map>
#include <memory>
#include <vector>

#include "../Camera.h"
#include "RenderStateManager.h"
#include "../Mesh.h"
#include "../PostProcessing/PostProcessingPipeline.h"
#include "../Scene/Scene.h"
#include "../Window.h"
#include "../PostProcessing/PostProcessingGroup.h"
#include "Renderer/RenderCommand.h"
#include "Shaders/ShaderProgram.h"
#include "Lighting/LightManager.h"
#include "glm/ext/vector_float4.hpp"

class Renderer {
private:
    std::map<std::string, std::unique_ptr<ShaderProgram>> mShaderPrograms;

    ShaderProgram* mScreenShader;
    Mesh* mScreenQuad;

    Window* mTarget;

    PostProcessingPipeline* mPostProcessingPipeline;

    RenderStateManager mStateManager = {};
    LightManager mLightManager = {};
    ShaderProgram* mCurrentProgram = nullptr;

    glm::vec4 mScreenClearColor{0.0f, 0.0f, 0.0f, 1.0f};

    GLbitfield mClearBitField = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
public:
    Renderer(Window* target);
    ~Renderer();


    void addShaderProgram(std::string name, std::unique_ptr<ShaderProgram> shaderPrograms);

    EffectHandle addPostProcessingEffect(PostProcessingEffect effect);
    EffectHandle addPostProcessingEffect(PostProcessingGroup& effect);
    void enableEffect(EffectHandle handle);
    void disableEffect(EffectHandle handle);

    void prepare();

    void draw(const RenderQueue& queue, const Camera& camera, const std::vector<LightData>& lights);

    void setClearBits(GLbitfield bits);

    void setClearColor(glm::vec4 color);

private:
    void drawPass(const RenderQueue& queue, const Camera& camera, const std::vector<LightData>& lights);
    void renderToScreen();

    void executeRenderCommand(const RenderCommand& command, const Camera& camera, const std::vector<LightData>& lights);
    void executeDrawCommand(const DrawCommand& command, const Camera& camera, const std::vector<LightData>& lights);
    void executeStateChangeCommand(const StateChangeCommand& command);
    void executeClearCommand(const ClearCommand& command);

    void uploadStandardUniforms(ShaderProgram& program, const Camera& camera, const std::vector<LightData>& lights);
};


#endif //GCVCPROJECT_RENDERER_H
